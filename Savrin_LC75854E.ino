/*

fork from https://github.com/RodLophus/SanyoCCB

modified library to fit for LC75854
1) CCB address byte, no swap, direct from LC75854 datasheet, 0x42
2) CCB data bytes, MSB to LSB
3) CCB byte, MSb to LSb
4) CCB_DELAY changed from 100 to 1, DO output high, delay 3.5us, CL high

Savrin, car, Panasonic Car audio with LCD module,
LCD controller is LC75854
COM1 - COM4, used
S1 - S35, used
total 4x35 = 140 segments
actual display segments = D1 to D123

Test done.
2019-SEP-23, by xiaolaba

        
Arduino connections:
Arduino                SAVRIN LCD
D3 (DO)                  DI
D4 (CL)                  CL
D5 (CE)                  CE
D6 (DI)                  
GND                      GND


*/


#include <SanyoCCB.h>

SanyoCCB ccb(3, 4, 6, 5); // Arduino Pins: DO CL DI CE, our case, uses 3,4 & 5 only. LCD has no data send back to Arduino

#define LC75854E 0x42 //ccb address, see LC75854 datasheet for "Serial Data Input"

uint8_t lcd_full []={0xff,0xff,0xff,0xff,0xff,0xF0,0x00, // read datasheet for Serial Data Input, 7 bytes x 4 packet, all segment displayed
                     0xff,0xff,0xff,0xff,0xff,0x00,0x01,
                     0xff,0xff,0xff,0xff,0xff,0x00,0x02,
                     0xff,0xff,0xff,0xff,0xff,0x00,0x03};

uint8_t lcd_clear[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00, // read datasheet for Serial Data Input, 7 bytes x 4 packet, all segment cleared
                     0x00,0x00,0x00,0x00,0x00,0x00,0x01,
                     0x00,0x00,0x00,0x00,0x00,0x00,0x02,
                     0x00,0x00,0x00,0x00,0x00,0x00,0x03};

uint8_t lcd_map[] = {0xff,0xff,0xff,0xff,0xff,0xF0,0x00, // read datasheet for Serial Data Input, 7 bytes x 4 packet, buffer will dump to LCD
                     0xff,0xff,0xff,0xff,0xff,0x00,0x01,
                     0xff,0xff,0xff,0xff,0xff,0x00,0x02,
                     0xff,0xff,0xff,0xff,0xff,0x00,0x03};



uint8_t counter;  // show segment number for cleared


void LCD_roll_dot() { // segment data, D1 to D164, cleared one by one, and then dump to LCD module
  /** address, data, data_lenght **/
  if (lcd_map[0] != 0) {lcd_map[0] >>= 1;}
  if (lcd_map[0] == 0) {lcd_map[1] >>= 1;}
  if (lcd_map[1] == 0) {lcd_map[2] >>= 1;}
  if (lcd_map[2] == 0) {lcd_map[3] >>= 1;}
  if (lcd_map[3] == 0) {lcd_map[4] >>= 1;}
  if (lcd_map[4] == 0) {lcd_map[5] >>= 1; lcd_map[5] &= 0xF0;}

  if (lcd_map[5] == 0) {lcd_map[7] >>= 1;}        
  if (lcd_map[7] == 0) {lcd_map[8] >>= 1;}        
  if (lcd_map[8] == 0) {lcd_map[9] >>= 1;}        
  if (lcd_map[9] == 0) {lcd_map[10] >>= 1;}        
  if (lcd_map[10] == 0) {lcd_map[11] >>= 1;}

  if (lcd_map[11] == 0) {lcd_map[14] >>= 1;}        
  if (lcd_map[14] == 0) {lcd_map[15] >>= 1;} 
  if (lcd_map[15] == 0) {lcd_map[16] >>= 1;}        
  if (lcd_map[16] == 0) {lcd_map[17] >>= 1;}        
  if (lcd_map[17] == 0) {lcd_map[18] >>= 1;}        

  if (lcd_map[18] == 0) {lcd_map[21] >>= 1;}        
  if (lcd_map[21] == 0) {lcd_map[22] >>= 1;} 
  if (lcd_map[22] == 0) {lcd_map[23] >>= 1;}        
  if (lcd_map[23] == 0) {lcd_map[24] >>= 1;}        
  if (lcd_map[24] == 0) {lcd_map[25] >>= 1;} 
  delay(100);       
}


void LCD_clear_screen() {
  /** address, data, data_lenght **/
  ccb.write(LC75854E, lcd_clear, 7);
  ccb.write(LC75854E, lcd_clear+7, 7);
  ccb.write(LC75854E, lcd_clear+14, 7);
  ccb.write(LC75854E, lcd_clear+21, 7);    
  }

void LCD_full_screen() {
  /** address, data, data_lenght **/
  ccb.write(LC75854E, lcd_full, 7);
  ccb.write(LC75854E, lcd_full+7, 7);
  ccb.write(LC75854E, lcd_full+14, 7);
  ccb.write(LC75854E, lcd_full+21, 7);    
  }

void LCD_reset_map (){
  for (uint8_t i =0; i<=26; i++) {
    lcd_map[i] = lcd_full[i];
  }
}


/*******************\
 * LCD setup *
\*******************/

void LCD_setup() {  // dump map to LCD module
  // address, data, data_lenght
  ccb.write(LC75854E, lcd_map, 7);
  ccb.write(LC75854E, lcd_map+7, 7);
  ccb.write(LC75854E, lcd_map+14, 7);
  ccb.write(LC75854E, lcd_map+21, 7);    
  }


/*******************\
 * Arduino setup() *
\*******************/ 
void setup() {

  counter =0;
  Serial.begin(57600);
  ccb.init();
  Serial.print("xiaolaba, Savrin Panasonic Car Audio, LC74854, LCD CCB bus test");
  Serial.println("");
  Serial.println("");

  // blink LCD 5 times
  for (uint8_t i =0; i<5; i++) {
    LCD_full_screen();
    delay(300);
    LCD_clear_screen();
    delay(300);
  } 

    LCD_full_screen();
    delay(3000);
}

void(* resetFunc) (void) = 0;//declare reset function at address 0

 

/******************\
 * Arduino loop() *
\******************/ 
void loop() {
  Serial.print("D"); Serial.println(counter,DEC); 
  LCD_setup();  // output lcd_map to LCD module
  LCD_roll_dot(); // clear one bit of lcd_map each time, from MSb to LSb, total 164 bits data, LCD segment cleared one by one
  //delay(2000);  //if you want see actual segment number vs physical LCD display, enable this delay for visual easy
  counter++;
  if (counter > 123){
    LCD_reset_map ();
    delay(1500);
    resetFunc(); // software reset the MCU, come again
  }

  
}
  
