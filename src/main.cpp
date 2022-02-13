#include <M5StickC.h>
#include <WiFi.h>
#include "time.h"
#include "DHT12.h"
#include <Wire.h> //The DHT12 uses I2C comunication.
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include <Arduino.h>
#include <Tone32.h>

#define BUZZER_CHANNEL 0
#define BUZZER_PIN 0

#define OCTAVE_4_DO 523
#define OCTAVE_4_RE 587
#define OCTAVE_4_MI 659
#define OCTAVE_4_FA 349
#define OCTAVE_4_SOL 392
#define OCTAVE_4_LA 440
#define OCTAVE_4_SI 494

extern const unsigned char m5stack_startup_music[];

const int servo_pin = 26;

int freq = 50;
int ledChannel = 0;
int resolution = 10;
int listNotes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int k = 0;

DHT12 dht12; //Preset scale CELSIUS and ID 0x5c.
Adafruit_BMP280 bme;

//ssid de son choix à spécifier
const char* ssid = "CAMPUS-HEP-TOULOUSE";        
const char* password = "";
const char* ntpServer = "ntp.midway.ovh";

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

int battery = 0;
float b = 0;
int state = 1;

void partitionEmptyWithKey()
{
  M5.Lcd.fillRect(20 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  M5.Lcd.fillRect(26 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  M5.Lcd.fillRect(32 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  M5.Lcd.fillRect(38 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  M5.Lcd.fillRect(44 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
}

void note(int note, int position)
{
  M5.Lcd.fillRect(note /*y (haut en bas*/, position, 6 /*longueur*/, 6 /*largeur */, BLACK);
  M5.Lcd.fillRect(note - 10 /*y (haut en bas*/, position, 10 /*longueur*/, 2 /*largeur */, BLACK);
}

String showNote(int i)
{
  String action = "";
  switch ((i - 20) / 3 % 7)
  {
  case 0:
    ledcWriteTone(BUZZER_PIN, OCTAVE_4_MI);
    delay(200);
    ledcWriteTone(BUZZER_PIN, 0);
    delay(200);
    action = "MI";
    break;
  case 1:
    ledcWriteTone(BUZZER_PIN, OCTAVE_4_RE);
    delay(200);
    ledcWriteTone(BUZZER_PIN, 0);
    delay(200);    
    action = "RE";
    break;
  case 2:
    ledcWriteTone(BUZZER_PIN, OCTAVE_4_DO);
    delay(200);
    ledcWriteTone(BUZZER_PIN, 0);
    delay(200);
    action = "DO";
    break;
  case 3:
    ledcWriteTone(BUZZER_PIN, OCTAVE_4_SI);
    delay(200);
    ledcWriteTone(BUZZER_PIN, 0);
    delay(200);
    action = "SI";
    break;
  case 4:
    ledcWriteTone(BUZZER_PIN, OCTAVE_4_LA);
    delay(200);
    ledcWriteTone(BUZZER_PIN, 0);
    delay(200);
    action = "LA";
    break;
  case 5:
    ledcWriteTone(BUZZER_PIN, OCTAVE_4_SOL);
    delay(200);
    ledcWriteTone(BUZZER_PIN, 0);
    delay(200);
    action = "SOL";
    break;
  case 6:
    ledcWriteTone(BUZZER_PIN, OCTAVE_4_FA);
    delay(200);
    ledcWriteTone(BUZZER_PIN, 0);
    delay(200);
    action = "FA";
    break;
  }
  return action;
}

void actualisePartition(){
      M5.Lcd.fillScreen(TFT_WHITE);
    partitionEmptyWithKey();
    for (int j = 0; j < 8; j++)
    {
      if (listNotes[j] != 0)
      {
        note(listNotes[j], 140 - j * 15);
      }else{
        j = 8;
      }
    }
    M5.Lcd.setCursor(0, 0);
}

void setTime() {

  // connexion au Wifi
  Serial.printf("Attempting to connect to %s ", ssid);
  WiFi.begin(ssid, password);
  delay(1000);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Connexion au \r\nWi-fi en cours");
  while(WiFi.status() != WL_CONNECTED) { //Si la co Wifi n'est pas encore faite
    delay(500); //délai de 0.5s
    M5.Lcd.print(".");
  }
  while (WiFi.status() == WL_CONNECTED) { 

    Serial.println(" CONNECTED, SETTING TIME");

    configTime(1 * 3600, 3600, ntpServer); //1 est la zone Europe/Paris (utc +1) 
  
    // Get local time
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) {
  
      // Set RTC time
      RTC_TimeTypeDef TimeStruct;
      TimeStruct.Hours   = timeInfo.tm_hour;
      TimeStruct.Minutes = timeInfo.tm_min;
      TimeStruct.Seconds = timeInfo.tm_sec;
      M5.Rtc.SetTime(&TimeStruct);
      
      RTC_DateTypeDef DateStruct;
      DateStruct.WeekDay = timeInfo.tm_wday;
      DateStruct.Month = timeInfo.tm_mon + 1;
      DateStruct.Date = timeInfo.tm_mday;
      DateStruct.Year = timeInfo.tm_year + 1900;
      M5.Rtc.SetData(&DateStruct);
    }
      // Disconnect Wifi if it did connect
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("Wifi disconnected");  
  }
}

void loadDisplay() {

  static const char *wd[7] = {"Dim","Lun","Mar","Mer","Jeu","Ven","Sam"};
  
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Rtc.GetData(&RTC_DateStruct);

  M5.Lcd.fillScreen(WHITE); 
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE); 
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(2, 2);
  M5.Lcd.printf("%s   %02d-%02d-%04d", wd[RTC_DateStruct.WeekDay], RTC_DateStruct.Date, RTC_DateStruct.Month, RTC_DateStruct.Year);
  M5.Lcd.drawLine(0, 15, 159, 15, TFT_BLACK);
  M5.Lcd.setCursor(2, 25);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(TFT_RED, TFT_WHITE); 
  M5.Lcd.printf("%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes);
  M5.Lcd.setCursor(100, 32);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("%02d", RTC_TimeStruct.Seconds);
}

void batteryPercent() {
  M5.Lcd.setCursor(115, 2);
  M5.Lcd.setTextSize(1);
  b = M5.Axp.GetVbatData() * 1.1 / 1000;
  battery = ((b - 3.0) / 1.2) * 100;

  M5.Lcd.setTextColor(TFT_BLACK, TFT_GREEN);
  M5.Lcd.print(battery);
  M5.Lcd.print("%");

}
//Fonction appelée au démarrage, exécuté une fois --> initialise les variables
void setup() {

  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(3);
  
  M5.Axp.ScreenBreath(15); //permettre de mettre la niveau de luminosité élevé
  M5.Lcd.fillScreen(BLACK);
  setTime();

  
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(servo_pin, ledChannel);
  
}

void buttonAction()
{
  String action = "";
  int i = 20;
  if (digitalRead(M5_BUTTON_HOME) == LOW 
  || digitalRead(BUTTON_B_PIN) == LOW)
  {
  actualisePartition();
    if (digitalRead(M5_BUTTON_HOME) == LOW)
    {
      //action = "A";
      while (digitalRead(M5_BUTTON_HOME) == LOW)
      {
        if (digitalRead(BUTTON_B_PIN) == LOW)
        {
          //action = "A B";
          i += 3;
          while (digitalRead(BUTTON_B_PIN) == LOW)
          {
            actualisePartition();
            M5.Lcd.setRotation(3);
            M5.Lcd.println(showNote(i));
            M5.Lcd.setRotation(0);
          }
          delay(200);
        }
      }
    }
  
    k = 0;
    do
    {
      if (listNotes[k] == 0)
      {
        listNotes[k] = i;
        note(listNotes[k], 140 - k * 15);
        if(k == 8){
          for(int m = 0; m < 8; m++){
            delay(200);
          }
          ledcWriteTone(ledChannel, 0);
          delay(1000);
        }
        k = 8;
      }
      k++;
    } while (k < 8);

    delay(200);
  }
  if(listNotes[7] != 0 && listNotes[8] == 0){
    for(int b = 0; b < 4; b++){
      for(int a = 0; a < 8; a++){
        M5.Lcd.setRotation(3);
        showNote(listNotes[a]);
        M5.Lcd.setRotation(0);
      }
    }
    for(int a = 0; a < 8; a++){
      listNotes[a] = 0;
    }
  }
}

//loupe consécutivement afin que le programme réponde aux différentes actions
void loop() {

  int modeButton = digitalRead(M5_BUTTON_HOME);
  int sleepButton = digitalRead(M5_BUTTON_RST);  //prefered using a button for sleep instead of a timer
  
  switch(state) {
    case 1:
      
      if(modeButton == HIGH && sleepButton == HIGH) {
        
            loadDisplay();

            batteryPercent();

            delay(1000);
        

        } else if(modeButton == LOW && sleepButton == HIGH) {
            state = 2;
        } else if(modeButton == HIGH && sleepButton == LOW) {
          M5.Lcd.fillScreen(BLACK);
          M5.Axp.SetSleep();
        }
      break;

      case 3:
        if(modeButton == HIGH && sleepButton == HIGH) {
  
          if (!bme.begin(0x76)){  
            Serial.println("Could not find a valid BMP280 sensor, check wiring!");
            M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.fillScreen(BLACK);    //Added visual indicator to add time between the button press and state change.
          M5.Lcd.setCursor(2, 25);
          M5.Lcd.setTextSize(2);
          M5.Lcd.printf("Passage en mode musique");
            state = 4;
          }
  
          float tmp = dht12.readTemperature();
          float hum = dht12.readHumidity();
          float pressure = bme.readPressure();

          M5.Lcd.setCursor(0, 15);
          M5.Lcd.fillScreen(WHITE); 
          M5.Lcd.setTextColor(PURPLE, TFT_WHITE); 
          M5.Lcd.setTextSize(2);
          M5.Lcd.printf("Temp: %2.1fC\r\nHumi: %2.0f%%  \r\nPression:%2.0fPa\r\n", tmp, hum, pressure);
  
          delay(1000);
          
        } else if(modeButton == LOW && sleepButton == HIGH) {
          delay(1000);
          
        } else if(modeButton == LOW && sleepButton == HIGH) {
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.fillScreen(BLACK);    //Added visual indicator to a
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.fillScreen(BLACK);    //Added visual indicator to add time between the button press and state change.
          M5.Lcd.setCursor(2, 25);
          M5.Lcd.setTextSize(2);
          M5.Lcd.printf("Passage en mode musique");
          delay(4000);
          state = 4;
        } else if(modeButton == HIGH && sleepButton == LOW) {
          M5.Lcd.fillScreen(BLACK);
          M5.Axp.SetSleep();
        }
      break;

      case 2:

        // This is a serial passthrough Mode for the RS485 Hat
        if(modeButton == HIGH && sleepButton == HIGH) {

          M5.Lcd.print(".");  // a visual indicator that things are still moving along........

          if(Serial.available())  // If stuff was typed in the serial monitor
          {
              // Send any characters the Serial monitor prints to the rs485 device
              Serial2.print((char)Serial.read());
          }
          
          if(Serial2.available()) {    // If data was sent from the rs485 device show it
              char ch = Serial2.read();
              M5.Lcd.setCursor(2, 2);
              M5.Lcd.fillScreen(BLACK);
              M5.Lcd.print(ch);
          }
            delay(500);
            

            }
            else if(modeButton == LOW && sleepButton == HIGH) {
              M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
              M5.Lcd.fillScreen(BLACK);   //Added visual indicator to add time between the button press and state change.
              M5.Lcd.setCursor(2, 25);
              M5.Lcd.setTextSize(2);
              M5.Lcd.printf("Passage en\r\nmode capteur");
              Wire.begin(0,26);  //Wire.begin(sda, scl); Sets the pin configuration for mode 3 and the ENV hat
              delay(1000);
              state = 3;
            }else if(modeButton == HIGH && sleepButton == LOW) {
              M5.Lcd.fillScreen(BLACK);
              M5.Axp.SetSleep();
            }
      break;

      case 4 :
          M5.Lcd.setRotation(0);
          buttonAction();
          if(modeButton == LOW && sleepButton == HIGH) {
          M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
          M5.Lcd.fillScreen(BLACK);    //Added visual indicator to add time between the button press and state change.
          M5.Lcd.setCursor(2, 25);
          M5.Lcd.setTextSize(2);
           M5.Lcd.setRotation(3);
          M5.Lcd.printf("Retour a \r\nl'accueil");
          delay(1000);
          state = 1;
        } else if(modeButton == HIGH && sleepButton == LOW) {
          M5.Lcd.fillScreen(BLACK);
        }
          
   }
}







