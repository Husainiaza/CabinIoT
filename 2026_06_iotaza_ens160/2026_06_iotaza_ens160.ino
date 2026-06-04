/*------------------------------------------------------------------
  ===================  Seksyen 1 - HEADER        ===================
  ------------------------------------------------------------------*/

//-------------------------------------------------------------------
//=  A. - Library  include and define  yang diperlukan              =
//-------------------------------------------------------------------

#include <WiFi.h>
#include <MQTT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DFRobot_ENS160.h>



//--------------------------------------------------------------------------
//-----b. - ISTIHAR Sambungan nama Pin dengan Nombor Fizikal PIN ----
//--------------------------------------------------------------------------

#define relay01     26  // digital OUTPUT - RELAY 1
#define relay02     27  // digital OUTPUT - RELAY 2
#define buzzer      25  // digital OUTPUT - BUZZER
#define SensorDS18  15  // pin ke DS18B20 (ONEWIRE) sensor Suhu
#define sensorLDR   36  // analog INPUT   - Sensor Cahaya
#define sensorTilt  4   // digital Input  - Sensor Tilt
#define sensorVolt  35  // analog INPUT   - Sensor Voltan

// ~~~~~~~~~~~~~~~~~~~~ PENTING ~~~~~~~~~~~~~~~~~~~~~~~~
// ------ Sila edit nama atau ID ikut keperluan --------
#define Client_Id   "id_anda___"
#define NamaBroker  "broker.emqx.io"
//#define namaSSID    "IoT";
//#define SSIDpwd     "iot@kksj2023";
#define namaSSID    "Haza@unifi";
#define SSIDpwd     "0135335045";
// ~~~~~~~~~~~~~~~~~~~  TAMMAT   ~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~ Penetapan Saiz Skrin OLED ~~~~~~~~~~~~~~~~~
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Istihar objek bagi Module OLED Display - SSD1306 
Adafruit_SSD1306 paparOled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ~~~~~~~~~~~~~~~~~~~  TAMMAT   ~~~~~~~~~~~~~~~~~~~~~~~

//----------------
#define I2C_COMMUNICATION  //I2C communication. Comment out this line of code if you want to use SPI communication.

#ifdef  I2C_COMMUNICATION
  /**
   *   For Fermion version, the default I2C address is 0x53, connect SDO pin to GND and I2C address will be 0x52
   */
  DFRobot_ENS160_I2C ENS160(&Wire, /*I2CAddr*/ 0x53);
#else
  /**
   * Set up digital pin according to the on-board pin connected with SPI chip-select pin
   * csPin Available Pins. For example: ESP32&ESP8266(D3), m0(6)
   */
  uint8_t csPin = D3;
  DFRobot_ENS160_SPI ENS160(&SPI, csPin);
#endif
//-------------------------

//-----c.  - ISTIHAR  constant dan pembolehubah------------------------------
//---Penetapan nama Pembolehubah yg diumpukkan kepada satu nilai awal  --
const char ssid[] = namaSSID;
const char pass[] = SSIDpwd;

//------ ISTIHAR Pemboleh ubah dengan jenis Data yang digunakan---
unsigned long lastMillis = 0;

//-----d. - Cipta Objek dari Librari berkaitan------------------ ----
//--------------------------------------------------------------------------

WiFiClient net;
MQTTClient client;


//##################  Seksyen 1 - TAMAT #############################
//--------------FUNCTION----------------------------

