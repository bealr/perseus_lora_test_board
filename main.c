/* 
 * File:   main.c
 * Author: rbeal
 *
 * Created on December 8, 2016, 11:42 AM
 */

#include <stdio.h>
#include <stdlib.h>


// PIC18F26K80 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config RETEN = OFF      // VREG Sleep Enable bit (Ultra low-power regulator is Disabled (Controlled by REGSLP bit))
#pragma config INTOSCSEL = HIGH // LF-INTOSC Low-power Enable bit (LF-INTOSC in High-power mode during Sleep)
#pragma config SOSCSEL = HIGH   // SOSC Power Selection and mode Configuration bits (High Power SOSC circuit selected)
#pragma config XINST = OFF       // Extended Instruction Set (Disabled)

// CONFIG1H
#pragma config FOSC = INTIO2    // Oscillator (Internal RC oscillator)
#pragma config PLLCFG = ON      // PLL x4 Enable bit (Enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = OFF       // Internal External Oscillator Switch Over Mode (Disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power Up Timer (Disabled)
#pragma config BOREN = SBORDIS  // Brown Out Detect (Enabled in hardware, SBOREN disabled)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (1.8V)
#pragma config BORPWR = LOW     // BORMV Power level (BORMV set to low power level)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WDTPS = 1048576  // Watchdog Postscaler (1:1048576)

// CONFIG3H
#pragma config CANMX = PORTC    // ECAN Mux bit (ECAN TX and RX pins are located on RC6 and RC7, respectively)
#pragma config MSSPMSK = MSK7   // MSSP address masking (7 Bit address masking mode)
#pragma config MCLRE = ON       // Master Clear Enable (MCLR Enabled, RE3 Disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Overflow Reset (Enabled)
#pragma config BBSIZ = BB2K     // Boot Block Size (2K word Boot Block size)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protect 00800-03FFF (Disabled)
#pragma config CP1 = OFF        // Code Protect 04000-07FFF (Disabled)
#pragma config CP2 = OFF        // Code Protect 08000-0BFFF (Disabled)
#pragma config CP3 = OFF        // Code Protect 0C000-0FFFF (Disabled)

// CONFIG5H
#pragma config CPB = OFF        // Code Protect Boot (Disabled)
#pragma config CPD = OFF        // Data EE Read Protect (Disabled)

// CONFIG6L
#pragma config WRT0 = OFF       // Table Write Protect 00800-03FFF (Disabled)
#pragma config WRT1 = OFF       // Table Write Protect 04000-07FFF (Disabled)
#pragma config WRT2 = OFF       // Table Write Protect 08000-0BFFF (Disabled)
#pragma config WRT3 = OFF       // Table Write Protect 0C000-0FFFF (Disabled)

// CONFIG6H
#pragma config WRTC = OFF       // Config. Write Protect (Disabled)
#pragma config WRTB = OFF       // Table Write Protect Boot (Disabled)
#pragma config WRTD = OFF       // Data EE Write Protect (Disabled)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protect 00800-03FFF (Disabled)
#pragma config EBTR1 = OFF      // Table Read Protect 04000-07FFF (Disabled)
#pragma config EBTR2 = OFF      // Table Read Protect 08000-0BFFF (Disabled)
#pragma config EBTR3 = OFF      // Table Read Protect 0C000-0FFFF (Disabled)

// CONFIG7H
#pragma config EBTRB = OFF      // Table Read Protect Boot (Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "d_uart.h"
#include "d_spi.h"
#include "d_lora.h"
#include "ihm.h"
#include "global.h"
#include "RC522.h"



// STATE STRUCT
struct System_state state_struct;

void init();

void interrupt tc_int(void) // High priority interrupt
{
    GIEH = 0;
    
    if (PIR1bits.RCIF)
    {
        uart_recep_char(RCREG);
    }
    
    GIEH = 1;
}
 
void interrupt low_priority LowIsr(void) // Low priority interrupt
{
    GIEL = 0;
    
    if (INTCONbits.TMR0IF) {

        INTCONbits.TMR0IF = 0;
    }
    
    GIEL = 1;
}

/*
 * 
 */
