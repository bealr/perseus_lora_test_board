/* 
 * File:   SPI.h
 * Author: BEAL Romain
 *
 * Created on 17 janvier 2015, 13:14
 */

#ifndef SPI_H
#define	SPI_H

void spi_init();
void spi_w8b(char byte);
char spi_r8b();

void transmit_SPI(char addr, char data);
char receive_SPI(char addr);

void clear_bits_SPI(char addr, char data);
void set_bits_SPI(char addr, char data);

#endif	/* SPI_H */

