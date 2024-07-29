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

#include <LiquidCrystal.h>
#include <stdlib.h>
#include <limits.h>
enum DisplayItem {GRAPHIC_ITEM_NONE, GRAPHIC_ITEM_A, GRAPHIC_ITEM_B, GRAPHIC_ITEM_NUM};
#define GRAPHIC_WIDTH 16
#define GRAPHIC_HEIGHT 4

byte block[3] = {
  B01110,
  B01110,
  B01110,
};

byte apple[3] = {
  B00100,
  B01010,
  B00100,
};

#define DEBOUNCE_DURATION 20

//Return true if the actual output value is true
bool debounce_activate(unsigned long* debounceStart)
{
  if(*debounceStart == 0)
    *debounceStart = millis();
  else if(millis()-*debounceStart>DEBOUNCE_DURATION)
    return true;
  return false;
}


//Return true if it's rising edge/falling edge
bool debounce_activate_edge(unsigned long* debounceStart)
{
  if(*debounceStart == ULONG_MAX){
    return false;
  }else if(*debounceStart == 0){
    *debounceStart = millis();
  }else if(millis()-*debounceStart>DEBOUNCE_DURATION){
    *debounceStart = ULONG_MAX;
    return true;
  }
  return false;
}

void debounce_deactivate(unsigned long* debounceStart){
  *debounceStart = 0;
}


#define BUTTON_LEFT 16
#define BUTTON_RIGHT 17
unsigned long debounceCounterButtonLeft = 0;
unsigned long debounceCounterButtonRight = 0;

struct Pos {
  uint8_t x=0, y=0;
};



struct Pos snakePosHistory[GRAPHIC_HEIGHT*GRAPHIC_WIDTH]; //first element is the head.
size_t snakeLength = 0;
enum {SNAKE_LEFT,SNAKE_UP,SNAKE_RIGHT,SNAKE_DOWN} snakeDirection;
struct Pos applePos;
unsigned long lastGameUpdateTick = 0;
unsigned long gameUpdateInterval = 1000;
bool thisFrameControlUpdated = false;
enum {GAME_MENU, GAME_PLAY, GAME_LOSE, GAME_WIN} gameState;


uint8_t graphicRam[GRAPHIC_WIDTH*2/8][GRAPHIC_HEIGHT];


void graphic_generate_characters()
{
  /*
  space: 0 0
  0: 0 A
  1: 0 B
  2: A 0
  3: A A
  4: A B
  5: B 0
  6: B A
  7: B B
   */
  for (size_t i=0; i<8; i++) {
    byte glyph[8];
    int upperIcon = (i+1)/3;
    int lowerIcon = (i+1)%3;

    memset(glyph, 0, sizeof(glyph));
    if(upperIcon==1)
      memcpy(&glyph[0], &block[0], 3);
    else if(upperIcon==2)
      memcpy(&glyph[0], &apple[0], 3);

    if(lowerIcon==1)
      memcpy(&glyph[4], &block[0], 3);
    else if(lowerIcon==2)
      memcpy(&glyph[4], &apple[0], 3);

    lcd.createChar(i, glyph);
  }
  delay(1); //Wait for the CGRAM to be written
}

void graphic_clear(){
  memset(graphicRam, 0, sizeof(graphicRam));
}

void graphic_add_item(uint8_t x, uint8_t y, enum DisplayItem item) {
  graphicRam[x/(8/2)][y] |= (uint8_t)item*(1<<((x%(8/2))*2));
}


void graphic_flush(){
  lcd.clear();
  for(size_t x=0; x<16; x++){
    for(size_t y=0; y<2; y++){
      enum DisplayItem upperItem = (DisplayItem)((graphicRam[x/(8/2)][y*2]>>((x%(8/2))*2))&0x3);
      enum DisplayItem lowerItem = (DisplayItem)((graphicRam[x/(8/2)][y*2+1]>>((x%(8/2))*2))&0x3);
      if(upperItem>=GRAPHIC_ITEM_NUM)
        upperItem = GRAPHIC_ITEM_B;
      if(lowerItem>=GRAPHIC_ITEM_NUM)
        lowerItem = GRAPHIC_ITEM_B;
      lcd.setCursor(x, y);
      if(upperItem==0 && lowerItem==0)
        lcd.write(' ');
      else
        lcd.write(byte((uint8_t)upperItem*3+(uint8_t)lowerItem-1));
    }
  }
}


void game_new_apple_pos()
{
  bool validApple = true;
  do{
    applePos.x = rand()%GRAPHIC_WIDTH;
    applePos.y = rand()%GRAPHIC_HEIGHT;
    validApple = true;

    for(size_t i=0; i<snakeLength; i++)
    {
      if(applePos.x==snakePosHistory[i].x && applePos.y==snakePosHistory[i].y){
        validApple = false;
        break;
      }
    }
  } while(!validApple);

}

void game_init(){
  srand(micros());
  gameUpdateInterval = 1000;
  gameState = GAME_PLAY;

  snakePosHistory[0].x=3; snakePosHistory[0].y=1;
  snakePosHistory[1].x=2; snakePosHistory[1].y=1;
  snakePosHistory[2].x=1; snakePosHistory[2].y=1;
  snakePosHistory[3].x=0; snakePosHistory[3].y=1;
  snakeLength = 4;
  snakeDirection = SNAKE_RIGHT;
  game_new_apple_pos();
  thisFrameControlUpdated = false;
}

