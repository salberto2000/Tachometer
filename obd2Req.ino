
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16, 2);

int rpm = 0;

void setup() {

  lcd.begin();
  lcd.backlight();
  
  Serial.begin(115200);
  Serial.println("CAN Read - Testing receival of CAN Bus message");
  delay(1000);

  if(Canbus.init(CANSPEED_500))
    Serial.println("CAN Init ok");
  else
    Serial.println("Can´t init CAN");

  delay(1000);

}

//************************Main Loop************************//

void loop()
{
  obd2Request();
  delay(5);
  obd2Replay();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("RPM:");
  lcd.print(rpm);
  delay(5);
}

void obd2Request() 
{
  tCAN message;
    message.id = 0x7DF;
    message.header.rtr = 0;
    message.header.length = 8;
    message.data[0] = 0x02;
    message.data[1] = 0x01;
    message.data[2] = 0x0C;
    message.data[3] = 0x55;
    message.data[4] = 0x55;
    message.data[5] = 0x55;
    message.data[6] = 0x55;
    message.data[7] = 0x55;

  mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  mcp2515_send_message(&message);
}

void obd2Replay()
{
  tCAN message;
if (mcp2515_check_message())
   {
    if (mcp2515_get_message(&message))
   {
        if (message.id == 0x7E8)
            {
              Serial.print("ID: ");
              Serial.print(message.id,HEX);
              Serial.print(" ");
              Serial.print("Data: ");
              Serial.print(message.header.length,DEC);
              Serial.print(" ");
              for(int i=0;i<message.header.length;i++)
                {
                  Serial.print(message.data[i],HEX);
                  Serial.print(" ");
                }
              rpm = ((message.data[3] * 256)+ message.data[4])/4;
              Serial.print(rpm);
              Serial.print("");
            }
    }
   }
}
