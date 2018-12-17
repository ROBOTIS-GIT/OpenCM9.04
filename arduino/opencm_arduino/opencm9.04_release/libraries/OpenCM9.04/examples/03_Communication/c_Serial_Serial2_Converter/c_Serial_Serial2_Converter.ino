/*******************************************************************************
* Copyright (c) 2016, ROBOTIS CO., LTD.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
* * Neither the name of ROBOTIS nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
/*SerialUSB_Serial2_Converter

 This example is convert from serial2 to USB.
 OpenCM9.04 has a port(J9) connected directly to Serial2.
 If some data is coming from Serial2, it is sent to serialUSB.
 On the contrary, all data coming from serialUSB is sent to Serial2.


 You can connect the below products to J9 Connector in CM-900, OpenCM9.04
 [BT-110A] or [BT-110A Set]
 http://www.robotis-shop-kr.com/goods_detail.php?goodsIdx=875
 [ZIG-110A Set]
 http://www.robotis-shop-kr.com/goods_detail.php?goodsIdx=405
 [LN-101] USART communication and download tool in CM-100
 http://www.robotis-shop-kr.com/goods_detail.php?goodsIdx=348

 You can also find all information about ROBOTIS products
 http://emanual.robotis.com/

 */
/* BAUDRATE 
  If BAUDRATE is defined, the converter will use the specified baud rate
  for Serial2

  If BAUDRATE is NOT defined, the code will ask the USB Serial object for 
  the hosts requested baud rate and use it.  In addition it will check to
  see if the host baud rate changes and will change accordingly. 

  BaudRate Changes authored by: KurtE
*/

//#define BAUDRATE 57600

#ifndef BAUDRATE
uint32_t g_usb_baudrate;
#endif

void setup(){
  while (!Serial && millis() < 5000) ;
  Serial.begin(115200);
#if defined(BAUDRATE)
  Serial2.begin(BAUDRATE);
#else
  g_usb_baudrate = Serial.getBaudRate();
  Serial2.begin(g_usb_baudrate);
#endif  
  pinMode(BOARD_LED_PIN, OUTPUT);
}

void loop(){
  if(Serial.available()){
    Serial2.print((char)Serial.read());//send data coming from USB to Serial2
  }

  if(Serial2.available()){
    toggleLED();
    Serial.print((char)Serial2.read()); //send data coming from Serial2 to USB(PC)
  }
#ifndef BAUDRATE
  if (g_usb_baudrate != Serial.getBaudRate()){
    g_usb_baudrate = Serial.getBaudRate();
    Serial2.begin(g_usb_baudrate);
  }
#endif  
}
