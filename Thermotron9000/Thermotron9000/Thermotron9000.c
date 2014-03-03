/*
 * Author: Patrick R. Clark <Patrick@prclark.net> http://www.prclark.net
 * Thermotron9000.c
 * Created: 1/18/2014 2:36:10 PM
 * Thermostat Firmware for AVR
 * Copyright (C) 2014  Patrick R. Clark
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *   
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd/lcd.h"
#include "mrf24j/lib_mrf24j.h"
#include "l74hc595/l74hc595.h"

ISR(INT0_vect) {
	mrf_interrupt_handler();
}

static inline void mrf_interrupt_disable(void) {
	EIMSK &= ~(_BV(INT0));
}
static inline void mrf_interrupt_enable(void) {
	EIMSK |= _BV(INT0);
}

void handle_rx(mrf_rx_info_t *rxinfo, uint8_t *rx_buffer) {
	mrf_interrupt_disable();
	//printf_P(PSTR("Received a packet: %u bytes long\n"), rxinfo->frame_length);
	//printf("headers:");
	switch (rxinfo->frame_type) {
		case MAC_FRAME_TYPE_BEACON: /*printf("[ft:beacon]");*/ break;
		case MAC_FRAME_TYPE_DATA: /*printf("[ft:data]");*/ break;
		case MAC_FRAME_TYPE_ACK: /*printf("[ft:ack]");*/ break;
		case MAC_FRAME_TYPE_MACCMD: /*printf("[ft:mac command]");*/ break;
		//default: printf("[ft:reserved]");
	}
	if (rxinfo->pan_compression) {
		/*printf("[pan comp]");*/
	}
	if (rxinfo->ack_bit) {
		/*printf("[ack bit]");*/
	}
	if (rxinfo->security_enabled) {
		/*printf("[security]");*/
	}
	switch (rxinfo->dest_addr_mode) {
		case MAC_ADDRESS_MODE_NONE: /*printf("[dam:nopan,noaddr]");*/ break;
		case MAC_ADDRESS_MODE_RESERVED: /*printf("[dam:reserved]");*/ break;
		case MAC_ADDRESS_MODE_16: /*printf("[dam:16bit]");*/break;
		case MAC_ADDRESS_MODE_64: /*printf("[dam:64bit]");*/break;
	}
	switch (rxinfo->frame_version) {
		case MAC_FRAME_VERSION_2003: /*printf("[fv:std2003]");*/break;
		case MAC_FRAME_VERSION_2006: /*printf("[fv:std2006]");*/break;
		default: /*printf("[fv:future]");*/break;
	}
	switch (rxinfo->src_addr_mode) {
		case MAC_ADDRESS_MODE_NONE: /*printf("[sam:nopan,noaddr]");*/ break;
		case MAC_ADDRESS_MODE_RESERVED: /*printf("[sam:reserved]");*/ break;
		case MAC_ADDRESS_MODE_16: /*printf("[sam:16bit]");*/break;
		case MAC_ADDRESS_MODE_64: /*printf("[sam:64bit]");*/break;
	}

	//printf("\nsequence: %d(%x)\n", rxinfo->sequence_number, rxinfo->sequence_number);

	uint8_t i = 0;
	uint16_t dest_pan = 0;
	uint16_t dest_id = 0;
	//uint16_t src_pan = 0;
	uint16_t src_id = 0;
	if (rxinfo->dest_addr_mode == MAC_ADDRESS_MODE_16
	&& rxinfo->src_addr_mode == MAC_ADDRESS_MODE_16
	&& rxinfo->pan_compression) {
		dest_pan = rx_buffer[i++];
		dest_pan |= rx_buffer[i++] << 8;
		dest_id = rx_buffer[i++];
		dest_id |= rx_buffer[i++] << 8;
		src_id = rx_buffer[i++];
		src_id |= rx_buffer[i++] << 8;
		//printf("[dpan:%x]", dest_pan);
		//printf("[d16:%x]", dest_id);
		//printf("[s16:%x]", src_id);
		// TODO Can't move this into the library without doing more decoding in the library...
		//i += 2; // as in the library, module seems to have two useless bytes after headers!
		} else {
		//printf("unimplemented address decoding!\n");
	}

	// this will be whatever is still undecoded :)
	//printf_P(PSTR("Packet data, starting from %u:\n"), i);
	char data[9] = {0};
	int index = 0;
	while((rx_buffer[i] != '¡') && (index < 8))
	{
		data[index++] = rx_buffer[i++];
	}
	data[index] = '\0';
	lcd_clrscr();
	lcd_puts(data);
	
	//printf_P(PSTR("\nLQI/RSSI=%d/%d\n"), rxinfo->lqi, rxinfo->rssi);
	
	mrf_interrupt_enable();
}

void handle_tx(mrf_tx_info_t *txinfo) {
	if (txinfo->tx_ok) {
	}
}

int tts = 0;
int time_to_send()
{
	if(tts++ == 1000)
	{
		tts = 0;
		return 1;
	}
	else return 0;
}

void initSPI()
{
	DDRB |= (1<<PINB3); // as output (DO)
	DDRB |= (1<<PINB5); // as output (USISCK)
	DDRB &= ~(1<<PINB4); // as input (DI)
	DDRB |= (1<<PINB2); // as output (CS)
	DDRB |= (1<<PINB1); // as reset
	SPCR |= (1<<MSTR);               // Set as Master
	//SPCR |= (1<<SPR0)|(1<<SPR1);     // divided clock by 128
	SPCR |= (1<<SPE);                // Enable SPI
}


int main(void)
{	
	wdt_enable(WDTO_2S);
	l74hc595_init();
	backlight_on();
	lcd_init(LCD_DISP_ON_CURSOR);
	lcd_clrscr();
	lcd_puts("Init'd");
	initSPI();
	DDRD |= (1<<PIND1);
		
	//Set the interrupt bit
	EIMSK |= (1<<INT0);
		
	//Initialize the wireless chip
	mrf_reset(&PORTB, PINB1);
	mrf_init(&PORTB, PINB2);
		
	mrf_pan_write(0xcafe);
	mrf_address16_write(0x4202);
	sei();
    
	char buf[20];
	sprintf(buf, "PAN:%X", mrf_pan_read());
	lcd_clrscr();
	lcd_puts(buf);
	while(1)
    {
		wdt_reset();
        mrf_check_flags(&handle_rx, &handle_tx);
        if (time_to_send()) {
	        mrf_send16(0x6000, 3, "Pat");
        }
        _delay_ms(1);
    }
}