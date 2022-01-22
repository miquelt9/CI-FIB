/* Main.c file generated by New Project wizard
 *
 * Created:   Fri Oct 15 2021
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#include "config.h"
#define _XTAL_FREQ 8388608

void main(void)
 {
   // Write your code here
    ANSELA = 0;
    ANSELB = 0;
    ANSELD = 0;
    TRISA = 0;
    TRISB = 0xFF;
    TRISD = 0;
    
    int num[10];
    num[0] = 0b00111111;
    num[1] = 0b00000110;
    num[2] = 0b01011011;
    num[3] = 0b01001111;
    num[4] = 0b01100110;
    num[5] = 0b01101101;
    num[6] = 0b01111101;
    num[7] = 0b00000111;
    num[8] = 0b01111111;
    num[9] = 0b01101111;
    
    PORTA = 0x0F;
    PORTD = num[0];
    
    int b0 = 0;
    int b1 = 0;
    int b2 = 0;
    int b3 = 0;
    int b4 = 0;
    
    int n0 = 0;
    int n1 = 0;
    int n2 = 0;
    int n3 = 0;
    
   while (1) {
	 if (PORTBbits.RB0 && !b0) {
	    ++n0;
	    if (n0 == 10) n0 = 0;
	    b0 = 1;
	    __delay_ms(10);
	 }
	 else if (!PORTBbits.RB0) b0 = 0;
	 
	 if (PORTBbits.RB1 && !b1) {
	    ++n1;
	    if (n1 == 10) n1 = 0;
	    b1 = 1;
	    __delay_ms(10);
	 }
	 else if (!PORTBbits.RB1) b1 = 0;
	    
	 if (PORTBbits.RB2 && !b2) {
	    ++n2;
	    if (n2 == 10) n2 = 0;
	    b2 = 1;
	    __delay_ms(10);
	 }
	 else if (!PORTBbits.RB2) b2 = 0;
	    
	 if (PORTBbits.RB3 && !b3) {
	    ++n3;
	    if (n3 == 10) n3 = 0;
	    b3 = 1;
	    __delay_ms(10);
	 }
	 else if (!PORTBbits.RB3) b3 = 0;
	 
	 if (!PORTBbits.RB4 && b4) {
	    n0 = 2*n0;
	    n1 = 2*n1;
	    n2 = 2*n2;
	    n3 = 2*n3;
	    if (n0 >= 10) {
	       n0 = n0 - 10;
	       ++n1;
	       }
	    if (n1 >= 10) {
	       n1 = n1 - 10;
	       ++n2;
	       }
	    if (n2 >= 10) {
	       n2 = n2 - 10;
	       ++n3;
	       }
	    if (n3 >= 10) n3 = n3 - 10;
	    b4 = 0;
	    __delay_ms(10);
	 }
	 else if (PORTBbits.RB4) b4 = 1;
	 
	 PORTA = 1;
	 PORTD = num[n0];
	 __delay_ms(5);
	 PORTA = 2;
	 PORTD = num[n1];
	 __delay_ms(5);
	 PORTA = 4;
	 PORTD = num[n2];
	 __delay_ms(5);
	 PORTA = 8;
	 PORTD = num[n3];
	 __delay_ms(5);
   }
 }