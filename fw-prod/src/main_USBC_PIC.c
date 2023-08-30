/*
 * File:   main_PWB_PIC.c
 * Author: Jakub
 *
 * Created on 21. Cervence 2023, 11:33
 */

#define __NEED_uint8_t      //to be able to use uint8_t
#define __NEED_uint16_t     //to be able to use uint16_t

//#include <pic16f15214.h>
#include "main_USBC_PIC.h"


//INs
#define DRV_EN          PORTAbits.RA1
#define BT              PORTAbits.RA3

//OUTs
#define LED_on          LATAbits.LATA0 = 1
#define LED_off         LATAbits.LATA0 = 0

#define DRV_on          LATAbits.LATA1 = 1
#define DRV_off         LATAbits.LATA1 = 0

#define CCMOS_on        LATAbits.LATA4 = 1
#define CCMOS_off       LATAbits.LATA4 = 0

#define ConsIncMOS_on   LATAbits.LATA5 = 1
#define ConsIncMOS_off  LATAbits.LATA5 = 0

#define press_change    10      //n*10ms
#define press_off       90     //n*10ms

//#define Curr_100        172     //full power 135mA SINGLE PLED    1.275*(200-current)
//#define Curr_100        191     //full power 150mA SINGLE PLED
#define Curr_100        205     //full power 160mA SINGLE PLED def.
//#define Curr_100        231     //full power 180mA SINGLE PLED
//#define Curr_100        255     //full power 200mA SINGLE PLED

//#define Curr_redu       118     //80mA
#define Curr_redu       65     //50mA

                                //MODES: 0 = strobe, 1 = full, 2 = redu
#define WaUpMode        1       //set wake-up mode (1->full, 2->redu, 4->strobe)
#define MinMode         1       //lowest mode
#define MaxMode         4       //highest mode


union
{
    uint8_t byte;
    struct {
        unsigned PLED_FULL:1;
        unsigned PLED_REDU:1;
        unsigned PLED_STR:1;
        unsigned :1;
        unsigned :1;
        unsigned :1;
        unsigned :1;
        unsigned :1;
    };
} OPstate;



