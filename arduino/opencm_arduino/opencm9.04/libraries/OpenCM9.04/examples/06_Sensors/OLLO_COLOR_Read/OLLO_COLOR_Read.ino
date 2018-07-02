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
/* OLLO Color sensor Read

 connect Color Sensor Module(CS-80) to port 2.

 You can buy DMS Sensor DYNAMIXEL in ROBOTIS-SHOP
 http://www.robotis-shop-en.com/?act=shop_en.goods_view&GS=1615&GC=GD0B0103

 You can also find all information
 http://emanual.robotis.com/docs/en/parts/sensor/cs-10/

                 Compatibility
  OpenCM9.04             O

 created 16 Nov 2012
 by ROBOTIS CO,.LTD.
*/

#include <OLLO.h>
OLLO myOLLO;

void setup(){
  myOLLO.begin(2,COLOR_SENSOR);//OLLO Color Module must be connected at port 2.
  Serial.begin(115200);
}
void loop(){
  Serial.print("COLOR Read = ");
  Serial.println(myOLLO.read(2, COLOR_SENSOR));
  delay(100);
}

/*  Result --->  myOLLO.read(2, COLOR_SENSOR)
1 -> White
2 -> Black
3 -> Red
4 -> Green
5 -> Blue
6 -> Yellow
*/
