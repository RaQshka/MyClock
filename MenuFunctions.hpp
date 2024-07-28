int countOfSignals = 0;

long long 
void CheckAllButtonsSignals(){
  if(digitalRead(16) && digitalRead(17)){
    countOfSignals++;
  }
  
}