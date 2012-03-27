#include <LCD50530.h>

LCD50530 lcd;

void setup(){
  
  lcd.init();
  lcd.home();
  lcd.print("Hello World!");
  
}

void loop() {
  
}



