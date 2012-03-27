#include "LCD50530.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LCD50530 constructor is called).

LCD50530::LCD50530(uint8_t ioc1, uint8_t ioc2, uint8_t rw, uint8_t ex, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	init(0, ioc1, ioc2, rw, ex, d0, d1, d2, d3, d4, d5, d6, d7);
}
		
LCD50530::LCD50530(uint8_t ioc1, uint8_t ioc2, uint8_t rw, uint8_t ex, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	init(1, ioc1, ioc2, rw, ex, 0, 0, 0, 0, d4, d5, d6, d7);
}

void LCD50530::init(uint8_t fourbitmode, uint8_t ioc1, uint8_t ioc2, uint8_t rw, uint8_t ex, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  _ioc1_pin = ioc1;
  _ioc2_pin = ioc2;
  _rw_pin = rw;
  _ex_pin = ex;
  
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7; 

  DDRD = B11111111; // Default: Set all ports to output (Atmel shortcut)
  
  if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else 
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
  begin(16, 1);  
}

void LCD50530::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  /*
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x12DOTS;
  }
  
  //put the LCD into 4 bit or 8 bit mode
  if (! (_displayfunction & LCD_8BITMODE)) {
	// TODO: DO SOMETHING 
  } else {

  }

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONMODESET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_CURSORBLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_CURSORMOVEWRITE | LCD_CURSORMOVERIGHT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);
  */

  command(LCD_FUNCTIONMODESET | LCD_4BITMODE | LCD_5x8DOTS | LCD_4x40);
  command(LCD_CLEARDISPLAY);
}

/********** high level commands, for the user! */
void LCD50530::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
}

void LCD50530::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
}

void LCD50530::setCursor(uint8_t col, uint8_t row)
{
  uint8_t instruction = row*40+col;
  send(instruction, 0x03);
}

// Turn the display on/off (quickly)
void LCD50530::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYMODESET | _displaycontrol);
}
void LCD50530::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYMODESET | _displaycontrol);
}

// Turns the underline cursor on/off
void LCD50530::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYMODESET | _displaycontrol);
}
void LCD50530::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYMODESET | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCD50530::noBlink() {
  _displaycontrol &= ~LCD_CURSORBLINKON;
  command(LCD_DISPLAYMODESET | _displaycontrol);
}
void LCD50530::blink() {
  _displaycontrol |= LCD_CURSORBLINKON;
  command(LCD_DISPLAYMODESET | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCD50530::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_DISPLAYMOVELEFT);
}
void LCD50530::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_DISPLAYMOVERIGHT);
}

// This is for text that flows Left to Right
void LCD50530::leftToRight(void) {
  _displaymode |= LCD_DISPLAYMOVEWRITE | LCD_DISPLAYMOVELEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCD50530::rightToLeft(void) {
  _displaymode &= ~LCD_DISPLAYMOVELEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCD50530::autoscroll(void) {
  _displaymode |= LCD_CURSORMOVEWRITE | LCD_CURSORMOVELEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCD50530::noAutoscroll(void) {
  _displaymode &= ~LCD_CURSORMOVELEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

/*********** mid level commands, for sending data/cmds */

inline void LCD50530::command(uint8_t value) {
  send(value, 0);
}

inline void LCD50530::command(uint8_t value, uint8_t controlpins) {
  send(value, controlpins);
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void LCD50530::send(uint8_t value, uint8_t controlpins) {
  while(busyState());	// Wait while the lcd controller is still busy
  if (_displayfunction & LCD_8BITMODE) {
	while(busyState());
    write8bits(value, controlpins); 
  } else {
    write4bits(value>>4, controlpins);
    write4bits(value, controlpins);
  }

  delay(20);  
}

void LCD50530::pulseExecute(void) {
  digitalWrite(_ex_pin, HIGH);
  delayMicroseconds(2);
  digitalWrite(_ex_pin, LOW);
  delayMicroseconds(2);
}

void LCD50530::write4bits(uint8_t value, uint8_t controlpins) {
  PORTD = (value & B11110000);

  if(controlpins) {
	PORTD |= controlpins;
  }
  
  pulseExecute();
}

void LCD50530::write8bits(uint8_t value, uint8_t controlpins) {
  for (int i = 0; i < 8; i++) {
    pinMode(_data_pins[i], OUTPUT);
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  }
  
  pulseExecute();
}

/*
	Returns the state of the lcd controller
	When 0 is returned, the controller is ready for a new instruction
*/
uint8_t LCD50530::busyState() {
  uint8_t state = 0;
  DDRD = B00001111; // Set data pins to input
  PORTD = B00000000 | 2^_rw_pin;
  
  digitalWrite(_ex_pin, HIGH);
  delayMicroseconds(2);
  state = digitalRead(_data_pins[7]);
  digitalWrite(_ex_pin, LOW);
  delayMicroseconds(2);
  
  PORTD = B00000000 | (2^_rw_pin);
  
  digitalWrite(_ex_pin, HIGH);
  delayMicroseconds(2);
  state |= digitalRead(_data_pins[7]);
  digitalWrite(_ex_pin, LOW);
  delayMicroseconds(2);
  
  DDRD = B11111111; // Reset pins to output
  return state;
}
