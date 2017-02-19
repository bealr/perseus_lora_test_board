/* 
 * File:   global.h
 * Author: rbeal
 *
 * Created on February 10, 2017, 4:25 PM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

void load_tab(char* toload, char* tab);

struct System_state
{
    char mode; // Rx=0, Tx=1, None=2
    char payload[255];
    int  reapet_delay;
    char channel;
    char rf_mode;
    char src_addr;
    char dest_addr;
    char display_rx;
};

#endif	/* GLOBAL_H */

