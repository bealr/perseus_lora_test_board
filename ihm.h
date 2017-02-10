/* 
 * File:   ihm.h
 * Author: rbeal
 *
 * Created on February 8, 2017, 10:50 AM
 */

#ifndef IHM_H
#define	IHM_H

void ihm_clear();
void ihm_recep_car(char car);

void ihm_writeMode(char car);
void ihm_writePayload(char car);
void ihm_writeFreq(char car);
void ihm_writeRepeatDelay(char car);
void ihm_writeRFMode(char car);

void ihm_writeSrcAddr(char car);
void ihm_writeDestAddr(char car);

#endif	/* IHM_H */

