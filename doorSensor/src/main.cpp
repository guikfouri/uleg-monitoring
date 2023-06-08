#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "WiFi.h"

#define DOOR_OPEN '0'
#define DOOR_CLOSED '1'

#define LED_BUILTIN 2

#define  SENSOR_PIN 4

// Set sensor id
#define SENSOR_ID 0

uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x65, 0x6F, 0x28};

const char *ssid = "VaiCorinthians2.4";

// Structure to send data
// Must match the receiver structure
typedef struct struct_message
{
  int id = SENSOR_ID; // Id that represents which device it is
  char cmd;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void sendEspNowData(struct_message data) {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&data, sizeof(data));

  if (result == ESP_OK) {
    Serial.print("Sent command: ");
    Serial.print(data.cmd);
    Serial.println(" with success");
  } else {
    Serial.println("Error sending the data");
  }
}

void sendDoorOpen() {
  struct_message data;
  data.cmd = DOOR_OPEN;
  sendEspNowData(data);
}

void sendDoorClosed() {
  struct_message data;
  data.cmd = DOOR_CLOSED;
  sendEspNowData(data);
}

void addPeer() {
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
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

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_STA);

  int32_t channel = getWiFiChannel(ssid);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  addPeer();

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(onDataSent);

  pinMode (SENSOR_PIN, INPUT);

  delay(1000);
}

char doorStatus = DOOR_OPEN;
int previousRead = 0;
bool isVariating = false;
bool firstVariation = false;
unsigned long previousMillistemprst = 0;
const int tempresetinterval = 1000;

void loop() {
  unsigned long currentMillis = millis();
  int currentRead = digitalRead(SENSOR_PIN);

  // Verifica se está variando
  if (currentRead != previousRead) {
    if (firstVariation == false) {
      firstVariation = true;
    } else {
      isVariating = true;
    }
  }

  // Após 1 segundo
  if (currentMillis - previousMillistemprst > tempresetinterval) {
    // Se no espaço de um segundo não houver variação considera porta fechada
    if (isVariating == false && currentRead == LOW && doorStatus == DOOR_OPEN) {
      doorStatus = DOOR_CLOSED;
      sendDoorClosed();
    }

    // Se no espaço de um segundo houver variação considera porta aberta
    if (isVariating == true && doorStatus == DOOR_CLOSED) {
      doorStatus = DOOR_OPEN;
      sendDoorOpen();
    }

    isVariating = false;
    firstVariation = false;
    previousMillistemprst = currentMillis;
  }

  previousRead = currentRead;
}
