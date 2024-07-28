#pragma once
#include <EEPROM.h>
byte ActiveNote = 0;
void LogSerial(String s){
  Serial.println(s);
}

void write_string_EEPROM (int Addr, String Str) {
    byte lng=Str.length();
    
    EEPROM.begin(255);
    LogSerial("eeprom begin");
    if (lng>16 )  lng=0;
    EEPROM.write(Addr , lng); 
    unsigned char* buf = new unsigned char[16];
    Str.getBytes(buf, lng + 1);
    Addr++;
    for(byte i = 0; i < lng; i++) {EEPROM.write(Addr+i, buf[i]); delay(10);}
    EEPROM.commit();
}

char *read_string_EEPROM (int Addr) {
    EEPROM.begin(255);
    LogSerial("eeprom begin");

    byte lng = EEPROM.read(Addr);
    char* buf = new char[16];
    Addr++;
    for(byte i = 0; i < lng; i++) buf[i] = char(EEPROM.read(i+Addr));
    buf[lng] = '\x0';
    EEPROM.commit();

    return buf;
}

byte GetCountOfNotes(){
  EEPROM.begin(255);
  LogSerial("eeprom begin");

  byte countOfNotes = EEPROM.read(0);
  return countOfNotes;
}
void initEEPROM(){
  byte count = GetCountOfNotes();
  
  if(count>=0&&count<=10){
    return;
  }
  count = 0;
  EEPROM.begin(255);
  EEPROM.put(0, count);
  EEPROM.commit();
}

void WriteNoteToEEPROM(String note){

  byte countOfNotes = GetCountOfNotes();
  if(countOfNotes>10){
    countOfNotes = 0;
      EEPROM.begin(255);
    EEPROM.put(0, countOfNotes);
    EEPROM.commit();
  }
  write_string_EEPROM(countOfNotes*17+1, note);
    EEPROM.begin(255);
  EEPROM.put(0, ++countOfNotes);
  EEPROM.commit();
  ActiveNote = countOfNotes;
  
}
String ReadNotesFromEEPROM(bool html = true){
  String result  = "";
  byte countOfNotes = GetCountOfNotes();
  
  for(int i = 0; i<countOfNotes; i++){
    if(html)result += "<li>";
    result += read_string_EEPROM(i*17+1);
    if(html)result += "</li>\n";
    if(!html)result +="\n";
  }
  return result;
  
}
String ReadNoteFromEEPROM(byte id){

  return  read_string_EEPROM(id*17+1);
  
}

struct AllNotes{
  String note[11];
  int count = 0;
  int rewritecount = 0;
  void InitNotes(){
    // count = GetCountOfNotes();
    // for (byte i = 0; i<count; i++) {
    //   note[i] =  ReadNoteFromEEPROM(i);
    // }
   
  }
  void WriteNoteToMemory(String toMem){
    // WriteNoteToEEPROM(toMem);
    note[rewritecount] = toMem;
    count++;
    rewritecount++;
    if(count>9){
      count = 10;
    }    
    if(rewritecount>9){
      rewritecount = 0;
    } 
    ActiveNote = rewritecount;
  }
  String GetHtmlNotes(){
    String result = "";
    for (byte i = 0; i<count; i++) {
      result += "<li>";
      result += note[i];
      result += "</li>";
    }
    return result;
  }
  String Next(){
    ActiveNote++;
    if(ActiveNote>=count){
      ActiveNote = 0;
    }
    return note[ActiveNote];
  }
}MainNotes;