void game_calculate_logic() {
  if(gameState!=GAME_PLAY) //Game over. Don't bother calculating game logic.
    return;

  //Calculate the movement of the tail
  for(size_t i=snakeLength; i>=1; i--){ //We intentionally use i=snakeLength instead of i=snakeLength-1 so that the snake will be lengthened right after it eats the apple
    snakePosHistory[i] = snakePosHistory[i-1];
  }
  //Calculate the head movement
  snakePosHistory[0]=snakePosHistory[1];
  switch(snakeDirection){
    case SNAKE_LEFT:  snakePosHistory[0].x--; break;
    case SNAKE_UP:    snakePosHistory[0].y--; break;
    case SNAKE_RIGHT: snakePosHistory[0].x++; break;
    case SNAKE_DOWN:  snakePosHistory[0].y++; break;
  }

  //Look for wall collision
  if(snakePosHistory[0].x<0||snakePosHistory[0].x>=GRAPHIC_WIDTH||snakePosHistory[0].y<0||snakePosHistory[0].y>=GRAPHIC_HEIGHT){
    gameState = GAME_LOSE;
    return;
  }

  //Look for self collision
  for(size_t i=1; i<snakeLength; i++){
    if(snakePosHistory[0].x==snakePosHistory[i].x && snakePosHistory[0].y==snakePosHistory[i].y){
      gameState = GAME_LOSE;
      return;
    }
  }

  if(snakePosHistory[0].x==applePos.x && snakePosHistory[0].y==applePos.y){
      snakeLength++;
      gameUpdateInterval = gameUpdateInterval*9/10;
      if(snakeLength>=sizeof(snakePosHistory)/sizeof(*snakePosHistory))
        gameState = GAME_WIN;
      else
        game_new_apple_pos();
  }

  
}

void game_calculate_display() {
  graphic_clear();
  switch(gameState){
    case GAME_LOSE:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Boo! You lose!");
      lcd.setCursor(0, 1);
      lcd.print("Length: ");
      lcd.setCursor(8, 1);
      lcd.print(snakeLength);
    break;
    case GAME_WIN:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("You won. Congratz!");
      lcd.setCursor(0, 1);
      lcd.print("Length: ");
      lcd.setCursor(8, 1);
      lcd.print(snakeLength);
    break;
    case GAME_PLAY:
      for(size_t i=0; i<snakeLength; i++)
        graphic_add_item(snakePosHistory[i].x, snakePosHistory[i].y, GRAPHIC_ITEM_A);
      graphic_add_item(applePos.x, applePos.y, GRAPHIC_ITEM_B);
      graphic_flush();
    break;
    case GAME_MENU:
      //Do nothing
    break;
  }
}



void snakeSetup() {
  lcd.begin(16, 2);
  lcd.print("1602 LCD Snake");
   lcd.setCursor(0, 1);
  lcd.print("- Sadale.net");

  graphic_generate_characters();
  gameState = GAME_MENU;
}

bool snakeLoop() {
    CheckAlarm();
  AlarmUntilTouch(); 
   if(CheckBothButtonPress()){
        return false;
      } 
  lcd.setCursor(0, 0);
  if(digitalRead(BUTTON_LEFT)==HIGH){
      if(debounce_activate_edge(&debounceCounterButtonLeft)&&!thisFrameControlUpdated){
        switch(gameState){
          case GAME_PLAY:
            switch(snakeDirection){
              case SNAKE_LEFT:   snakeDirection=SNAKE_DOWN;  break;
              case SNAKE_UP:     snakeDirection=SNAKE_LEFT;  break;
              case SNAKE_RIGHT:  snakeDirection=SNAKE_UP;    break;
              case SNAKE_DOWN:   snakeDirection=SNAKE_RIGHT; break;
            }
            thisFrameControlUpdated = true;
          break;
          case GAME_MENU:
            game_init();
          break;
        }
      }
  }else{
      debounce_deactivate(&debounceCounterButtonLeft);
  }
 if(CheckBothButtonPress()){
        return false;
      }
  lcd.setCursor(8, 0);
  if(digitalRead(BUTTON_RIGHT)==HIGH){
      if(debounce_activate_edge(&debounceCounterButtonRight)&&!thisFrameControlUpdated){
        switch(gameState){
          case GAME_PLAY:
            switch(snakeDirection){
              case SNAKE_LEFT:   snakeDirection=SNAKE_UP;    break;
              case SNAKE_UP:     snakeDirection=SNAKE_RIGHT; break;
              case SNAKE_RIGHT:  snakeDirection=SNAKE_DOWN;  break;
              case SNAKE_DOWN:   snakeDirection=SNAKE_LEFT;  break;
            }
            thisFrameControlUpdated = true;
          break;
          case GAME_MENU:
            game_init();
          break;
        }
      }
  }else{
      debounce_deactivate(&debounceCounterButtonRight);
  }
  CheckAlarm();
  AlarmUntilTouch();  
   if(CheckBothButtonPress()){
        return false;
      }
  if(millis()-lastGameUpdateTick>gameUpdateInterval){
    game_calculate_logic();
    game_calculate_display();
    lastGameUpdateTick = millis();
    thisFrameControlUpdated = false;
  }
  return true;
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
            if(selectedMenu2 == 3)
              page = 3;
            
            if(selectedMenu2 >4)
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
            if(page == 3){
                lcd.clear();
                lcd.setCursor(0,0);
                if(selectedMenu2==3)lcd.print(">Jumping game ");
                else lcd.print(" Jumping game ");

                lcd.setCursor(0,1);
                if(selectedMenu2==4)lcd.print(">Snake game");
                else lcd.print(" Snake game");
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
            if(selectedMenu2 == 4){
                lcd.clear();
                snakeSetup();
                while(snakeLoop()){
                  
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



