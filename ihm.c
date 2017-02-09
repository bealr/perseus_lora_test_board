/* 
 * File:   ihm.c
 * Author: rbeal
 *
 * Created on F 8, 201, 10:50 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "d_uart.h"
#include <xc.h>
#include "d_lora.h"
#include "ihm.h"

char choice=0;
char i;

struct System_state
{
    char mode; // Rx=0, Tx=1, None=2
    char payload[100];
    int reapet_delay;
    char channel;
    char rf_mode;
    char src_addr;
    char dest_addr;
    char display_rx;
};

extern struct System_state state_struct;

void ihm_clear() {
    
    uart_send_char(0x0C);
}

void ihm_display_header() {
    
    uart_send_string("############ IHM LoRa ############\n");
}

void ihm_display_menu(char menu) {
    
    switch (menu) {

        case 0:
            ihm_display_header();
            uart_send_string(">> MAIN MENU\n");
            uart_send_string("------- INFO ------\n");
            uart_send_string("1 -> Display info\n");
            uart_send_string("------ CHANGE------\n");
            uart_send_string("2 -> Mode (Tx or Rx)\n");
            uart_send_string("3 -> Canal\n");
            uart_send_string("4 -> RF Mode\n");
            
            uart_send_string("5 -> Payload\n");
            uart_send_string("6 -> Src ID\n");
            uart_send_string("7 -> Dest ID\n");
            uart_send_string("8 -> Delay repeat\n");
            uart_send_string("9 -> Display Rx\n");
            uart_send_string("\n");
            uart_send_string("Choice : ");
            break;

        case 1:
            ihm_display_header();
            uart_send_string(">> DISPLAY INFO\n");
            
            uart_send_string("Mode : ");
            if (state_struct.mode == LORA_MODE_TX)
                uart_send_string("Tx\n");
            if (state_struct.mode == LORA_MODE_RX)
                uart_send_string("Rx\n");
            if (state_struct.mode == LORA_MODE_NONE)
                uart_send_string("None\n");
            
            uart_send_string("RF Mode : ");
            uart_send_char_int(state_struct.rf_mode);
            uart_send_string("\n");
            
            uart_send_string("Channel : ");
            uart_send_char_int(state_struct.channel);
            uart_send_string("\n");
            
            uart_send_string("Src addr : ");
            uart_send_char_int(state_struct.src_addr);
            uart_send_string("\n");
            
            uart_send_string("Dest addr : ");
            uart_send_char_int(state_struct.dest_addr);
            uart_send_string("\n");
            
            uart_send_string("Playload : ");
            uart_send_string(state_struct.payload);
            uart_send_string("\n");
            
            uart_send_string("Repeat delay : ");
            uart_send_char_int(state_struct.reapet_delay);
            uart_send_string("\n");
            
            break;
            
        case 2:
            ihm_display_header();
            uart_send_string(">> MODE\n");
            uart_send_string("0. Rx   Mode\n");
            uart_send_string("1. Tx   Mode\n");
            uart_send_string("2. None Mode\n");
            
            uart_send_string("Mode : ");
            break;
            
        case 3:
            uart_send_string("AVAILABLE :\n");
            uart_send_string("1. channel 10, 865.20MHz\n");
            uart_send_string("2. channel 11, 865.50MHz\n");
            uart_send_string("3. channel 12, 865.80MHz\n");
            uart_send_string("4. channel 13, 866.10MHz\n");
            uart_send_string("5. channel 14, 866.40MHz\n");
            uart_send_string("6. channel 15, 866.70MHz\n");
            uart_send_string("7. channel 16, 867.00MHz\n");
            uart_send_string("8. channel 17, 868.00MHz\n");
            uart_send_string("9. channel 00, 903.08MHz\n");
            uart_send_string("10. channel 01, 905.20MHz\n");
            uart_send_string("11. channel 02, 907.40MHz\n");
            uart_send_string("12. channel 03, 909.56MHz\n");
            uart_send_string("13. channel 04, 911.72MHz\n");
            uart_send_string("14. channel 05, 913.88MHz\n");
            uart_send_string("15. channel 06, 916.04MHz\n");
            uart_send_string("16. channel 07, 918.20MHz\n");
            uart_send_string("17. channel 08, 920.36MHz\n");
            uart_send_string("18. channel 09, 922.52MHz\n");
            uart_send_string("19. channel 10, 924.68MHz\n");
            uart_send_string("20. channel 11, 926.84MHz\n");
            uart_send_string("21. default,    915.00MHz\n");
            
            uart_send_string("Channel : ");
            break;
            
        case 5:
            ihm_display_header();
            uart_send_string(">> PLAYLOAD ENTER\n");
            uart_send_string("Text : ");
            break;
    }
}

void ihm_recep_car(char car) {

    char inib_display=0;
    state_struct.display_rx = 0;
    
    if (choice == 2) {
        ihm_writeMode(car);
        inib_display = 1;
    }
    
    if (choice == 3) {
        ihm_writeFreq(car);
        inib_display = 1;
    }
    
    if (choice == 5) {
        ihm_writePayload(car);
        inib_display = 1;
    }
    
    if (car == 'm')
        choice = 0;
    
    if (choice == 0 && car == '1')
        choice = 1;
    
    if (choice == 0 && car == '2')
        choice = 2;
    
    if (choice == 0 && car == '3') {
        i = 0;
        choice = 3;
    }
    
    if (choice == 0 && car == '5') {
        i = 0;
        choice = 5;
    }
    
    if (choice == 0 && car == '9') {
        state_struct.display_rx = 1;
        ihm_clear();
        uart_send_string("Rx display mode -- 'm' to return");
        choice = 9;
        inib_display = 1;
    }
    
    if (!inib_display) {
        ihm_clear();
        ihm_display_menu(choice);
    }
}

void ihm_writePayload(char c) {
    
    uart_send_char(c);
    
    if (c != '\r') {
        *(state_struct.payload+i) = c;
        *(state_struct.payload+i+1) = '\0';
        i++;
    }
    else {
        choice = 0;
        ihm_clear();
        ihm_display_menu(choice);
    }
}

void ihm_writeMode(char car) {
    
    if (car != '0' && car != '1' && car != '2' && car != '\r')
        return;
    
    uart_send_char(car);
    
    if (car != '\r') {
        state_struct.mode = car - '0';
    }
    else {
        choice = 0;
        ihm_clear();
        ihm_display_menu(choice);
    }
}

void ihm_writeFreq(char car) {
    if (car < '0' && car > '9' && car != '\r')
    return;
    
    uart_send_char(car);
    
    if (car != '\r') {
        
        if (i == 1) {
            state_struct.channel = state_struct.channel*10 + (car - '0');
        }
        if (i == 0) {
            state_struct.channel = car - '0';
            i++;
        }
    }
    else {
        if (state_struct.channel < 1)
            state_struct.channel = 1;
        
        if (state_struct.channel > 21)
            state_struct.channel = 21;
        
        
        lora_setChannel(state_struct.channel);
        
        choice = 0;
        ihm_clear();
        ihm_display_menu(choice);
    }
}