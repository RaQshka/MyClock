#include "esp32-hal.h"
#pragma once
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "EEPROMManipulation.hpp"

LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей

bool _blinkDots = false; 
String previousTime;

String rollingString="";
bool IsNotesQueue = false;

void LedInit(){
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
}

void WriteLine(String str, byte line = 0, byte offset = 0, bool clear = true){
  if(clear) lcd.clear();
  lcd.setCursor(offset, line%2);
  lcd.print(str);
}


void WriteDateOnCenter(String time, String date){
  if(previousTime == time){
    return;
  }
  
  String newTime = time;

  if(_blinkDots)
    newTime.replace(':', ' ');

  _blinkDots = !_blinkDots;

  WriteLine(newTime, 0, 4);
  WriteLine(date, 1, 3, false);
  previousTime = time;
}
void WriteTimeOnCenter(String time, bool optimize = true){
  if(previousTime == time && optimize){
    return;
  }
  
  String newTime = time;

  if(_blinkDots)
    newTime.replace(':', ' ');

  _blinkDots = !_blinkDots;

  WriteLine(newTime, 0, 4);
  previousTime = time;
}


String SpaceString = "               ";    
long long scrollTimer;
int ScrollSpeed = 300;
int _cycleToShow = 0;
void ShowString(String time)
{

  if(_cycleToShow == 0){

    lcd.setCursor(0, 1);                                               // Start cursor on position 0 at row 0
  }

  int StrLen = (SpaceString + rollingString).length();               // Concat 16 spaces, to the actual string.
                                                                    
  if(millis()-scrollTimer >ScrollSpeed){
    scrollTimer = millis();
    WriteTimeOnCenter(time, false);
    lcd.setCursor(0, 1);

    lcd.print((SpaceString + rollingString).substring(_cycleToShow, _cycleToShow + 16));   
    _cycleToShow++;
  }

  if(_cycleToShow == StrLen-1){
    _cycleToShow = 0;
    if(IsNotesQueue){
      rollingString = MainNotes.Next();
    }
  }
}