int main() {
    
    char received[255];
    char length;
    int i;
    char last_pacnum=0;
    
    init();
    
    state_struct.mode = 2; // None mode
    *state_struct.payload = '\0';
    state_struct.reapet_delay = 1500;
    state_struct.channel = 1;
    state_struct.rf_mode = 4;
    state_struct.src_addr = LORA_ID;
    state_struct.dest_addr = 0; // broadcast
    state_struct.display_rx = 0;
    
    spi_init();
    uart_init();
    //lora_init(state_struct.mode);
    init_RC522();
    
    int i;
    char status;
    char str[100];
    
    while(1) {
        status = MFRC522_Request(PICC_REQIDL, str); // check if card is present
        if (status == MI_OK)
        {
            uart_send_string("CARTE OK !! \n");
            
            if (MFRC522_ReadCardSerial(str) == MI_OK) // get UID
            {
                uart_send_string("SERIAL OK !! \n");
                uart_send_string("UID = ");     // and display it
                uart_send_hex(*(str+0));
                uart_send_hex(*(str+1));
                uart_send_hex(*(str+2));
                uart_send_hex(*(str+3));
                uart_send_string("\n");
            }
            
            LATA0 = 1;                         // blink LED0
            for (i=0;i<10;i++) __delay_ms(10); // .
            LATA0 = 0;                         // .
            for (i=0;i<10;i++) __delay_ms(10); // .
            LATA0 = 1;                         // .
            for (i=0;i<10;i++) __delay_ms(10); // .
            LATA0 = 0;                         // .
            for (i=0;i<10;i++) __delay_ms(10); // .
            LATA0 = 1;                         // .
            for (i=0;i<10;i++) __delay_ms(10); // .
            LATA0 = 0;                         // .
            for (i=0;i<10;i++) __delay_ms(10); // .
        }
    }
    
    LATA0 = 0;
    LATA1 = 0;
    
    //GIEH = 1;
    //GIEL = 1;
    

    // TIMER TEST
    T0CONbits.PSA = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.T0PS = 0b111;
    T0CONbits.T08BIT = 0;
    T0CONbits.TMR0ON = 0;
    
    INTCON2bits.TMR0IP = 0;
    INTCONbits.TMR0IE = 0;
    
    __delay_ms(500);
    
    while (1) {
        
        if (state_struct.mode == 2) {
            
            Nop();
        }
        
        if (state_struct.mode == 1) {
            
            lora_sendPacket(state_struct.dest_addr, state_struct.payload); // 0 -> broadcast
            for (i=0;i<state_struct.reapet_delay;i++)
                __delay_ms(1); // pas super propre... TODO
        }
        
        if (state_struct.mode == 0) {
        
            for (i=0;i<30;i++)
                *(received+i) = '\0';
            if (lora_receivePaquet_tmo1s(&length, received) == 0) {
            
                if (state_struct.display_rx) {

                    ihm_clear();
                    uart_send_string("Rx display mode -- 'm' to return\n");

                    uart_send_string("Dest addr: ");
                    uart_send_hex(*(received + 0));
                    uart_send_string("\n");

                    uart_send_string("Src addr: ");
                    uart_send_hex(*(received + 1));
                    uart_send_string("\n");

                    uart_send_string("Pacnum: ");
                    uart_send_hex(*(received + 2));
                    uart_send_string("\n");

                    uart_send_string("Length: ");
                    uart_send_hex(*(received + 3));
                    uart_send_string("\n");

                    uart_send_string("Payload: ");
                    uart_send_string_length((received + 4), length);
                    uart_send_string("\n");

                    uart_send_string("RSSI level: ");
                    uart_send_char_int(lora_getRSSI());
                    uart_send_string("\n\n");
                }
            }
        }
    }
    
    return (EXIT_SUCCESS);
}

void init() {
    // Clock : 16MHz *4 = 64MHz
    OSCCONbits.IRCF = 0b111;
    OSCTUNEbits.PLLEN = 1; // PLL enabled
    OSCCONbits.SCS = 0b00; // primary clock
    
    RCONbits.IPEN = 1; // Enable levels interrupt
    
    
    TRISAbits.TRISA0 = 0; // Led Tx LORA
    TRISAbits.TRISA1 = 0; // Led Rx LORA
    TRISBbits.TRISB1 = 0; // reset RC522
    TRISBbits.TRISB2 = 0; // CS RC522
    
    LATA1 = 0;
    LATA0 = 0;
    
    ANCON0 = 0;
    ANCON1 = 0;
}


void load_tab(char* toload, char* tab) {
    char i;
    
    for (i=0;*(tab+i) != '\0';i++)
        *(tab+i) = *(toload+i);
}