/*
 * File:   main.c
 * Author: BEAL Romain
 *
 * Created on 2016, 12:38
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "d_spi.h"

void transmit_SPI(char addr, char data)
{
    LATB2 = 0;              // CS
    
    spi_w8b((addr<<1)&0x7E);
    
    spi_w8b(data);
    
    LATB2 = 1;              // CS
    
    //__delay_ms(5);        // for debugging
}

char receive_SPI(char addr)
{
    char tmp;
    LATB2 = 0;                      // CS
    
    spi_w8b(((addr<<1)&0x7E)|0x80);
    spi_w8b(0x00);
    
    LATB2 = 1;                      // CS
    
    return spi_r8b();                  // slave response
}

void clear_bits_SPI(char addr, char mask)
{
    char tmp;
    
    tmp = receive_SPI(addr);
    transmit_SPI(addr, tmp&(~mask)); // change only masked bit
}

void set_bits_SPI(char addr, char mask)
{
    char tmp;
    
    tmp = receive_SPI(addr);
    transmit_SPI(addr, tmp|mask);   // change only masked bit
}


/**
  * @desc Send SPI byte to buffer
  * @param unsigned char : byte to send
  * @return none
 **/
void spi_w8b(char byte) {
    
    PIR1bits.SSPIF = 0;       // clear interrupt flag
    SSPBUF = byte;            // send byte
    while(!PIR1bits.SSPIF) ;  // Wait until flag set
}

/**
  * @desc Read SPI byte from buffer
  * @param none
  * @return char : buffer content
 **/
char spi_r8b() {

    return SSPBUF; // get byte
}

/**
  * @desc SPI module init
  * @param none
  * @return none
 **/
void spi_init() {
    
    // Set directions
    TRISCbits.TRISC3 = 0;   // CLK
    TRISCbits.TRISC4 = 1;   // SDI
    TRISCbits.TRISC5 = 0;   // SDO
    TRISCbits.TRISC2 = 0;   // CS_lora

    // --------------
    // 0011 = SPI Master mode, clock = TMR2 output/2  [ ]
    // 0010 = SPI Master mode, clock = FOSC/64        [ ]
    // 0001 = SPI Master mode, clock = FOSC/16        [#] (64Mhz)
    // 0000 = SPI Master mode, clock = FOSC/4         [ ]
    // *
    SSPCON1bits.SSPM0 = 0;
    SSPCON1bits.SSPM1 = 1;
    SSPCON1bits.SSPM2 = 0;
    SSPCON1bits.SSPM3 = 0;
    // --------------

    // SPI MODE
    SSPCON1bits.CKP=0; // Idle state for clock is a low level
    SSPSTATbits.CKE=1; // Transmit occurs on transition from active to Idle clock state
    SSPSTATbits.SMP = 0;
    
    SSPCON1bits.SSPEN=1; // start SPI

    SSPIF=0; // clear interrupt flag
    
    LATC2 = 1; // CS_lora
    LATB3 = 1; // CS RC522
}