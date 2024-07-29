
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "GlobalTime.hpp"
#include "DisplayFunctions.hpp"
#include "WebInterruption.hpp"
#include "EEPROMManipulation.hpp"
#include "MenuFunctions.hpp"

long long LowLightTimer;
bool isLowLight = false;

void setup() {
    Serial.begin(115200);
  WifiInit();
    handleWebPages();
  TimeInit();
  LedInit();
  ButtonStartMenuTimer = millis();
  LowLightTimer = millis();
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

  if(CheckAllButtonsSignals()){
    StartMenu();
  }
  if(CheckAnyButton()){
     LowLightTimer = millis();
  }
  if(millis() - LowLightTimer >100000 && !isLowLight && AllowSleepMode){
    LowLightTimer = millis();
    lcd.noBacklight();
    isLowLight = true;
  }
  if(CheckAnyButton()&&isLowLight){
      lcd.backlight();
      isLowLight = false;
  }


  delay(30);
}
