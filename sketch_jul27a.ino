

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "GlobalTime.hpp"
#include "DisplayFunctions.hpp"
#include "WebInterruption.hpp"
#include "EEPROMManipulation.hpp"


void setup() {
    Serial.begin(115200);
  WifiInit();
    handleWebPages();
  TimeInit();
  LedInit();
  pinMode(25, OUTPUT);   // Buzzer alaram as output
}
void loop() {
  if(millis() - modeTimer >32*ScrollSpeed){
    modeTimer = millis();
    isRolling = !isRolling;
  }

  if(isRolling&&rollingString!=""){
    ShowString(GetTime());
  }else{
    WriteDateOnCenter(GetTime(), GetDate());
  }
      CheckAlarm();
      AlarmUntilTouch();  

  delay(30);
}
