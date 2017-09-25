/* OLLO Brightness Sensor Read 
 
 connect Brightness Sensor Module(CDS-10) to port 1.
 
 You can buy Brightness Sensor (CDS-10) in ROBOTIS-SHOP
 https://goo.gl/in8evW
 www.robotis-shop-kr.com/?act=shop.goods_view&GS=2572&GC=GD010201
 
  You can also find all information 
 http://support.robotis.com/
 
                 Compatibility
 CM900                  X
 OpenCM9.04             O
 
 created 14 Aug 2017
 by ROBOTIS CO,.LTD.
 */

#include <OLLO.h>
OLLO myOLLO;

void setup(){
  myOLLO.begin(1);
}
void loop(){
  SerialUSB.print("Brightness Sensor ADC Value = ");
  SerialUSB.println(myOLLO.read(1)); //read ADC value from OLLO port 1
  delay(60);
}



