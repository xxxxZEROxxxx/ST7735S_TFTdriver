//
// Created by Arcana on 8/4/26.
//

#ifndef TFT_SCREEN_TFT_DRIVER_H
#define TFT_SCREEN_TFT_DRIVER_H


#include <stdint.h>

#endif //TFT_SCREEN_TFT_DRIVER_H

#define SWERSET 0x01  //software reset

#define SLPIN 0x10  //enter sleep mode
#define SLPOUT 0x11 // wake up
#define PTLON 0x12  //turn on partial mode, the partial mode window is described by 0x30
#define NORON 0x13  //normal disp on, which is the default vlaue
#define INVOFF 0x20 //invert off
#define INVON 0x21  //invert on
#define GAMSET  0x26  //set gamma curve, just use default value, no extra
#define DISPOFF 0x28  //enter into display off mode
#define DISPON  0x29  //turn it back on, should invoke in the startup
#define CASET   0x2A  //CASET, column address set
/**
 * follow by 4 parameter,each under the D/CX = 1
 *      The value of XS [7:0] and XE [7:0] are referred when RAMWR command comes.
 *      1,2 st parameter is XS [15:8][7:0]
 *      3,4,st parameter is XE [15:8][7:0]
 *
*      3. 132X162 memory base (GM = ’00’)
(Parameter range: 0 < XS [15:0] < XE [15:0] < 131 (0083h)): MV=”0”)
(Parameter range: 0 < XS [15:0] < XE [15:0] < 161 (00A1h)): MV=”1”)
 */
#define RASET   0x2B    //row address set
/**
 *follow by 4 parameter,each under the D/CX = 1
 *      the value of YS [7:0] and YE [7:0] are referred when RAMWR command comes.
 *      1,2 st parameter is YS [15:8][7:0]
 *      3,4 st parameter is YE [15:8][7:0]
 *
*      3. 132X162 memory base (GM = ’00’)
(Parameter range: 0 < YS [15:0] < YE [15:0] < 161 (00A1h)): MV=”0”
(Parameter range: 0 < YS [15:0] < YE [15:0] < 131 (0083h)): MV=”1”
 */
#define RAMWR   0x2C   //memory write
/**
 *  write memory pixel data, after command 0x2c is sent, pull D/CX line to high for all time
 *  parameter:
 *              1st parameter, 2th parameter ...... nth parameter, each 8 bit
 *
3. 132x162 memory base (GM = ‘00’)
132x162x18-bit memory can be written on this command.
Memory range: (0000h, 0000h) -> (0083h, 00A1h)
 */
#define RGBSET 0x2D   //color set for 4k, 65k, 262k, and 16.7M
/** after command 0x2d is sent, pull D/CX line to high for all time
   it has 128 parameter
  128-Bytes must be written to the LUT regardless of the color mode. Only the values in
Section 9.18 are referred.
for detail refer to p.133
 */
#define RAMRD  0x2E   //read data from frame memory to MCU

#define PTLAR   0x30   //define the partial mode's display area
/** after command 0x30 is sent, pull D/CX line to high
 * 4 parameters associaled with this command
       first defines the start row (PSL)
       second the end row (PEL)
       as the format as before [15:8][7:0]
 */
#define SCRLAR  0x33   //scroll area set
#define TEOFF   0x34
#define TEON    0x35

#define MADCTL  0x36   //Memory data access control
/** only one parameter.
* D7   D6   D5   D4   D3   D2   D1   D0
  MY   MX   MV   ML   RGB  MH   -    -
  0    0    0    0    0    0    0    0  // for my setting which is 0x00
// current set up MX, Y, L = 0
| Bit | Name | Function                         |
| --- | ---- | -------------------------------- |
| D7  | MY   | flip vertically (top ↔ bottom)   |
| D6  | MX   | flip horizontally (left ↔ right) |
| D5  | MV   | swap X/Y (rotate 90°)            |
| D4  | ML   | vertical refresh order           |
| D3  | RGB  | color order (RGB/BGR)       0 for RGB panel; 1 for GBR panel
| D2  | MH   | horizontal refresh order         |

default value after power up MY=0,MX=0,MV=0,ML=0,RGB=0,MH=0
 */
#define VSCSAD  0x37   //Vertical Scroll Start Address of RAM
#define IDMOFF  0x38   //idle mode off
#define IDMON   0x39   //idle mode on, color expression reduced
#define COLMOD  0x3A   //interface pixel format, used to select color format 444, 565, 666
/**  have one parameter

     D7   D6   D5   D4   D3   D2   D1   D0
     -    -    -    -    -    O    O    O         only [2:0] is used
     011 -3- 12 bit pixel
     101 -5- 16 bit pixel // my setting which is 0x05
     110 -6- 18 bit pixel
     111 -7- not used.

 */
#define RDID1   0xDA
#define RDID2   0xDB
#define RDID3   0xDC

#define DC_COMMAND HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
#define DC_DATA HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)
#define RES_LOW HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define RES_HIGH HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define TCS_LOW HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
#define TCS_HIGH HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)

void tft_send(uint8_t *data, _Bool type, uint16_t length);
void tft_init(void);
void tft_def_window(uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye);
void send_data(uint32_t start_addr, uint32_t end_addr);