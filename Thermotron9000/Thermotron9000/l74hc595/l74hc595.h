/*
l74hc595 lib 0x01

copyright (c) Davide Gironi, 2011

References: bildr 74hc595 library for arduino
	http://bildr.org/2011/08/74hc595-breakout-arduino/

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <avr/io.h>

#ifndef L74HC595_H_
#define L74HC595_H_

//setup ports
#define L74HC595_DDR DDRC
#define L74HC595_PORT PORTC
#define L74HC595_SERPIN PINC1
#define L74HC595_RCLKPIN PINC2
#define L74HC595_SRCLKPIN PINC3

//setup number of chip attached to the board
#define L74HC595_ICNUMBER 1

uint8_t l74hc595_icarray[L74HC595_ICNUMBER];

extern void l74hc595_init();
extern void l74hc595_setregallon();
extern void l74hc595_setregalloff();
extern void l74hc595_setreg(uint16_t regindex, uint8_t val);
extern void l74hc595_setchipbyte(uint8_t chipindex, uint8_t val);
extern void l74hc595_shiftout();

/*
 * set a register index on
 */
#define l74hc595_setregon(regindex) l74hc595_setreg(regindex, 1);
/*
 * set a register index off
 */
#define l74hc595_setregoff(regindex) l74hc595_setreg(regindex, 0);

#endif
