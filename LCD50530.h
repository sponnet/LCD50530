/*
LCD driver for Mitsubishi 50530 compatible displays

*/

#ifndef LCD50530_h
#define LCD50530_h

#include <inttypes.h>

// Full commands
#define LCD_NOOP 0x00
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x03

// Partial commands (with flags)
#define LCD_BLINKFREQSET 0x04		// with flags
#define LCD_WRITERAMULINESET 0x08	// with flags
#define LCD_UNDERLINEMODESET 0x0C	// with flags
#define LCD_SHIFTCOMMAND 0x10		// with flags
#define LCD_DISPLAYMODESET 0x20		// with flags
#define LCD_ENTRYMODESET 0x40		// with flags
#define LCD_FUNCTIONMODESET 0xC0	// with flags

//#define LCD_SETCGRAMADDR 0x40
//#define LCD_SETDDRAMADDR 0x80

// flags for function mode set
#define LCD_8BITMODE 0x20
#define LCD_4BITMODE 0x00
#define LCD_4LINE 0x08
#define LCD_2LINE 0x04
#define LCD_1LINE 0x00
#define LCD_5x12DOTS 0x00
#define LCD_5x8DOTS 0x10
#define LCD_160CHAR 0x03
#define LCD_192CHAR 0x02
#define LCD_224CHAR 0x01
#define LCD_256CHAR 0x00

// TODO: RAM -- setting region (page 28/29 of datasheet)

// flags for display set entry mode
#define LCD_DISPLAYMOVEREAD 0x01	// Update display address after reading RAM data
#define LCD_DISPLAYMOVEWRITE 0x02	// Update display address after writing RAM data
#define LCD_CURSORMOVEREAD 0x08		// Update cursor address after reading RAM data
#define LCD_CURSORMOVEWRITE 0x10	// Update cursor address after writing RAM data
#define LCD_DISPLAYMOVERIGHT 0x00	// Display address decremented after instruction
#define LCD_DISPLAYMOVELEFT 0x04	// Display address incremented after instruction
#define LCD_CURSORMOVELEFT 0x20		// Cursor address decremented after instruction
#define LCD_CURSORMOVERIGHT 0x00	// Cursor address incremented after instruction

#define LCD_LEFT	0x04
#define LCD_RIGHT	0x00
#define LCD_READ	0x01
#define LCD_WRITE	0x02

// flags for display on/off control
#define LCD_DISPLAYON 0x10		// Turn all displays on
#define LCD_DISPLAYOFF 0x00		// Turn all displays off
#define LCD_CURSORON 0x08		// Turn on cursor display
#define LCD_CURSOROFF 0x00		// Turn off cursor display
#define LCD_UNDERLINEON 0x04	// Turn on underline display
#define LCD_UNDERLINEOFF 0x00	// Turn off underline display
#define LCD_CURSORBLINKON 0x02	// Blinking cursor display
#define LCD_CURSORBLINKOFF 0x00	// Cursor display without blinking
#define LCD_CHRBLINKON 0x01		// Blinking Character display of cursor position
#define LCD_CHRBLINKOFF 0x00	// Character display without blinking
 
// flags for display/cursor shift command
#define LCD_DISPLAYSHIFT 0x02
#define LCD_CURSORSHIFT 0x08
#define LCD_SHIFTLEFT 0x01
#define LCD_SHIFTRIGHT 0x00

// TODO: flags for underline mode instruction (SU)
// TODO: flags for Write underline bit instruction (WU)
// TODO: flags for Set blink frequency instruction (SB)

class LCD50530 {
public:  
  LCD50530(uint8_t ioc1, uint8_t ioc2, uint8_t rw, uint8_t ex, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

  void init(uint8_t ioc1, uint8_t ioc2, uint8_t rw, uint8_t ex, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  void clear();
  void home();

  void noDisplay();
  void display();
  void noCursor();
  void cursor();
  void noUnderline();
  void underline();
  void noCursorBlink();
  void cursorBlink();
  void noCharBlink();
  void charBlink();

  void moveCursorAfterInstruction(uint8_t direction, uint8_t instruction);
  void moveDisplayAfterInstruction(uint8_t direction, uint8_t instruction);
  void holdCursor();
  void holdDisplay();

  void shiftCursor(uint8_t direction);
  void shiftDisplay(uint8_t direction);
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();

  //void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t); 
  void command(uint8_t);
  void writechar(uint8_t);
private:
  void send(uint8_t, uint8_t);
  void write4bits(uint8_t, uint8_t);
  void pulseExecute();
  uint8_t busyState();

  uint8_t _ioc1_pin; // LOW: command.  HIGH: character.
  uint8_t _ioc2_pin; // LOW: command.  HIGH: character.
  uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
  uint8_t _ex_pin; // activated by a HIGH pulse.
  uint8_t _data_pins[4];

  uint8_t _functionmode;
  uint8_t _displaymode;
  uint8_t _entrymode;

  uint8_t _initialized;

  uint8_t _numlines, _currline;
  uint8_t _cols;
};

#endif

