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

#include <OLLO.h>

#define INTERVAL 100000 //micros

OLLO myOLLO;
HardwareTimer Timer(1);
long before_start_time = 0;

void setup() {

  myOLLO.begin(1);
  Serial.begin(115200);

  Timer.pause();
  Timer.setPeriod(INTERVAL);
  Timer.attachInterrupt(getDMSValue); //割り込む関数を指定
  Timer.refresh();
  Timer.resume();
}

void loop() {
  // Nothing! It's all in the handler_led() interrupt:
}

void getDMSValue(void) {
  long start_time = 0;
  long interval_time = 0;
  int DMS_value = 0;
  int tmp =0;

  start_time = micros();
  interval_time = start_time - before_start_time;
  before_start_time = start_time;

  DMS_value = myOLLO.read(1);


  if(DMS_value >= 1000){
   for(int i=0; i< 100000; i++) {
      tmp = tmp + i - tmp * i / tmp;
   }
  }


  Serial.print("[INTERVAL=");
  Serial.print(INTERVAL);
  Serial.print("] ");
  Serial.print("DMS=");
  Serial.print(DMS_value);
  Serial.print(", start_time=");
  Serial.print(start_time);
  Serial.print(", interval_time=");
  Serial.print(interval_time);
  Serial.print(", exec_time=");
  Serial.println(micros()- start_time);

}
