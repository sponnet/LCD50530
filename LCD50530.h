/*
LCD driver for Mitsubishi 50530 compatible displays

*/

#ifndef LCD50530_h
#define LCD50530_h

#include "Print.h"

class LCD50530 : public Print {

public:
  LCD50530();
  void init();
  void clear();
  void home();	
  virtual void write(uint8_t);

private:

};

#endif

