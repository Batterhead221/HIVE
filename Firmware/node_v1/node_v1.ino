#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

typedef struct {
  char nodeId[16];
  uint32_t counter;
  uint32_t uptimeMs;
} HeartbeatPacket;

HeartbeatPacket packet;

uint8_t broadcastAddress[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

uint32_t lastSendMs = 0;
uint32_t counter = 0;

void onSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("Send callback: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("SUCCESS");
  } else {
    Serial.println("FAILED");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== ESP32-S3 OUTLET NODE ===");

  WiFi.mode(WIFI_STA);
  delay(200);
  WiFi.disconnect(false);
  delay(200);

  esp_wifi_set_protocol(
    WIFI_IF_STA,
    WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N
  );

  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  Serial.print("Node MAC: ");
  Serial.println(WiFi.macAddress());

  uint8_t channel;
  wifi_second_chan_t second;
  esp_wifi_get_channel(&channel, &second);
  Serial.print("Node channel: ");
  Serial.println(channel);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(onSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add broadcast peer");
    return;
  }

  Serial.println("Outlet node started");
}

void loop() {
  if (millis() - lastSendMs >= 5000) {
    lastSendMs = millis();

    strcpy(packet.nodeId, "KITCHEN");
    packet.counter = counter++;
    packet.uptimeMs = millis();

    esp_err_t result = esp_now_send(
      broadcastAddress,
      (uint8_t *)&packet,
      sizeof(packet)
    );

    Serial.print("esp_now_send result: ");
    Serial.println(result);
  }
}