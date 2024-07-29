#pragma once
#include "GlobalTime.hpp"
#include "WebInterruption.hpp"
#include "JumpingGame.hpp"
bool AllowSleepMode = true;


void delayWithAlarmHandling(int d){

  for(int i = 0; i<=d; i+=10){
    CheckAlarm();
    AlarmUntilTouch(); 
    delay(9);
  }
}
//0 - no buttons
//1 - 16pin
//2 - 17pin
//3 - both
int WaitButtonPress(){
  bool pin16 = digitalRead(16);
  bool pin17 = digitalRead(17);
  while(true){

    pin16 = digitalRead(16);
    pin17 = digitalRead(17);
    delayWithAlarmHandling(60);
    if(pin16 && !pin17){
      pin16 = digitalRead(16);
      pin17 = digitalRead(17);
      if(pin16 && !pin17) return 1;
    }    
    if(pin17 && !pin16){
      pin16 = digitalRead(16);
      pin17 = digitalRead(17);
      if(pin17 && !pin16) return 2;
    }
    if(pin16 &&pin17){
      pin16 = digitalRead(16);
      pin17 = digitalRead(17);
      if(pin16 && pin17) return 3;
    }
  }
  return 0;
}

//1 - 16pin
//2 - 17pin
int WaitButtonHold(){
  long long buttonHoldTimer = millis();
  int button = 0;
  bool pin16 = digitalRead(16);
  bool pin17 = digitalRead(17);
  int counter = 0;

  while(counter <200){
    pin16 = digitalRead(16);
    pin17 = digitalRead(17);
    if(pin16){
      counter++;
      if(button !=1)
        counter = 0;
      button = 1;
    }
    if(pin17){
      counter++;
      if(button != 2){
        counter=0;
      }
      button = 2;
    }
    delayWithAlarmHandling(10);
  }
  return button;
}

int countOfSignals = 0;
long long ButtonStartMenuTimer;
bool CheckAllButtonsSignals(){
  if(digitalRead(16) && digitalRead(17)){
    countOfSignals++;
  }
  if(millis()-ButtonStartMenuTimer>3000){
    ButtonStartMenuTimer = millis();
    Serial.println(countOfSignals);
    if(countOfSignals>30){
          countOfSignals = 0;
        return true;
    }
    countOfSignals = 0;
  }
  return false;
}

bool CheckBothButtonPress(){  
  bool pin16 = digitalRead(16);
  bool pin17 = digitalRead(17);
  if(pin16&&pin17){
    delayWithAlarmHandling(60);
    pin16 = digitalRead(16);
    pin17 = digitalRead(17);
    if(pin16&&pin17)
      return true;
  }
  return false;
}
//////////////////////////////////////////////////////////////////////game/////////////////////////////////////////////////////
int level = 1;       // переменная для отсчета уровня
int gamePause = 400; // переменная для задержки
byte p = 0;          // переменная для времени прыжка

// создаем массивы дракончика, дерева, камня и птицы
byte dracon[8] = {
 0b01110, 0b11011, 0b11111, 0b11100, 0b11111, 0b01100, 0b10010, 0b11011
};
byte derevo[8] = {
 0b00000, 0b00000, 0b00000, 0b11011, 0b11011, 0b11011, 0b01100, 0b01100
};
byte kamen[8] = {
 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01110, 0b11111
};
byte ptica[8] = {
 0b00100, 0b00101, 0b01111, 0b11111, 0b10100, 0b00100, 0b00000, 0b00000
};

void setupGame() {

 // создаем символы дракончика, дерева, камня и птицы
 lcd.createChar(0, dracon);
 lcd.createChar(1, derevo);
 lcd.createChar(2, kamen);
 lcd.createChar(3, ptica);

 // начинаем игру: выводим надпись GO!
 lcd.setCursor(7, 0);
 lcd.print("GO!");
 delayWithAlarmHandling(400);
 tone(18, 600);
 delayWithAlarmHandling(100);
 noTone(18);
 lcd.clear();
}

