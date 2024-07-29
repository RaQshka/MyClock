#pragma once
#include "esp32-hal-gpio.h"
#include "DisplayFunctions.hpp"

#define BUZZER_PIN  18 // ESP32 pin GPIO18 connected to piezo buzzer

// Replace with your network credentials
const char* ssid     = "TP-LINK_C742";
const char* password = "alt3rosid3r_7930";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
String alarmString = "";
bool isAlarmOn = false;
long long modeTimer;
bool isRolling = false;
struct TimeBytes{
  byte hour = 0, min =0, sec = 0;
  TimeBytes(byte hour, byte min, byte sec){
    this->hour = hour;
    this->min = min;
    this->sec = sec;

  }
};

void TimeInit(){
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600*3);

    modeTimer = millis();

}

String GetDate(bool debug = false){
    while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);


  if(debug == true){
    
    Serial.println(formattedDate);
    Serial.print("DATE: ");
    Serial.println(dayStamp);

  }
  return dayStamp;
}

String GetTime(bool debug = false){
    while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();

  // Extract date
  int splitT = formattedDate.indexOf("T");
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);

  if(debug == true){
    
    Serial.println(formattedDate);
    Serial.print("HOUR: ");
    Serial.println(timeStamp);

  }
  return timeStamp;
}

void SetAlarm(String time){
  alarmString = time;
}

String FormatTimeFromBytes(byte hour, byte min, byte sec){
  if(hour <0||hour>23 ||min<0||min>59||sec<0||sec>59){
    Serial.println("Wrong time on alarm");
    return GetTime();
  }
  String h = hour/10==0?"0"+String(hour):String(hour);
  String m = min/10==0?"0"+String(min):String(min);
  String s = sec/10==0?"0"+String(sec):String(sec);
  String result = h + ":"+ m+ ":"+ s;
  return result;
}


bool CheckAnyButton(){
  return digitalRead(16)|| digitalRead(17);
}
void CheckAlarm(){
  if(alarmString ==""){
//    Serial.println("alarm is not detected");
    return;
  }
  if(alarmString == GetTime()){
    isAlarmOn = true;
  }
}

void myTone( int pin = 25)
{
    tone(BUZZER_PIN, 1000);

}

void myNoTone( int pin = 25)
{
  noTone(BUZZER_PIN);
}

void Alarm(bool toner){
    if(toner)
      myTone();
    else myNoTone();
}
void AlarmUntilTouch(){
  if(!isAlarmOn) return;
  long long currentMillis;
  long long timer = millis();
  long long timerBacklight = millis();
  bool toner = false;
  bool backlightSwitcher = false;
  while(isAlarmOn){

    currentMillis = millis(); 
    if(currentMillis-timer > 200 ){
      timer = currentMillis;
      toner = !toner;
      Alarm(toner);
    }
    if(currentMillis - timerBacklight >800){
      timerBacklight = currentMillis;
      if(backlightSwitcher)
        lcd.backlight();
      else lcd.noBacklight();
      backlightSwitcher =!backlightSwitcher;
    }

    if(CheckAnyButton()){
      isAlarmOn = false;
      alarmString = "";
      lcd.backlight();
      myNoTone();
      return;
    }

  }
}

String BytesToTimeString(TimeBytes time){
  if(time.hour <0||time.hour>23 ||time.min<0||time.min>59||time.sec<0||time.sec>59){
    Serial.println("Wrong time");
  }

  return String(time.hour) + ":"+ String(time.min)+ ":"+ String(time.sec);
}

TimeBytes GetTimeInBytes(){
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  return TimeBytes(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
}
