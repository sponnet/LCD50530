/*
LCD driver for Mitsubishi 50530 compatible displays

*/

#ifndef LCD50350_h
#define LCD50350_h

#include "Print.h"

class LCD50350 : public Print {

public:
  LCD50350();
  void init();
  void clear();
  void home();	
  virtual void write(uint8_t);

private:

};

#endif

