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
/* Calibration

 Demonstrates one techinque for calibrating sensor input.  The sensor
 readings during the first five seconds of the sketch execution
 define the minimum and maximum of expected values attached to the
 sensor pin.

 The sensor minumum and maximum initial values may seem backwards.
 Initially, you set the minimum high and listen for anything lower,
 saving it as the new minumum.  Likewise, you set the maximum low and
 listen for anything higher as the new maximum.

 The circuit:
 Analog sensor (potentiometer will do) attached to analog input 1

 */

// Constant (won't change):
const int sensorPin = 2;    // pin that the sensor is attached to

// Variables:
int sensorMin = 0;     // minimum sensor value
int sensorMax = 1023;  // maximum sensor value
int sensorValue = 0;   // the sensor value

void setup() {
  // Declare the sensorPin as INPUT_ANALOG:
  pinMode(sensorPin, INPUT_ANALOG);

  // Turn on the built-in LED to signal the start of the calibration
  // period:
  digitalWrite(BOARD_LED_PIN, HIGH);

  // Calibrate during the first five seconds:
  while (millis() < 5000) {
    sensorValue = analogRead(sensorPin);

    // Record the maximum sensor value:
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }

    // Record the minimum sensor value:
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }
  }

  // Signal the end of the calibration period:
  digitalWrite(BOARD_LED_PIN, LOW);
}

void loop() {
  // Read the sensor:
  sensorValue = analogRead(sensorPin);

  // Apply the calibration to the sensor reading:
  sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 255);

  // In case the sensor value is outside the range seen during calibration:
  sensorValue = constrain(sensorValue, 0, 255);

  // Fade the LED using the calibrated value:
  analogWrite(BOARD_LED_PIN, sensorValue);
  delay(100);
}
