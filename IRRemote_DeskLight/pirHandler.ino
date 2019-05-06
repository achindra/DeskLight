//#include "deskLight.h"

#define PAUSE_MS   5000
#define CALIB_TIME 15
#define PIRPIN     5

bool needLights = true;
bool takeLowTime = false;;
long unsigned int lowIn;

void _motion_on()
{
  irsend.sendNEC(CMD_ON, 32);
  delay(100);
  irsend.sendNEC(CMD_STROBE, 32);
}

void _motion_off()
{
  irsend.sendNEC(0xF7E01F, 32);  //white
  delay(100);
  for (int i = 0; i < 100; i++){
    irsend.sendNEC(0xF7807F, 32); //dim
    delay(100);
  }
  irsend.sendNEC(CMD_OFF, 32);
}

void calibratePIR()
{
  pinMode(PIRPIN, INPUT);
  for (int i = 0; i < CALIB_TIME; i++) {
    delay(1000);
  }  
  //btDeskLight.write("Device caliberated, ready!");
}

void checkMotionAndStrobe()
{
  if (digitalRead(PIRPIN) == HIGH) {
    if (needLights) {
      needLights = false;
      _motion_on();
      delay(50);
    }
    takeLowTime = true;
  }

  if (digitalRead(PIRPIN) == LOW) {
    if (takeLowTime) {
      takeLowTime = false;
      lowIn = millis();
    }
    if (!needLights && (millis() - lowIn) > PAUSE_MS) {
      needLights = true;
      _motion_off();
      delay(50);
    }
  }
}