bool loopGame() {
  // первоначальное положение дракончика и препятствия
  byte d = 1;
  byte x = 15;
  byte y = 1;
  // выбираем препятствие, которое появится, рандомно
  byte i = random (1, 4);
  if (i == 3) y = 0;
  else y = 1;

  while (x > 0) {
    // очищаем экран и выводим номер уровня
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(level);

    // считываем данные с кнопки и учитываем количество циклов в прыжке
    // если дакончик находится в прыжке долго - возвращаем его вниз
    if (digitalRead(17) == LOW) d = 1;
    if (digitalRead(17) == HIGH) d = 0;
    if (p > 3) d = 1;

    // выводим дракончика в нужной строке
    lcd.setCursor(4, d);
    lcd.print(char(0));
    // выводим препятствие
    lcd.setCursor(x, y);
    tone(18, 50);
    lcd.print(char(i));
    noTone(18);
    if(CheckBothButtonPress()){
      return false;
    }
    // если дракончик наткнулся на препятствие выводим надпись GAME OVER!
    if (x == 4 && y == d) {
      if(CheckBothButtonPress()){
        return false;
      }
      delayWithAlarmHandling(400);
            if(CheckBothButtonPress()){
        return false;
      }
      tone(18, 50);
      delayWithAlarmHandling(100);
            if(CheckBothButtonPress()){
        return false;
      }
      noTone(18);
      delayWithAlarmHandling(100);
            if(CheckBothButtonPress()){
        return false;
      }
      tone(18, 20);
      delayWithAlarmHandling(300);
            if(CheckBothButtonPress()){
        return false;
      }
      noTone(18);
      lcd.clear();
      delayWithAlarmHandling(200);
            if(CheckBothButtonPress()){
        return false;
      }
      lcd.setCursor(3, 0);
      lcd.print("GAME OVER!");
      delayWithAlarmHandling(600);
            if(CheckBothButtonPress()){
        return false;
      }
      lcd.clear();
      delayWithAlarmHandling(400);
            if(CheckBothButtonPress()){
        return false;
      }
      lcd.setCursor(3, 0);
      lcd.print("GAME OVER!");
      delayWithAlarmHandling(600);
            if(CheckBothButtonPress()){
        return false;
      }
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("LEVEL: ");
      lcd.print(level);
      delayWithAlarmHandling(400);
            if(CheckBothButtonPress()){
        return false;
      }
      lcd.setCursor(3, 0);
      lcd.print("GAME OVER!");
      delayWithAlarmHandling(3000);
            if(CheckBothButtonPress()){
        return false;
      }
      lcd.clear();

      // начинаем игру заново, обнулив уровень игры
      lcd.setCursor(7, 0);
      lcd.print("GO!");
      delayWithAlarmHandling(400);
      tone(18, 600);
      delayWithAlarmHandling(100);
      noTone(18);
      lcd.clear();
      if(CheckBothButtonPress()){
        return false;
      }
      level = 0;
      gamePause = 400;
      p = 0;
      y = 1;
      x = 0;
      break;
    }

    // если дракончик прыгнул, издаем звук
    if (d == 0) { tone(18, 200); delayWithAlarmHandling(100); noTone(18); }
    else { delayWithAlarmHandling(100); }
      if(CheckBothButtonPress()){
        return false;
      }
    // если дракончик не столкнулся, то меняем положение препятствия
    // начинаем считать сколько циклов дракончик находится в прыжке
    delayWithAlarmHandling(gamePause);
          if(CheckBothButtonPress()){
        return false;
      }
    x = x - 1;
    p = p + 1;
    if (p > 4) p = 0; 
 }
      if(CheckBothButtonPress()){
        return false;
      }
  // переходим на следующий уровень и сокращаем время задержки
  tone(18, 800);
  delayWithAlarmHandling(20);
  level = level + 1;
  gamePause = gamePause - 20;
  if (gamePause < 0) gamePause = 0;
  return true;
}
//


