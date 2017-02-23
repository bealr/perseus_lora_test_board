/* 
 * File:   RC522.c
 * Author: rbeal
 *
 * Created on July 17, 2016, 11:08 AM
 */

#include <xc.h>
#include "RC522.h"
#include "d_spi.h"

void init_RC522() 
{
    int i;
    
    MFRC522_Reset();
    
    transmit_SPI(TModeReg, 0x8D);      // Timer auto + TimerH = 13
    transmit_SPI(TPrescalerReg, 0x3E); // TimerL = 62
    transmit_SPI(TReloadRegH, 30);     // Reload value H
    transmit_SPI(TReloadRegL, 0);      // Reload value L
    transmit_SPI(TxAutoReg, 0x40);     // 100% ASK
    transmit_SPI(ModeReg, 0x3D);       // TxWaitRF = 1 / Polarity active HIGH / CRC = 6363h
    antenna(ANTENNA_ON);
    __delay_us(20);
        
    if (receive_SPI(VersionReg) == 0x91) // Check MFRC522 version, visual test: MFRC522 OK
    {
        LATA0 = 1;                         // Blink LED0
        for (i=0;i<10;i++) __delay_ms(70); // .
        LATA0 = 0;                         // .
        for (i=0;i<10;i++) __delay_ms(30); // .
        LATA0 = 1;                         // .
        for (i=0;i<10;i++) __delay_ms(5);  // .
        LATA0 = 0;                         // .
        for (i=0;i<10;i++) __delay_ms(5);  // .
        LATA0 = 1;                         // .
        for (i=0;i<10;i++) __delay_ms(5);  // .
        LATA0 = 0;                         // .
        for (i=0;i<10;i++) __delay_ms(5);  // .
    }
    
}

void antenna(char STATE) {
    
    char tmp;
    
    if (STATE == ANTENNA_ON)
    {
        tmp = receive_SPI(TxControlReg);
        if (!(tmp&0x03)) transmit_SPI(TxControlReg, tmp | 0x03); // enable output signal on antenna and not changes others bits
    }
    
    if (STATE == ANTENNA_OFF)
    {
        tmp = receive_SPI(TxControlReg);
        if (tmp&0x03) transmit_SPI(TxControlReg, tmp & 0xFC);   // disable output signal on antenna and not changes others bits
    }
}

char MFRC522_Request(char reqMode, char *TagType)
{
    char status;
    unsigned int backBits;
    
    transmit_SPI(BitFramingReg, 0x07); // TxLastBists = BitFramingReg[2..0]
    *(TagType) = reqMode;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
    if (status != MI_OK || (backBits != 0x10))
        status = MI_ERR;
    
    return status;
}

char MFRC522_ToCard(char command, char *sendData, char sendLen, char *backData, unsigned int *backLen)
{
    char status = MI_TIMEOUT;
    char irqEn, waitIrq;
    char lastBits;
    char n;
    unsigned int i;
    
    switch (command)
    {
        case PCD_AUTHENT:
            irqEn   = 0x12; // interrupt : allow IdleIRq / allow ErrIRq
            waitIrq = 0x10; // interrupt : IdleIRq
            break;
        case PCD_TRANSCEIVE:
            irqEn   = 0x77; // interrupt : allow TxIRq / allow RxIRq / allow IdleIRq / allow LoAlertIRq / allow ErrIRq / allow TimerIRq
            waitIrq = 0x30; // interrupt : RXIrq / IdleIRq
            break;
            
        default:
            irqEn   = 0x00;
            waitIrq = 0x00;
            break;
    }
    
    transmit_SPI(CommIEnReg, irqEn | 0x80); // write interrupt + IRQ signal inverted
    clear_bits_SPI(CommIrqReg, 0x80);       // clear all interrupts in CommIrq register
    set_bits_SPI(FIFOLevelReg, 0x80);       // Flush buffer
    transmit_SPI(CommandReg, PCD_IDLE);     // Cancel all commands
    
    for (i=0;i<sendLen;i++) transmit_SPI(FIFODataReg, *(sendData+i));
    
    transmit_SPI(CommandReg, command); // send command requested
    if (command == PCD_TRANSCEIVE) set_bits_SPI(BitFramingReg, 0x80); // Transmit data now
    
    i = 2000; // adjust timeout, min 25mS
    do {
        n = receive_SPI(CommIrqReg); // read interrupt register
        i--;
    } while ((i!=0) && !(n&0x01) && !(n&waitIrq));
    
    clear_bits_SPI(BitFramingReg, 0x80); // stop send data
    
    if (i != 0) { // if timeout not reached
        if(!(receive_SPI(ErrorReg) & 0x1B))	// if not fatal error
        {
            status = MI_OK;
            if (n & irqEn & 0x01) status = MI_NOTAGERR; // no card present in antenna area
            if (command == PCD_TRANSCEIVE)
            {
               	n = receive_SPI(FIFOLevelReg); // get RX buffer size
              	lastBits = receive_SPI(ControlReg) & 0x07; // ?
                
                if (lastBits)
                    *backLen = (n-1)*8 + lastBits;
                else
                    *backLen = n*8;
                
                if (!n)
                    n = 1;
                if (n > MAX_LEN)
                    n = MAX_LEN;
                
                for (i=0; i<n; i++)
                    backData[i] = receive_SPI(FIFODataReg); // read FIFO RX
            }
        }
        else
            status = MI_ERR;
    }
    return status;	
}

void MFRC522_Reset()
{
    int i;
    
    // hard reset
    LATB1 = 0;
    __delay_ms(1);
    LATB1 = 1;
    for (i=0;i<10;i++) __delay_ms(10);
    
    // soft reset
    transmit_SPI(CommandReg, PCD_RESETPHASE);
    for (i=0;i<10;i++) __delay_ms(10);
}

char MFRC522_AntiColl(char *UID)
{
    char status;
    char i;
    char bcc = 0;
    unsigned unLen;
    transmit_SPI(BitFramingReg, 0x00); // do not send data now
    UID[0] = PICC_ANTICOLL; // prevent conflict
    UID[1] = 0x20; // receiver On
    clear_bits_SPI(Status2Reg, 0x08); // MIFARE Crypto1 : On
    status = MFRC522_ToCard(PCD_TRANSCEIVE, UID, 2, UID, &unLen); // Transmit
    if (status == MI_OK)
    {
        for (i=0;i<4;i++) // UID = 4 bytes
        {
            //  "Block Check Character, it is calculated as exclusive-or over the 4 previous bytes."
            bcc ^= UID[i];
        }

        if (bcc != UID[4])
        {
            status = MI_ERR;
        }
    }
    return status;
}

char MFRC522_ReadCardSerial(char *str)
{
    char status; 
    status = MFRC522_AntiColl(str);
    str[5] = '\0';
    
    return status;
}
