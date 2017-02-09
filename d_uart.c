/* 
 * File:   main.c
 * Author: rbeal
 *
 * Created on December 8, 2016, 11:42 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <pic18f26k80.h>
#include "d_uart.h"
#include "ihm.h"
#include <xc.h>

/**
  * @desc Init UART module
  * @param none
  * @return none
 **/
void uart_init() {
    
    // set directions
    TRISCbits.TRISC7 = 1; // Rx :  input
    TRISCbits.TRISC6 = 0; // Tx : output
    
    // Tx conf
    TXSTA1bits.TX9 = 0;  // 8b transmission
    TXSTA1bits.TXEN = 1; // Enable Tx
    TXSTA1bits.SYNC = 0; // No sync
    TXSTA1bits.BRGH = 1; // Baud rate high speed
    
    // Rx conf
    RCSTA1bits.RX9 = 0;  // 8b reception
    RCSTA1bits.CREN = 1; // Enable Rx
    
    // UART global
    RCSTA1bits.SPEN = 1; // Enable UART
    SPBRGH1 = 0; // Baud rate value
    SPBRG1 = 34; // 115200
    
    PIR1bits.TX1IF = 0; // clear Tx flag
    RC1IE = 1;          // Enable Rx interrupt on receive
    RC1IP = 1;          // Set interrupt to High priority
}

/**
  * @desc Send char on UART
  * @param char : char to send
  * @return none
 **/
void uart_send_char(char data) {
    
    while (!PIR1bits.TX1IF) ; // While last byte not sent ...
    TXREG1 = data;            // Load buffer with data
}

/**
  * @desc Send string on UART
  * @info Designed for static strings
  * @param const char* : pointer to the first byte ('\0' = last byte)
  * @return none
 **/
void uart_send_string(const char *data) {
    
    int i;
    for (i=0;*(data+i) != '\0';i++) // while char is not '\0'
        uart_send_char(*(data+i));  // Call send char fct
}

/**
  * @desc Send char on UART with length control
  * @param char* : pointer to first char
  * @param char  : string length
  * @return none
 **/
void uart_send_string_length(char *data, char length) {
    
    int i;
    for (i=0;i<length;i++)          // While length is not reached
        uart_send_char(*(data+i));  // Call send char fct
}

void uart_recep_char(char data) {
    ihm_recep_car(data);
}

/**
  * @desc Convert char to hex ascii format and send it
  * @param char : byte to send
  * @return none
 **/
void uart_send_hex(char hex) {
    
    char tmp[5];
    sprintf(tmp, "0x%02x\0", hex); // Convert char to ascii hex format
    uart_send_string(tmp);         // Send it
}

/**
  * @desc Display sligned int format
  * @param signed int : signed to send
  * @return none
 **/
void uart_send_char_int(signed int val) {
    
    char tmp[5];
    sprintf(tmp, "%d", val);  // Convert
    uart_send_string(tmp);    // Send it
}