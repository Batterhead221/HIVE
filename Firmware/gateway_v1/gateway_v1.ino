#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

typedef struct {
  char nodeId[16];
  uint32_t counter;
  uint32_t uptimeMs;
} HeartbeatPacket;

HeartbeatPacket incomingPacket;

uint32_t lastSeenMs = 0;
uint32_t lastPrintMs = 0;
bool nodeHasBeenSeen = false;

const uint32_t TIMEOUT_MS = 20000;
const uint32_t PRINT_MS = 2000;

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  Serial.print("RX packet len=");
  Serial.println(len);

  if (len == sizeof(HeartbeatPacket)) {
    memcpy(&incomingPacket, data, sizeof(incomingPacket));

    lastSeenMs = millis();
    nodeHasBeenSeen = true;

    Serial.print("Heartbeat from ");
    Serial.print(incomingPacket.nodeId);
    Serial.print(" | count=");
    Serial.print(incomingPacket.counter);
    Serial.print(" | uptime=");
    Serial.println(incomingPacket.uptimeMs);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== ESP32-C6 GATEWAY RECEIVER ===");

  WiFi.mode(WIFI_STA);
  delay(200);
  WiFi.disconnect(false);
  delay(200);

  esp_wifi_set_protocol(
    WIFI_IF_STA,
    WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N
  );

  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  Serial.print("Gateway MAC: ");
  Serial.println(WiFi.macAddress());

  uint8_t channel;
  wifi_second_chan_t second;
  esp_wifi_get_channel(&channel, &second);
  Serial.print("Gateway channel: ");
  Serial.println(channel);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  Serial.println("Gateway started");
}

void loop() {
  if (millis() - lastPrintMs >= PRINT_MS) {
    lastPrintMs = millis();

    if (!nodeHasBeenSeen) {
      Serial.println("NODE_01 = UNKNOWN / waiting for first heartbeat");
    } else {
      uint32_t age = millis() - lastSeenMs;

      if (age < TIMEOUT_MS) {
        Serial.print(incomingPacket.nodeId);
        Serial.print(" = ON | last seen ");
        Serial.print(age);
        Serial.println(" ms ago");
      } else {
        Serial.print(incomingPacket.nodeId);
        Serial.print(" = OFF / missing | last seen ");
        Serial.print(age);
        Serial.println(" ms ago");
      }
    }
  }
}