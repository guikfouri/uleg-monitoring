#include <Arduino.h>
#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
#include "IRremote/IRremote.hpp"
#include "IRremote/ac_LG.hpp"
#include "WiFi.h"
#include <esp_now.h>
#include <esp_wifi.h>

/*
 * LG2 has different header timing and a shorter bit time
 * Known LG remote controls, which uses LG2 protocol are:
 * AKB75215403
 * AKB74955603
 * AKB73757604:
 */
// #define USE_LG2_PROTOCOL // Try it if you do not have success with the default LG protocol
#define NUMBER_OF_COMMANDS_BETWEEN_PRINT_OF_MENU 5

#define DISABLE_CODE_FOR_RECEIVER // Disables restarting receiver after each send. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not used.

#define INFO // Deactivate this to save program memory and suppress info output from the LG-AC driver.
// #define DEBUG // Activate this for more output from the LG-AC driver.

#define POWER_OFF '0';
#define POWER_ON '1';

#define LED_BUILTIN 2

// Set sensor id
#define SENSOR_ID 1

#define SIZE_OF_RECEIVE_BUFFER 10

typedef struct struct_message
{
    int id = SENSOR_ID; // Id that represents which device it is
    char cmd;
} struct_message;

char tCommand = LG_COMMAND_ON;

// Create a struct_message called myData
struct_message myData;

char sRequestString[SIZE_OF_RECEIVE_BUFFER];

Aircondition_LG MyLG_Aircondition;

// Insert your SSID
constexpr char WIFI_SSID[] = "VaiCorinthians2.4";

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&myData, incomingData, sizeof(myData));

    Serial.print("Bytes received: ");
    Serial.println(len);

    char command = myData.cmd;
    int tParameter = 0;

    /*
     * Print command to send
     */
    Serial.println();
    Serial.print(F("Command="));
    Serial.print(command);
    if (tParameter != 0)
    {
        Serial.print(F(" Parameter="));
        Serial.print(tParameter);
    }
    Serial.println();

    if (!MyLG_Aircondition.sendCommandAndParameter(command, tParameter))
    {
        Serial.print(F("Error: unknown command or invalid parameter in \""));
        Serial.print(sRequestString);
        Serial.println('\"');
    }
}

int32_t getWiFiChannel(const char *ssid)
{
    if (int32_t n = WiFi.scanNetworks())
    {
        for (uint8_t i = 0; i < n; i++)
        {
            if (!strcmp(ssid, WiFi.SSID(i).c_str()))
            {
                return WiFi.channel(i);
            }
        }
    }
    return 0;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    // Initialize Serial Monitor
    Serial.begin(115200);
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    int32_t channel = getWiFiChannel(WIFI_SSID);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_recv_cb(OnDataRecv);

    /*
     * The IR library setup. That's all!
     */
    IrSender.begin(); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin

    Serial.println();
    MyLG_Aircondition.setType(LG_IS_WALL_TYPE);

    delay(1000);
}

void loop() {}
