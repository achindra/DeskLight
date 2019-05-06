#include <SoftwareSerial.h>

SoftwareSerial BTSerial(5, 6);

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  BTSerial.write("AT+DEFAULT\r\n");
  delay(100);
  BTSerial.write("AT+RESET\r\n");
  delay(100);
  BTSerial.write("AT+NAMEController\r\n");
  delay(100);
  //BTSerial.write("AT+TYPE1"); //Simple pairing
  //delay(100);
  BTSerial.write("AT+ROLE1\r\n");
  delay(1000);
  Serial.println("Client Ready!");
}
void loop()
{
  if (BTSerial.available())
    Serial.write(BTSerial.read());
  if (Serial.available()){
    BTSerial.write(Serial.read());
  }
}
