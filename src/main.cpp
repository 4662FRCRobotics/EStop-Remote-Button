#include <Arduino.h>
#include <ezButton.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

int iStopPin = D7;
int iGreenPin = D6;
int iRedPin = D5;

boolean bEStopOn = false;

const unsigned long lKeyDebounce = 50;

ezButton EStopButton(iStopPin);


void formatMacAddress (const uint8 *macAddr, char *buffer, int maxLength){
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
}



void sentCallback(uint8_t *macAddr, uint8 status){
char macStr[18];
formatMacAddress(macAddr, macStr, 18);
}
void setup() {
  // put your setup code here, to run once:
pinMode(iGreenPin, OUTPUT);
pinMode(iRedPin, OUTPUT);
EStopButton.setDebounceTime(lKeyDebounce);

Serial.begin(115200);
delay(1000);

WiFi.mode(WIFI_STA);
WiFi.disconnect() == 0;

if (esp_now_init() == 0)
{esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
esp_now_register_send_cb(sentCallback);
}
else
{
  Serial.println("ESP-NOW init failed");
delay(3000);
ESP.restart();
}

}

void broadcast(const String &message) 

{
  u8 broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  u8 peerRoleNull = {};
  u8 peerChannelNull = {};
  u8 *peerKeyNull = {};
  u8 peerKeyLenNull = {};
  if (!esp_now_is_peer_exist(broadcastAddress))
  {
    esp_now_add_peer(broadcastAddress, peerRoleNull, peerChannelNull, peerKeyNull, peerKeyLenNull);
  }
  uint8 result = esp_now_send(NULL, (u8 *)message.c_str(), message.length());

  if (result == 0)
  {
    Serial.println("Broadcast message success");
  }
  else 
  {
    Serial.println("Unknown error, try again lol");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  EStopButton.loop();

  if (EStopButton.isReleased()) {
    bEStopOn = true;
    broadcast("estop");
    Serial.println("estop");
  }
  else {
    if (EStopButton.isPressed()) {
      bEStopOn = false;
      broadcast("clear");
      Serial.println("clear");
    }

  }
  if (bEStopOn) {
    digitalWrite(iGreenPin, LOW);
    digitalWrite(iRedPin, HIGH);
  }
  else {
    digitalWrite(iGreenPin, HIGH);
    digitalWrite(iRedPin, LOW);
  }
}