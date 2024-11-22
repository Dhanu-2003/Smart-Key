

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <ESP8266HTTPClient.h>  // For ESP8266
//#include <HTTPClient.h>       // Uncomment for ESP32
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Define the OLED reset pin (-1 if not using a reset pin)
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Define the RX and TX pins for Software Serial 2
#define RX 12
#define TX 14

#define GPS_BAUD 9600

// The TinyGPS++ object
TinyGPSPlus gps;

// Create an instance of Software Serial
SoftwareSerial gpsSerial(RX, TX);

int key_status = 0;


WiFiClient wifiClient;  

float st1_hr = 0;
float st2_hr = 0;
float st1_sec = 0;
float st2_sec = 0;
float st1_min = 0;
float st2_min = 0;
float tdur = 0;
float dist = 0;
float sloc_lat = 0;
float sloc_lon = 0;
float stoploc_lat = 0;
float stoploc_lon = 0;
float avgspeed = 0;
float temp;
int count = 0;
int prev = 0;

int temp_spe;

const char* ssid = "Your_SSID";        // Replace with your Wi-Fi SSID
const char* password = "Your_Password"; // Replace with your Wi-Fi Password

const char* serverURL = "http://Flask Server IP Address:5000/postdata";  // Replace with your Flask server IP
const char* serverSpeedURL = "http://Flask Server IP Address:5000/postspeedloc";  // Replace with your Flask server IP
int temp_min;
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int a;
} struct_message;
int gps_check = 1;
int gps_count_check = 0;
// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv() {
  Serial.println("Access Granted");
  unsigned long start = millis();
  while (millis() - start < 1000) {
    
    while (gpsSerial.available() > 0) {
      
      gps.encode(gpsSerial.read());
    }
    if (gps.location.isUpdated()) {
     
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(SCREEN_WIDTH/2-45,SCREEN_HEIGHT/2-10);
      temp_spe = (int)(gps.speed.kmph());
      if(temp_spe<3){
        temp_spe = 0;
      }
      display.println(String(temp_spe)+" kmph");
      display.setCursor(SCREEN_WIDTH/2 - 45 ,SCREEN_HEIGHT/2+10);

      HTTPClient http;
      http.begin(wifiClient,serverSpeedURL);  // Specify the server URL
      http.addHeader("Content-Type", "application/json");  // Set content type to JSON
      String jsonData = "{\"lat\":\""+String(gps.location.lat())+"\",\"lon\":\""+String(gps.location.lng())+"\",\"speed\":\""+String(temp_spe)+"\"}";
      Serial.println(jsonData);
      count = -1;
      prev = -1;
      // Send the POST request with the JSON data
      int httpCode = http.POST(jsonData);

      // Check the status of the request
      if (httpCode > 0) {
        String payload = http.getString();  // Get the response payload
        Serial.println(httpCode);           // Print the response code
        Serial.println(payload);            // Print the response from the Flask server
      } else {
        Serial.println("Error in sending POST request");
      }

      http.end();  // Free resources
      // Display static text
      // Show initial display buffer contents on the screen --
      // the library initializes this with an Adafruit splash screen.
      display.display();

      display.setTextSize(1);
      display.setCursor(0,5);
      display.println(String(gps.date.year()) + "/" + String(gps.date.month()) + "/" + String(gps.date.day()));
      display.setCursor(0,SCREEN_HEIGHT-10);
      temp_min = (gps.time.minute()+30);
      if(temp_min>=60){
        display.println(String(((gps.time.hour()+5)%24)+1) + ":" + String((gps.time.minute()+30)%60)+":"+String(gps.time.second()));
      }
      else{
        display.println(String((gps.time.hour()+5)%24) + ":" + String((gps.time.minute()+30)%60)+":"+String(gps.time.second()));
      }
      
      // Display static text
      // Show initial display buffer contents on the screen --
      // the library initializes this with an Adafruit splash screen.
      display.display();

      if (sloc_lat==0 && sloc_lon==0){
        Serial.print("LAT: ");
        Serial.println(gps.location.lat(), 6);
        sloc_lat = gps.location.lat();
        sloc_lon = gps.location.lng();
        Serial.print("LONG: "); 
        Serial.println(gps.location.lng(), 6);
        Serial.print("SPEED (km/h) = "); 
        temp_spe = (int)(gps.speed.kmph());
        if(temp_spe<3){
          temp_spe = 0;
        }
        Serial.println(temp_spe); 
        Serial.print("ALT (min)= "); 
        Serial.println(gps.altitude.meters());
        Serial.print("HDOP = "); 
        Serial.println(gps.hdop.value() / 100.0); 
        Serial.print("Satellites = "); 
        Serial.println(gps.satellites.value()); 
        Serial.print("Time in UTC: ");
        Serial.println(String(gps.date.year()) + "/" + String(gps.date.month()) + "/" + String(gps.date.day()));
        st1_hr = (gps.time.hour()+5)%24;
        temp_min = (gps.time.minute()+30);
        if(temp_min>=60){
          st1_hr+=1;
        }
        st1_min = (temp_min)%60;
        st1_sec = gps.time.second();
        st2_hr = st1_hr;
        st2_min = st1_min;
        st2_sec = st1_sec;
      }
      else{
        temp_min = (gps.time.minute()+30);
        if(temp_min>=60){
          temp = ((((gps.time.hour()+5)%24)+1)*60 + (temp_min)%60) - (st2_hr*60 + st2_min);
        }
        else{
          temp = ((((gps.time.hour()+5)%24))*60 + (temp_min)%60) - (st2_hr*60 + st2_min);
        }
        
        if(temp>3){
          HTTPClient http;
  
          http.begin(wifiClient,serverURL);  // Specify the server URL
          
          http.addHeader("Content-Type", "application/json");  // Set content type to JSON
          int temp1 = st2_hr*60*60 + st2_min*60 + st2_sec;
          int temp2 = st1_hr*60*60 + st1_min*60 + st1_sec;
          avgspeed = dist/(temp1-temp2);
          tdur = temp1-temp2;
          String st1 = String(st1_hr)+":"+String(st1_min)+":"+String(st1_sec);
          String st2 = String(st2_hr)+":"+String(st2_min)+":"+String(st2_sec);
          String sloc = String(sloc_lat)+":"+String(sloc_lon);
          String stoploc = String(stoploc_lat)+":"+String(stoploc_lon);

          String jsonData = "{\"startt\":\""+st1+"\",\"stopt\":\""+st2+"\",\"tdur\":\""+String(tdur)+"\",\"dist\":\""+String(dist)+"\",\"sloc\":\""+sloc+"\",\"stoploc\":\""+stoploc+"\",\"avgspeed\":\""+String(avgspeed)+"\"}";
          Serial.println(jsonData);
          count = -1;
          prev = -1;
          // Send the POST request with the JSON data
          int httpCode = http.POST(jsonData);

          // Check the status of the request
          if (httpCode > 0) {
            String payload = http.getString();  // Get the response payload
            Serial.println(httpCode);           // Print the response code
            Serial.println(payload);            // Print the response from the Flask server
          } else {
            Serial.println("Error in sending POST request");
          }

          http.end();  // Free resources
          st1_hr = (gps.time.hour()+5)% 24; 
          temp_min = (gps.time.minute()+30);
          if(temp_min>=60){
            st1_hr+=1;
          }
          st1_min = (gps.time.minute()+30)%60;
          st1_sec = gps.time.second();
          st2_hr = st1_hr;
          st2_min = st1_min;
          st2_sec = st1_sec;
        }
        else{
          st2_hr = (gps.time.hour()+5)%24;
          temp_min = (gps.time.minute()+30);
          if(temp_min>=60){
            st2_hr+=1;
          }
          st2_min = (gps.time.minute()+30)%60;
          st2_sec = gps.time.second();
          temp_spe = (int)(gps.speed.kmph());
          if(temp_spe<3){
            temp_spe = 0;
          }
          dist += (temp_spe)*0.27777;
          stoploc_lat = gps.location.lat();
          stoploc_lon = gps.location.lng();
        }
        
      }
    }
    
  }
  
  Serial.println(count);
  count++;
  
  
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }
  


  testfillroundrect();
  delay(1000);
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  // Display static text
  display.println("Initializing");
  display.setCursor(0,30);
  display.println("requirement...");
  display.display();
  delay(1500);
  display.clearDisplay();

  display.setCursor(0,10);
  // Display static text
  display.println("connecting to");
  display.setCursor(0,30);
  display.println("wifi...");
  display.display();
  delay(1500);
  display.clearDisplay();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.println("Connecting to WiFi...");

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait until the ESP8266 is connected to the WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }
  display.setCursor(0,10);
  // Display static text
  display.println("WiFi");
  display.setCursor(0,30);
  display.println("Connected");
  display.display();
  delay(1500);
  display.clearDisplay();
  // Once connected, print the IP address
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // Print the IP address

  display.setCursor(0,10);
  // Display static text
  display.println("Initializing");
  display.setCursor(0,30);
  display.println("GPS");
  display.display();
  delay(1500);
  display.clearDisplay();

  gpsSerial.begin(GPS_BAUD);
  Serial.println("Software Serial started at 9600 baud rate");
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info

  pinMode(2,OUTPUT);
  pinMode(13,INPUT);
  digitalWrite(2,LOW);
  delay(500);
  digitalWrite(2,LOW);
  // Initialize the OLED display
  
  // Clear the display buffer

  display.setCursor(0,10);
  // Display static text
  display.println("Almost Done");
  display.display();
  delay(1000);
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setCursor(0,10);
  // Display static text
  display.println("Welcome");
  display.display();
  delay(1500);
  display.clearDisplay();

  
  display.setCursor(SCREEN_WIDTH/2 -32,SCREEN_HEIGHT/2-20);
  display.println("NO KEY");
  display.setCursor(SCREEN_WIDTH/2 - 45 ,SCREEN_HEIGHT/2+10);
  // Display static text
  display.println("DETECTED");
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();


  
}

