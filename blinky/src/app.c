/*
 * File:   main_PWB_PIC.c
 * Author: Jakub
 *
 * Created on 21. Cervence 2023, 11:33
 */

#define __NEED_uint8_t      //to be able to use uint8_t
#define __NEED_uint16_t     //to be able to use uint16_t

#include <pic16f15214.h>
#include "app.h"


//INs
#define DRV_EN          PORTAbits.RA1
#define BT              PORTAbits.RA3

//OUTs
#define LED_on          LATAbits.LATA0 = 1
#define LED_off         LATAbits.LATA0 = 0

#define DRV_on          LATAbits.LATA1 = 1
#define DRV_off         LATAbits.LATA1 = 0






void main(void) {

    uint8_t tmp8_1;  

    //OSC
    OSCFRQ = 0; //1MHz HFINTOSC, can't change osc source, only freq
            
    //SYSTEM
    // CPCON = 0;  //charge pump for analog periph. - off for now

    // WDTCON = 0b00010101;    //LFINTOSC, 1s
    
    //PORTS
    // ANSELA = 0b00000000;    //RA5:ConsIncMOS, RA4: CCMOS, RA3: BT(RST), RA2: NC, RA1: DRV(PGC), RA0: LED(PGD)
    // SLRCONA = 0b00111101;   //only DRV is on high SR
    TRISA = 0b11111100;     //outputs except BT and NC pin
    WPUA =  0b00001000;      //pull-up only for BT
    
    CLRWDT();
    __delay_ms(450);
    CLRWDT();
    
    LATA = 0x00;
    PORTA = 0x00;

    for (int i= 0; i < 5; i++) {
        LED_on;
        __delay_ms(50);
        LED_off;
        __delay_ms(50);
    }

    while(1)
    {
        CLRWDT();
        __delay_ms(5);

        tmp8_1 = BT;

        if(tmp8_1 == 0) {
            LED_on;
        }
        else {
            LED_off;
        }
    }
    
    RESET();
    return;
}


