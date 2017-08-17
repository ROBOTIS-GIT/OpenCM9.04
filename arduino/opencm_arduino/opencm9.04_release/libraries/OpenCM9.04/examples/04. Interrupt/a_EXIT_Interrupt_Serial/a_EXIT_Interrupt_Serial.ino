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

/*
 * EXIT_Interrupt_Serial
*/

/*
  EXTI PINS : 0 ~ 25
 */


void setup(){
  Serial.begin(115200);

  pinMode(9, OUTPUT);

  pinMode(0, INPUT_PULLDOWN);
  pinMode(1, INPUT_PULLDOWN);
  pinMode(2, INPUT_PULLDOWN);
  pinMode(3, INPUT_PULLDOWN);
  pinMode(4, INPUT_PULLDOWN);

  /*It can be choose as CHANGE, RISING or FALLING*/
  attachInterrupt(0, changeDirection_EXIT_0, RISING);
  attachInterrupt(1, changeDirection_EXIT_1, RISING);
  attachInterrupt(2, changeDirection_EXIT_2, RISING);
  attachInterrupt(3, changeDirection_EXIT_3, RISING);
  attachInterrupt(4, changeDirection_EXIT_4, RISING);
}

void changeDirection_EXIT_0(void){
  Serial.println("EXIT_Interrupt! 0");
}

void changeDirection_EXIT_1(void){
  Serial.println("EXIT_Interrupt! 1");
}

void changeDirection_EXIT_2(void){
  Serial.println("EXIT_Interrupt! 2");
}

void changeDirection_EXIT_3(void){
  Serial.println("EXIT_Interrupt! 3");
}

void changeDirection_EXIT_4(void){
  Serial.println("EXIT_Interrupt! 4");
}

void loop(){
  digitalWrite(9, HIGH);
  delay(500);
  digitalWrite(9, LOW);
  delay(500);
}
