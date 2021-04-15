
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Set the Wi-Fi credentials
const char *ssid = "YourWiFiSSID";
const char *password = "YourWiFiPassword";

// Set the Telegram parameters
String BOTtoken = "1234567890:ABCdefgHiJk0MNO1PQrsT1U2_V3Xyz25AYX"; // Your bot token
String CHAT_ID = "1234567891011"; // Your chat ID


int requestDelay = 1000; // Time in ms to check for new messages
unsigned long botLastTime; // The time of the last checked for a message
bool flashState = false; // The boolean to turn on or off the onboard led

// Sets the Telegram
WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

// A function with led bling to exemplify
void ledBlink()
{
    for (int a = 0; a < 20; a++)
    {
        digitalWrite(2, HIGH);
        delay(500);
        digitalWrite(2, LOW);
        delay(500);
    }
}

// The function to handle new telegram messages
// here you can implement new functions/commands
void handleNewMessages(int numNewMessages)
{
    Serial.print("Handle New Messages: ");
    Serial.println(numNewMessages);

    for (int i = 0; i < numNewMessages; i++)
    {
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != CHAT_ID)
        {
            bot.sendMessage(chat_id, "Unauthorized user", "");
            continue;
        }

        // Print the received message
        String text = bot.messages[i].text;
        Serial.print("Received message: ");
        Serial.println(text);

        // Get the name of Telegram user
        String from_name = bot.messages[i].from_name;

        // Verify the received command and execute the action
        if (text == "/bot")
        {
            // Run the bot begin command
            String welcome = "Bem-vindo , " + from_name + ". Use esses comandos:\n";
            welcome += "\n/flash - Liga o flash;";
            welcome += "\n/led - Dispara o led.";
            welcome += "\n\nDigite /bot para ver essa lista de comandos novamente.";
            bot.sendMessage(CHAT_ID, welcome, "");
        }
        else if (text == "/flash")
        {
            // Run the flash command
            flashState = !flashState;
            
            if (flashState)
            {
                digitalWrite(4, HIGH);
                bot.sendMessage(CHAT_ID, "Flash ativado!", "");
                Serial.println("Flash on!");
            }
            else
            {
                digitalWrite(4, LOW);
                bot.sendMessage(CHAT_ID, "Flash desativado!", "");
                Serial.println("Flash off!");
            }
        }
        else if (text == "/led")
        {
            // Run the led command
            bot.sendMessage(CHAT_ID, "LED blink ativado!", "");
            Serial.println("LED blink on!");
            ledBlink();
        }
    }
}

void setup()
{
    // Init Serial Monitor
    Serial.begin(115200);
    delay(1000);

    // Set onboard LED/Flash as output and turn off
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);

    // Set LED as output and turn off
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    // Add root certificate for api.telegram.org
    clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); 

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.print("\nESP32-CAM IP: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    if (millis() > botLastTime + requestDelay)
    {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        while (numNewMessages)
        {
            Serial.println("Reply...");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        botLastTime = millis();
    }
}