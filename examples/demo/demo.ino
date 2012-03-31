#include <LCD50530.h>

#define writedelay 100

LCD50530 lcd(1,0,2,3,4,5,6,7);

const char line1[] = "Welcome to www.opengarage.org test, v3";
const char line2[] = "The LCD is connected to an arduino!";
const char line3[] = {'B', 'A', 'R', '/', 'G', 'A', 'U', 'G', 'E', ':', ' ', 0x88,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x89};

void setup() {
  lcd.begin(40,4);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.leftToRight();
  
  for(int b2=0; b2<sizeof(line1)-1; b2++) {
    lcd.writechar(line1[b2]);
  }
    
  lcd.setCursor(0,1);
  for(int b2=0; b2<sizeof(line2)-1; b2++) {
    lcd.writechar(line2[b2]);
  }
  
  lcd.setCursor(0,2);
  for(int b2=0; b2<sizeof(line3); b2++) {
    lcd.writechar(line3[b2]);
  }
  
  while(true)
    Gauge();
}

void Gauge() {   
  lcd.setCursor(11,2);   
  lcd.leftToRight();
      
  for(int b2=0; b2<28; b2++) {
    lcd.writechar(0x9B);
    delay(writedelay);
  }

  // Don't move cursor after write
  lcd.holdCursor();
  lcd.writechar(0x9B);
  delay(writedelay);
    
  // Set cursor to move left after write    
  lcd.rightToLeft();
  lcd.writechar(0x89);
  delay(writedelay);
    
  for(int b2=12; b2<39; b2++) {
    lcd.writechar(0x97);
    delay(writedelay);
  }

  // Don't move cursor after write
  lcd.holdCursor();
  lcd.writechar(0x88);
  delay(writedelay);
}

