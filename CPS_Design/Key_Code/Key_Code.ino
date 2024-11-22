#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x3B, 0x09, 0xC2};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int a = 234987;
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
    digitalWrite(12,HIGH);
  }
  else{
    Serial.println("Delivery fail");
    digitalWrite(12,HIGH);
    delay(1000);
    digitalWrite(12,LOW);
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(12,OUTPUT);

  for(int i=0;i<20;i++){
    digitalWrite(12,HIGH);
    delay(50);
    digitalWrite(12,LOW);
    delay(50);
  }
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
   
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    lastTime = millis();
  }
}