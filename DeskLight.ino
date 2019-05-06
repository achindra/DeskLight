
/*
 * Study Room Controller
 * 1. BlueTooth Command and Control
 * 2. IT Desk Light
 * 3. PIR Night Assist
 * 4. Temp/Humidity Status
 * 5. Speaker
 * 6. Mic - always listening
 * 
 */

#include <SoftwareSerial.h>
#include <IRremote.h>

#define BUF_MAX     16
#define LIGHTPIN    A3
#define CALIB_TIME  15
#define PIRPIN      5
#define PAUSE_MS    5000
#define IRPIN       14


SoftwareSerial btDeskLight(5, 6); //Rx|Tx
IRsend irTx;

unsigned long dwCmd = 0;
bool isNightModeOn = false;
bool isLightOff = true;
char cmd[BUF_MAX] = { 0 };

enum cmd
{
  CMD_ON        = 0xF7C03F,
  CMD_OFF       = 0xF740BF,
  CMD_DIM       = 0xF7807F,
  CMD_BRIGHT    = 0xF700FF,
  CMD_WHITE     = 0xF7E01F,
  CMD_STROBE    = 0xF7F00F,
  CMD_NIGHT_ON  = 0xFF0011,
  CMD_NIGHT_OFF = 0xFF0000,
};

// accept hex in ascii without '0x' and converts to int
unsigned long _atohex(char *s)
{
  unsigned long val = 0;
  while ((NULL != s) && ('\0' != *s)) {
    if (*s >= '0' && *s <= '9') {
      val *= 16; val += *s - '0';
    } else if (*s >= 'a' && *s <= 'f') {
      val *= 16; val += (*s - 'a') + 10;
    } else if (*s >= 'A' && *s <= 'F') {
      val *= 16; val += (*s - 'A') + 10;
    } else {
      val = 0; break;
    }
    s++;
  }
  return val;
}

void _motion_on()
{
  //enough light?
  if(800 < analogRead(LIGHTPIN))
    return;
    
  irTx.sendNEC(CMD_ON, 32);
  delay(100);
  for (int i = 0; i < 100; i++){
    irTx.sendNEC(CMD_BRIGHT, 32);
    delay(10);
  }
  delay(100);
  btDeskLight.write("Motion On\n");
}

void _motion_off()
{
  irTx.sendNEC(CMD_WHITE, 32);
  delay(100);
  for (int i = 0; i < 100; i++){
    irTx.sendNEC(CMD_DIM, 32);
    delay(10);
  }
  delay(100);
  irTx.sendNEC(CMD_OFF, 32);
  btDeskLight.write("Motion Off\n");
}

void _checkMotionAndStrobe()
{
  static bool needLights = true;
  static bool takeLowTime = false;;
  static unsigned long lowIn;
  
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


void setup() {
  pinMode(4, INPUT);
  pinMode(IRPIN, INPUT);
  btDeskLight.begin(9600);
  btDeskLight.write("AT+NAME=DeskLight\r\n");
  btDeskLight.write("AT+ROLE0\r\n");
  btDeskLight.write("AT+TYPE1\r\n");
  btDeskLight.write("AT+RESET\r\n");
  delay(1000);
  btDeskLight.flush();
  pinMode(PIRPIN, INPUT);
  for (int i = 0; i < CALIB_TIME; i++) {
    delay(1000);
  }
  btDeskLight.write("Ready!"); btDeskLight.write("\n");
}

void loop() {
  static unsigned char iter = 0;

  if (btDeskLight.available())
  {
    cmd[iter] = btDeskLight.read();

    if ((cmd[iter] == 10) && (cmd[iter - 1] == 13))
    {
      cmd[iter - 1] = '\0'; iter = 0;      
      dwCmd = _atohex(cmd);
      switch (dwCmd)
      {
        case CMD_NIGHT_ON:
          isNightModeOn = true;
          break;
        case CMD_NIGHT_OFF:
          isNightModeOn = false;
          break;
        case CMD_ON:
          isLightOff = false;
          irTx.sendNEC(dwCmd, 32);
          break;
        case CMD_OFF:
          isLightOff = true;
          irTx.sendNEC(dwCmd, 32);
          break;
        default:  
          if (isLightOff)
            irTx.sendNEC(CMD_ON, 32);
          irTx.sendNEC(dwCmd, 32);
          if (isLightOff)
            irTx.sendNEC(CMD_OFF, 32);
          delay(100);
          break;
      }
      btDeskLight.write(cmd); btDeskLight.write("\n");
    }
    else
    {
      (++iter) %= BUF_MAX;
    }
  }

  if(isNightModeOn)
  {
    _checkMotionAndStrobe();
  }
}
