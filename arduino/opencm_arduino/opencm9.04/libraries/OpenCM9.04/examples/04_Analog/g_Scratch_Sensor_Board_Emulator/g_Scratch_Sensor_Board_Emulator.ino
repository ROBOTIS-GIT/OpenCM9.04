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
/* Scratch_Sensor_Board_Emulator

 A program that simulates a Scratch board using an Arduino. This version reads the button and  inputs A-D.
 Version 0.6

 */

#include <OLLO.h>
OLLO myOLLO;

void ScratchBoardSensorReport(int sensor, int value)
{
  Serial.print( B10000000
    | ((sensor & B1111)<<3)
    | ((value>>7) & B111));
  Serial.print( value & B1111111 );
}

void setup()
{

  pinMode(BOARD_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(BOARD_LED_PIN, OUTPUT);
  Serial.begin(115200);
  myOLLO.begin(1,TOUCH_SENSOR);
  myOLLO.begin(2, IR_SENSOR);
  myOLLO.begin(3);
  myOLLO.begin(4);

}
int irSensor = 0;
int gyroX=0;
int gyroY=0;
void loop()
{

  if(Serial.available()){
    gyroX = myOLLO.read(3);
    if(gyroX > 1000 && gyroX < 2000){
      gyroX = 1530;
    }
    gyroY = myOLLO.read(4);
    if(gyroY > 1000 && gyroY < 2000){
      gyroY = 1530;
    }
    ScratchBoardSensorReport(0, map(gyroX, 0, 3000, 0, 1023) );
    ScratchBoardSensorReport(1, map(gyroY, 0, 3000, 0, 1023) );
    ScratchBoardSensorReport(2, 0/*analogRead(3)*/);
    ScratchBoardSensorReport(3, 0/*analogRead(4)*/);
    irSensor = myOLLO.read(2, IR_SENSOR);
    if(irSensor > 250)
      irSensor = 300;
    ScratchBoardSensorReport(4, map(irSensor, 0, 300, 0, 1023));
    ScratchBoardSensorReport(5, 0);
    ScratchBoardSensorReport(6, 0);
    ScratchBoardSensorReport(7, (myOLLO.read(1, TOUCH_SENSOR) ? 1023 : 0));
    // Let Scratch catch up with us
    delay(30);
    toggleLED();
  }
}
