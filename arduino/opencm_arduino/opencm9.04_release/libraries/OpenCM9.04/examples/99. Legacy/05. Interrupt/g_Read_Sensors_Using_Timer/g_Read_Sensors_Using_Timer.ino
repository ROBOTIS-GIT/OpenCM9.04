#include <OLLO.h>

#define INTERVAL 100000 //micros

OLLO myOLLO;
HardwareTimer Timer(1);
long before_start_time = 0;

void setup() {

  myOLLO.begin(1);
  SerialUSB.begin();

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


  SerialUSB.print("[INTERVAL=");
  SerialUSB.print(INTERVAL);
  SerialUSB.print("] ");
  SerialUSB.print("DMS=");
  SerialUSB.print(DMS_value);
  SerialUSB.print(", start_time=");
  SerialUSB.print(start_time);
  SerialUSB.print(", interval_time=");
  SerialUSB.print(interval_time);
  SerialUSB.print(", exec_time=");
  SerialUSB.println(micros()- start_time);

}
