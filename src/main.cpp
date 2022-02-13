#include <Arduino.h>
#include <M5StickC.h>
#include <Tone32.h>
extern const unsigned char m5stack_startup_music[];

#define BUZZER_CHANNEL 0
#define BUZZER_PIN 0

#define DUREE_TEMPS 300
#define OCTAVE_4_DO 523
#define OCTAVE_4_RE 587
#define OCTAVE_4_MI 659
#define OCTAVE_4_FA 349
#define OCTAVE_4_SOL 392
#define OCTAVE_4_LA 440
#define OCTAVE_4_SI 494

const int servo_pin = 26;

int freq = 50;
int ledChannel = 0;
int resolution = 10;
int listNotes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int k = 0;

void partitionEmptyWithKey()
{
  M5.Lcd.fillRect(20 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  M5.Lcd.fillRect(26 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  M5.Lcd.fillRect(32 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  M5.Lcd.fillRect(38 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  M5.Lcd.fillRect(44 /*y (haut en bas*/, 10, 2 /*longueur*/, 140 /*largeur */, BLACK);
  //cleSol();
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

void setup()
{

  M5.begin();

  M5.Lcd.fillScreen(TFT_WHITE);
  M5.IMU.Init();

  M5.Lcd.setTextColor(TFT_BLACK); 
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

void loop()
{
  buttonAction();  
}