void connect() {
  Serial.print("sambungan ke wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.print("sambungan WiFi berjaya ..... ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // Print the IP address

  Serial.print("sambungan ke broker MQTT : ");
  Serial.println(NamaBroker);

  Serial.print("\nconnecting...");
  while (!client.connect(Client_Id)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe("topicAnda/buzzer");
  client.subscribe("topicAnda/relay1");
  client.subscribe("topicAnda/relay2");
  // -- tambah subcribe disini ---
 
}

void messageReceived(String &topic, String &payload) {
  //Serial.println("incoming: " + topic + " - " + payload);
   Serial.println(payload);
  
  //   ----Tulis Kod Kawalan ( subsribe here ) ---------------------
  //==========================Buzzer Control ========================
     if(String(topic) == "topicAnda/buzzer") 
  {
      if(payload =="on")
      {
      Serial.println(payload);
      digitalWrite(buzzer,HIGH);
      Serial.println("Buzzer ON");
      }
      
      else if(payload =="off")
      {
        Serial.println(payload);
        digitalWrite(buzzer,LOW);
        Serial.println("Buzzer OFF");
        
      }
  } 
  //==========================Relay 1 Control ========================
 
  if(String(topic) == "topicAnda/relay1") 
  {
      if(payload =="on")
      {
      Serial.println(payload);
      digitalWrite(relay01,HIGH);
      Serial.println("socket1 ON");
      }
      
      else if(payload =="off")
      {
        Serial.println(payload);
        digitalWrite(relay01,LOW);
        Serial.println("socket1 OFF");
      }
  }

  //==========================Relay 2 Control ========================

   if(String(topic) == "topicAnda/relay2") 
  {
      if(payload =="on")
      {
      Serial.println(payload);
      digitalWrite(relay02,HIGH);
      Serial.println("socket2 ON");
  
      }
      
      else if(payload =="off")
      {
        Serial.println(payload);
        digitalWrite(relay02,LOW);
        Serial.println("socket2 OFF");
     
      }
  }

  //--------------------------- END --------------------
}

//###################################################
//==================  Seksyen 2 - Fungsi Setup ======================
//-------------------------------------------------------------------

void setup() {
  pinMode(relay01,OUTPUT);
  pinMode(relay02,OUTPUT);
  pinMode(buzzer,OUTPUT); 
  pinMode(sensorTilt, INPUT);
 
  Serial.begin(115200); // initialize serial
  WiFi.begin(ssid, pass);

  client.begin(NamaBroker, net);
  client.onMessage(messageReceived);
  connect();

  if(!paparOled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
 
 //----------
 // Init the sensor
  while( NO_ERR != ENS160.begin() ){
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }
  Serial.println("Begin ok!");
  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  ENS160.setTempAndHum(/*temperature=*/25.0, /*humidity=*/50.0);
  //-----------------------------------

 
  paparOled.clearDisplay();
  paparOled.setTextSize(1);
  paparOled.setTextColor(WHITE);
  paparOled.setCursor(0,0);
  paparOled.println("---  ESI-DEVKIT  ---");
  paparOled.setCursor(0,10);
  paparOled.println("K. Komuniti Sbg Jaya");
  paparOled.setCursor(0,20);
  paparOled.println("--------------------");
  paparOled.display();
  delay(3000);

}
//##################  Seksyen 2 - TAMAT #############################

//==============  Seksyen 3 - Fungsi Utama (LOOP) ===================
//-------------------------------------------------------------------
void loop() {

 client.loop();
 delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

// publish a message roughly every second.
  if (millis() - lastMillis > 2000) {
  lastMillis = millis();

  //------SENSOR 1 -------------------------------------------------------------
    
  int dataLDR = analogRead(sensorLDR);
  Serial.print("Cahaya: ");
  Serial.println(dataLDR);  

  client.publish("topicAnda/ldr", String(dataLDR));   

  //------SENSOR 2 -------------------------------------------------------------
    
  int dataTilt = digitalRead(sensorTilt);
  Serial.print("Status TILT: ");
  Serial.println(dataTilt);  

  client.publish("topicAnda/tilt", String(dataTilt));   
 //-----------end ----------------------------------

 //------SENSOR ENS160-------------------------------------------------------------
  
uint8_t Status = ENS160.getENS160Status();
  Serial.print("Sensor operating status : ");
  Serial.println(Status);

  /**
   * Get the air quality index
   * Return value: 1-Excellent, 2-Good, 3-Moderate, 4-Poor, 5-Unhealthy
   */
  uint8_t AQI = ENS160.getAQI();
  Serial.print("Air quality index : ");
  Serial.println(AQI);
  client.publish("topicAnda/AQI", String(AQI));  
  // Menampilkan hasil ke Serial Monitor
    if (AQI == 1) Serial.print("Sangat Baik");
    else if (AQI == 2) Serial.print("Baik");
    else if (AQI == 3) Serial.print("Sedang");
    else if (AQI == 4) Serial.print("Buruk");
    else Serial.print("Sangat Bahaya");
    Serial.println(")");

  /**
   * Get TVOC concentration
   * Return value range: 0–65000, unit: ppb
   */
  uint16_t TVOC = ENS160.getTVOC();
  Serial.print("Concentration of total volatile organic compounds : ");
  Serial.print(TVOC);
  Serial.println(" ppb");
  client.publish("topicAnda/TVOC", String(TVOC));  

  /**
   * Get CO2 equivalent concentration calculated according to the detected data of VOCs and hydrogen (eCO2 – Equivalent CO2)
   * Return value range: 400–65000, unit: ppm
   * Five levels: Excellent(400 - 600), Good(600 - 800), Moderate(800 - 1000), 
   *               Poor(1000 - 1500), Unhealthy(> 1500)
   */
  uint16_t ECO2 = ENS160.getECO2();
  Serial.print("Carbon dioxide equivalent concentration : ");
  Serial.print(ECO2);
  Serial.println(" ppm");
  Serial.println();
  client.publish("topicAnda/ECO2", String(ECO2));  
 
 //-----------end ----------------------------------
  
  paparOled.setTextColor(WHITE,BLACK);
  paparOled.setCursor(0,30);
  paparOled.print("LDR :");
  paparOled.print(dataLDR,1);
  paparOled.setCursor(0,40);
  paparOled.print("TILT :");
  paparOled.print(dataTilt);
  paparOled.display();
  
  }
}

//##################  Seksyen 3 - TAMAT #############################