void loop() {
  int temp = digitalRead(13);
  if(temp){
    if(key_status==0){
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(SCREEN_WIDTH/2 -35 ,SCREEN_HEIGHT/2-20);
      display.println("KEY");
      display.setCursor(SCREEN_WIDTH/2 - 45 ,SCREEN_HEIGHT/2+10);
      // Display static text
      display.println("DETECTED");
      // Show initial display buffer contents on the screen --
      // the library initializes this with an Adafruit splash screen.
      display.display();
      delay(1500);
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0 ,SCREEN_HEIGHT/2-30);
      display.println("Let's Ride");
      // Show initial display buffer contents on the screen --
      // the library initializes this with an Adafruit splash screen.
      display.display();
      delay(1000);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0,10);
      display.println("Connecting to");
      display.setCursor(0,20);
      // Display static text
      display.println("satillite");
      // Show initial display buffer contents on the screen --
      // the library initializes this with an Adafruit splash screen.
      display.display();
      key_status = 1;
    }
    
    OnDataRecv();
    digitalWrite(2,LOW);

  }
  else{
    if(key_status==1){
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(SCREEN_WIDTH/2 -32,SCREEN_HEIGHT/2-20);
      display.println("NO KEY");
      display.setCursor(SCREEN_WIDTH/2 - 45 ,SCREEN_HEIGHT/2+10);
      // Display static text
      display.println("DETECTED");
      // Show initial display buffer contents on the screen --
      // the library initializes this with an Adafruit splash screen.
      display.display();
      key_status = 0;
    }
    
    digitalWrite(2,HIGH);
  }
}

void testfillroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}