#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Arduino_JSON.h>
#include <HTTPClient.h>

#define POWER_OFF '0'
#define POWER_ON '1'

#define LED_BUILTIN 2

// Set sensor id
#define SENSOR_ID 0

// uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x65, 0x6F, 0x28};
uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x65, 0x68, 0xEC};

const char *ssid = "VaiCorinthians2.4";
const char *password = "23132211";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.0.9:5000/setDoorStatus/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

AsyncWebServer server(80);

// Structure to send data
// Must match the receiver structure
typedef struct struct_message
{
  int id = SENSOR_ID; // Id that represents which device it is
  char cmd;
} struct_message;

char cmd = POWER_ON;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

void updateDoorSensorStatus(String mac_addr, String status) {
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
  
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"macAddress\":\"" + mac_addr + "\",\"status\":" + status + "}");
    
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

// callback function that will be executed when data is received
void onDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
  // Copies the sender mac address to a string
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&myData, incomingData, sizeof(myData));

  String status = myData.cmd == POWER_ON ? "true" : "false";

  // updateDoorSensorStatus(macStr, status);
  updateDoorSensorStatus("3C:61:05:65:68:EC", status);

  Serial.printf("Sensor mac: ");
  Serial.println(macStr);
  Serial.printf("Status: %s \n", status);
  Serial.println();
}

// callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void sendEspNowData(struct_message data, uint8_t broadcastAddress[])
{
  Serial.println(data.cmd);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&data, sizeof(data));

  if (result == ESP_OK)
  {
    Serial.print("Sent command: ");
    Serial.print(data.cmd);
    Serial.println(" with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
}

void turnAirOn(uint8_t broadcastAddress[])
{
  struct_message data;
  data.cmd = POWER_ON;
  sendEspNowData(data, broadcastAddress);
}

void turnAirOff(uint8_t broadcastAddress[])
{
  struct_message data;
  data.cmd = POWER_OFF;
  sendEspNowData(data, broadcastAddress);
}

void addPeer(uint8_t broadcastAddress[]) {
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
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

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);

  int32_t channel = getWiFiChannel(ssid);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }

  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", "index_html"); 
  });

  server.on("/turnOn", HTTP_POST, [](AsyncWebServerRequest *request) {
    int paramsNr = request->params(); // number of params (e.g., 1)

    AsyncWebParameter * command = request->getParam(0); // command
    String commandValue = command->value();

    AsyncWebParameter * mac = request->getParam(1); // mac
    String macValue = mac->value();
    std::vector<uint8_t> myVector(macValue.begin(), macValue.end());
    uint8_t *broadcastAddress = &myVector[0];

    addPeer(broadcastAddress);

    if (commandValue == "on") {
      turnAirOn(broadcastAddress);
      request->send(200);
    } else if (commandValue == "off") {
      turnAirOff(broadcastAddress);
      request->send(200);
    } else {
      request->send(404);
    }
  });

  server.begin();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  addPeer(broadcastAddress);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  delay(1000);
}

String status = "true";

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // updateDoorSensorStatus("3C:61:05:65:68:EC", status);
    lastTime = millis();
    if (status == "true") {
      status = "false";
    } else {
      status = "true";
    }
  }
}