void main(void) {
    uint8_t tmp8_1;  
        
    OPstate.byte = WaUpMode;
    
    //OSC
    OSCFRQ = 0; //1MHz HFINTOSC, can't change osc source, only freq
    OSCENbits.MFOEN = 1;    //enable MFOSC for PWM
            
    //SYSTEM
    CPCON = 0;  //charge pump for analog periph. - off for now
//    WDTCON = 0b00010111;    //LFINTOSC, 2s
    WDTCON = 0b00010101;    //LFINTOSC, 1s
//    WDTCON = 0b00010011;    //LFINTOSC, 0.5s
    
    //PORTS
    LATA = 0;
    ANSELA = 0b00000100;    //RA5:ConsIncMOS, RA4: CCMOS, RA3: BT(RST), RA2: NC, RA1: DRV(PGC), RA0: LED(PGD)
    WPUA = 0b00001000;      //pull-up only for BT
    SLRCONA = 0b00111101;   //only DRV is on high SR
    //INLVLA = ;            //input ST/TTL - default ST
    TRISA = 0b00001100;     //outputs except BT and NC pin
    
    RA0PPS = 0;
    RA1PPS = 0;
    RA2PPS = 0;
    RA4PPS = 0;
    RA5PPS = 0;
    
    CLRWDT();
    __delay_ms(450);
    CLRWDT();
    __delay_ms(450);
    CLRWDT();
    
    tmp8_1 = 255;
    CCMOS_on;
    LED_on;
    
    while(1)
    {
//        if((DRV_EN != 0)||(OPstate.PLED_ON == 0)) //rearm only when EN pin = 1 or OFF
//        {
            CLRWDT();   //reset WDT
//        }
        
        if(tmp8_1 > press_change)
        {
            if(OPstate.PLED_STR == 1)   //TIMER - PWM for strobe
            {
                ConsIncMOS_off;
                RA0PPS = 0;             //LATA as output source (LED_on)
                RA1PPS = 0;             //LATA as output source
                RA5PPS = 0;             //LATA as output source
                T2CONbits.ON = 0;
                
                __delay_ms(100);
                
                PWM4CONbits.EN = 1;     //PWM4 with inv. polarity for increased consumption
                PWM4CONbits.POL = 1;
                PWM4DCH = 56;           
                
                CCPR2H = 62;            //approx 10 ms for discharge (2U unit)
                CCP2CON = 0b10011100;    //enabled, left-aligned PWM
                        
                T2TMR = 0;
                T2PR = 162;             //3Hz
                T2CON = 0b01100000;     //1:64 prescale
                T2HLT = 0;              //no sync, free-running + SW gate
                T2CLKCON = 0b00000110;  //MFINTOSC 31.25kHz
                
                PWM3CONbits.EN = 1;
                PWM3DCH = 57;           //35%
                
                RA0PPS = 2;             //CCP2 PWM as output source
                RA1PPS = 3;             //PWM3 as output source
                RA5PPS = 4;             //PWM4 as output source
                
                T2CONbits.ON = 1;
                
//                PORTB = 0b00001001;    //NMOSFET/LED ON and driver OFF
//                my_delay_10ms(25);
//                
//                GTCCR = 0b00000001;     //reset prescaler
//                ICR0 = 1302;            //period register 3Hz/35%
//                TCNT0 = 0;              //timer count
//                OCR0A = 490;           //output compare reg. B (MOSFET) to discharge coil
//                OCR0B = 456;           //output compare reg. B (driver)
//                TCCR0A = 0b00100010;    //clear on compare match for ch. B, fast PWM 14 (1110)
//                TCCR0B = 0b00011100;    //timer config reg. B, 1:256 => 3906.25Hz
            }
            else
            {
                RA0PPS = 0;             //LATA as output source (LED_on)
                RA1PPS = 0;             //LATA as output source
                RA5PPS = 0;             //LATA as output source
                T2CONbits.ON = 0;
                CCP2CONbits.EN = 0;
                PWM4CONbits.EN = 0;
                
                __delay_ms(100);
                
                T2TMR = 0;
                T2PR = 255;             //490Hz
                T2CON = 0b00010000;     //1:2 prescale
                T2HLT = 0;              //no sync, free-running + SW gate
                T2CLKCON = 0b00000001;  //Fosc/4 = inst. rate (required for PWM)
                
                PWM3CONbits.EN = 1;
                if(OPstate.PLED_FULL == 1)
                {
                    PWM3DCH = Curr_100;
                    ConsIncMOS_off;
                }
                else if(OPstate.PLED_REDU == 1)
                {
                    PWM3DCH = Curr_redu;
                    ConsIncMOS_on;
                }
                
                RA1PPS = 3;             //PWM3 as output source
                
                T2CONbits.ON = 1;
                
//TIMER - PWM for dimming
//                PORTB = 0b00001001;    //NMOSFET/LED and driver OFF
//                my_delay_10ms(5);
//                my_delay_10ms(1);
//                
//                GTCCR = 0b00000001;     //reset prescaler
//                ICR0 = 255;              //period register
//                TCNT0 = 0;              //timer count
//                TCCR0A = 0b00110010;    //set on compare match for ch. B, fast PWM 14 (1110)
//                TCCR0B = 0b00011010;    //timer config reg. B, 1:8 => 125kHz
            }
            
            OPstate.byte <<= 1;
            if(OPstate.byte > MaxMode){OPstate.byte = MinMode;}
            while(BT == 0){CLRWDT();}
        }
        
            
        
        tmp8_1 = 0;
        while(BT == 0)
        {
            tmp8_1++;
            __delay_ms(10);
            if(tmp8_1 > press_off)
            {
                LED_off;
                ConsIncMOS_off;
                RA0PPS = 0;             //LATA as output source
                RA1PPS = 0;             //LATA as output source
                RA5PPS = 0;             //LATA as output source
                T2CONbits.ON = 0;

                CCMOS_off;
                while(BT == 0){CLRWDT();}  //endless
            }
            CLRWDT();
        }
        
        
    }
    
    RESET();
    return;
}


