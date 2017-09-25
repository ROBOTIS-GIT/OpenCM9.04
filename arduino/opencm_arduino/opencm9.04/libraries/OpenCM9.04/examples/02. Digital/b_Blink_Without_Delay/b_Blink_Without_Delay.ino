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
/*Blink without delay

 Turns on and off the built-in light emitting diode (LED), without
 using the delay() function.  This means that other code can run at
 the same time without being interrupted by the LED code.
 */
/*
millis()
 : Returns the number of milliseconds since the Arduino board began running the current program.
 This number will overflow (go back to zero), after approximately 50 days.
 */

// Variables:
long previousMillis = 0; // will store the last time the LED was updated
int interval = 1000;  // interval at which to blink (in milliseconds)

void setup() {
  // Set up the built-in LED pin as output:
  pinMode(BOARD_LED_PIN, OUTPUT);
}

void loop() {
  // Check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time we blinked
  // the LED is bigger than the interval at which we want to blink

  if ((int)millis() - previousMillis > interval) {
    // Save the last time you blinked the LED
    previousMillis = millis();

    // If the LED is off, turn it on, and vice-versa:
    toggleLED();
  }
}
