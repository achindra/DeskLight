/*
 * Turn desk lights on if there is motion in room
 */

#include <SoftSerial.h>
#include <IRremote.h>
#include <IRremoteInt.h>

SoftSerial btDeskLight(2, 3); //Rx/Tx
char cmd[16] = {0};

unsigned const int CMD_ON        = 0xF7C03F;
unsigned const int CMD_OFF       = 0xF740BF;
unsigned const int CMD_STROBE    = 0xF7F00F;
unsigned const int CMD_NIGHT_ON  = 0xFF00FF;
unsigned const int CMD_NIGHT_OFF = 0xFF0000;

unsigned int cmd_l = CMD_OFF;

IRsend irsend;

// accept hex in ascii without '0x' and converts to int
unsigned int atohex(char *s)
{
  unsigned int val = 0;
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

void setup() {
  btDeskLight.begin(9600);
  btDeskLight.write("AT+DEFAULT\r\n");
  btDeskLight.write("AT+RESET\r\n");
  delay(1000);

  btDeskLight.write("AT+NAME=DeskLight\r\n");
  btDeskLight.write("AT+Role0\r\n");  //Slave
  btDeskLight.write("AT+TYPE1\r\n");  //Simple Pairing

  //calibratePIR();
}

void loop() {
  static unsigned int iter = 0;
  static bool isNightMode =  false;
  static bool isSetupOn = true;

  if (btDeskLight.available())
  {
    cmd[iter] = btDeskLight.read();

    //if CRLF (Case of CC41A)
    if ((cmd[iter] == 10) && (cmd[iter - 1] == 13)) {
      cmd[iter - 1] = '\0'; iter = 0;
      cmd_l = atohex(cmd);

      //check night mode switching
      if ( CMD_NIGHT_ON == cmd_l ) {
        isNightMode = true;
        btDeskLight.write("Night mode ON ");
      }
      else if ( CMD_NIGHT_OFF == cmd_l ) {
        isNightMode = false;
        _motion_off();
        btDeskLight.write("Night mode OFF ");
      } 
      else {
        //check power commands
        if ( cmd_l == CMD_ON ) {
          isSetupOn = true;
          btDeskLight.write("Power ON");
        } 
        else if ( cmd_l == CMD_OFF ) {
          isSetupOn = false;
          btDeskLight.write("Power OFF");
        }

        //ON-CMD-OFF wrapper if lights are off
        if (!isSetupOn) {
          irsend.sendNEC(CMD_ON, 32);
          delay(100);
        }
        irsend.sendNEC(cmd_l, 32);
        if (!isSetupOn) {
          irsend.sendNEC(CMD_OFF, 32);
          delay(100);
        }
      }
      //echo
      btDeskLight.write(cmd); btDeskLight.write("\n");

    } else {
      iter++; iter %= 16;
    }
  }

/*
  //Check for motion if nightmode
  if (isNightMode) {
    checkMotionAndStrobe();
  }
*/
}
