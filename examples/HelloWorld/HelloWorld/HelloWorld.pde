#include <LCD50350.h>

LCD50350 lcd;

void setup(){
  
  lcd.init();
  lcd.home();
  lcd.print("Hello World!");
  
}

void loop() {
  
}



