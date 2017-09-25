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
/* Battery notifications

This example measures charge of 11.1v 1Ah LiPo battery from Bioloid Premium Kit.

*/

/*You need to do your own voltage divider Vout = (R2/(R2+R1))*Vin */
const int An_CMpin = 0; //Pin A0 in CM-900

void setup(){
  pinMode(BOARD_LED_PIN,OUTPUT);
  pinMode(An_CMpin, INPUT_ANALOG); // Set Analog input for pin A0
}

void loop(){
  float val = analogRead(An_CMpin)/10; // we read values from our battery

  //SerialUSB.println(val);
  /* we do a little conversion, 227 is the result of (1023/(R2(R2+R1)))/10 */
  float BatVolts = ((val)/(227));
  float VoltsMul = BatVolts * 11.1;

  if(VoltsMul > 4 && VoltsMul < 5) {
  /*Notification to know your battery is ok.*/
    digitalWrite(BOARD_LED_PIN,0);
    delay(100);
    digitalWrite(BOARD_LED_PIN,1);
    delay(100);
  }

  delay(500); // An small delay after each analog reading.
}