void StartMenu(){

  byte menuNumber = 1;
  
  Serial.println("Menu");

  int button1 = 0;
  
  while(button1 !=3){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Menu:");
    if(button1 == 0)   delayWithAlarmHandling(500);
    lcd.setCursor(7, 0);
    if(menuNumber == 1){
      lcd.print("Alarm -<");
    }else lcd.print("Alarm ");
    
    lcd.setCursor(7, 1);
    if(menuNumber == 2)
      lcd.print("Info  -<");  
    else lcd.print("Info");

    button1 = WaitButtonPress();
    if(button1 == 1){
      menuNumber++;
      if(menuNumber>2){
        menuNumber = 1;
      }
    }
    if(button1 == 2){
      //setAlarm
      if(menuNumber == 1){
        TimeBytes time(8,0,0);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Setting alarm:");
        int button2 = 0;
          int timeSwitchMode = 1;
        while(button2 !=3){
          lcd.setCursor(2,1);
          lcd.print(FormatTimeFromBytes(time.hour, time.min, time.sec) + "   " + String(timeSwitchMode) );
          button2 = WaitButtonPress();
          Serial.println(button2);
          Serial.println(timeSwitchMode);
          if(button2 ==1){
            if(timeSwitchMode == 1){
              time.hour++;
              if(time.hour >23) time.hour = 0;
            }           
            if(timeSwitchMode == 2){
              time.min++;
              if(time.min >59) time.min = 0;
            }            
            if(timeSwitchMode == 3){
              time.sec++;
              if(time.sec >59) time.sec = 0;
            }
          }          
          if(button2 == 2){
            timeSwitchMode++;

            if(timeSwitchMode>3){
              timeSwitchMode = 1;
            }
            delayWithAlarmHandling(100);
          }
          if(button2 == 3){
            SetAlarm(FormatTimeFromBytes(time.hour, time.min, time.sec));
            delayWithAlarmHandling(500);
            return;
          }
        }
      }
      if(menuNumber == 2){

        int page = 0;
        int selectedMenu2 = 0;
        int button2 = 0;

        lcd.clear();
        lcd.setCursor(0,0);
        if(selectedMenu2==0)lcd.print(">Net:" + String(ssid));
        else lcd.print(" Net:" + String(ssid));
        lcd.setCursor(0,1);
        if(selectedMenu2==1)lcd.print(">IP:" + address.toString());
        else lcd.print(" IP:" + address.toString());

        while(button2 !=3){
          button2 = WaitButtonPress();
          if(button2 == 1){
            delayWithAlarmHandling(100);
            selectedMenu2++;
            if(selectedMenu2 == 1)
              page = 1;            
            if(selectedMenu2 == 2)
              page = 2;
            
            if(selectedMenu2 >3)
            {  
              page = 0;
              selectedMenu2 = 0;
            }
            if(page == 0){
              lcd.clear();
              lcd.setCursor(0,0);
              if(selectedMenu2==0)lcd.print(">Net:" + String(ssid));
              else lcd.print(" Net:" + String(ssid));
              lcd.setCursor(0,1);
              if(selectedMenu2==1)lcd.print(">IP:" + address.toString());
              else lcd.print(" IP:" + address.toString());
            }
            if(page == 1){
                lcd.clear();
                lcd.setCursor(0,0);
                if(selectedMenu2==1)lcd.print(">IP:" + address.toString());
                else lcd.print(" IP:" + address.toString());
                lcd.setCursor(0,1);
                if(selectedMenu2==2)lcd.print(">Sleepmode: " + String(AllowSleepMode));
                else lcd.print(" Sleepmode: " + String(AllowSleepMode));
            }          
            if(page == 2){
                lcd.clear();
                lcd.setCursor(0,0);
                if(selectedMenu2==2)lcd.print(">Sleepmode: " + String(AllowSleepMode));
                else lcd.print(" Sleepmode: " + String(AllowSleepMode));

                lcd.setCursor(0,1);
                if(selectedMenu2==3)lcd.print(">Jumping game ");
                else lcd.print(" Jumping game ");
            }
          }
          if(button2 ==2){
            if(selectedMenu2 == 2){
              AllowSleepMode = !AllowSleepMode;
                lcd.setCursor(0,0);
                if(selectedMenu2==2)lcd.print(">Sleepmode: " + String(AllowSleepMode));
                else lcd.print(" Sleepmode: " + String(AllowSleepMode));
            }
            if(selectedMenu2 == 3){
                lcd.clear();
                setupGame();
                while(loopGame()){
                  
                }
            }            
          }
        }
      }
    }
    if(button1 == 3){
      delayWithAlarmHandling(500);
      return;
    }
  }
      
}



