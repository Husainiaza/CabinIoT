

#include <WiFi.h>
#include <MQTT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SCD30.h>




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
#define namaSSID    "IoT";
#define SSIDpwd     "iot@kksj2023";
//#define namaSSID    "Haza@unifi";
//#define SSIDpwd     "0135335045";
// ~~~~~~~~~~~~~~~~~~~  TAMMAT   ~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~ Penetapan Saiz Skrin OLED ~~~~~~~~~~~~~~~~~
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


// ~~~~~~~~~~~~~~~~~~~  TAMMAT   ~~~~~~~~~~~~~~~~~~~~~~~

//-----c.  - ISTIHAR  constant dan pembolehubah------------------------------
//---Penetapan nama Pembolehubah yg diumpukkan kepada satu nilai awal  --
const char ssid[] = namaSSID;
const char pass[] = SSIDpwd;

//------ ISTIHAR Pemboleh ubah dengan jenis Data yang digunakan---
unsigned long lastMillis = 0;

//-----d. - Cipta Objek dari Librari berkaitan------------------ ----
Adafruit_SSD1306 paparOled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_SCD30 scd30;
//--------------------------------------------------------------------------

WiFiClient net;
MQTTClient client;

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

void setup(void) {

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
 
  Serial.println("Permulaan Sistem IoT Kabin - Ujian Nod SCD30");

  if (!scd30.begin()) {
    Serial.println("Ralat: SCD30 tidak ditemui. Sila periksa pendawaian I2C.");
    while (1) { delay(10); } 
  }
  Serial.println("Sensor SCD30 berjaya disambungkan.");
  
  // Sela masa bacaan ditetapkan setiap 2 saat
  scd30.setMeasurementInterval(2); 

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

void loop() {

  client.loop();
 delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

if (millis() - lastMillis > 2000) {
  lastMillis = millis();

  //------SENSOR 1 -------------------------------------------------------------
    

  if (scd30.dataReady()) {
    if (!scd30.read()){
      Serial.println("Ralat ketika memproses pembacaan data.");
      return;
    }

    // Paparan data pada Serial Monitor
    Serial.print("Kepekatan CO2 : ");
    Serial.print(scd30.CO2, 3);
    Serial.println(" ppm");
    client.publish("cabin/ppm", String(scd30.CO2,0)); 

    Serial.print("Suhu Kabin    : ");
    Serial.print(scd30.temperature);
    Serial.println(" °C");
    client.publish("cabin/suhu", String(scd30.temperature,0)); 

    Serial.print("Kelembapan    : ");
    Serial.print(scd30.relative_humidity);
    Serial.println(" %");
    client.publish("cabin/humiditi", String(scd30.relative_humidity,0)); 

    Serial.println("-----------------------------------");
    // Nota: Integrasi pubsubclient.h akan diletakkan di sini untuk penghantaran data ke Broker Raspberry Pi via Wi-Fi
  }
  delay(100);
}
}
