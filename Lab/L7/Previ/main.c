/* 
 * Created:   2021
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#include <string.h>
#include "config.h"
#include "GLCD.h"
#define _XTAL_FREQ 8000000  

void writeTxt(byte page, byte y, char * s) {
   int i=0;
   while (*s!='\n' && *s!='\0') 
   {
      putchGLCD(page, y+i, *(s++));
      i++;
   };
}

void starting_message()
{
   writeTxt(1, 9, "L7 GLCD");
   writeTxt(3, 3, "Bernat Borras Civil");
   writeTxt(5, 3, "Miquel Torner Vinals");
   __delay_ms(2000);
   clearGLCD(0,7,0,127);   //Esborrem pantalla
}

void main(void)
{
   ANSELA=0x00; 
   ANSELB=0x00;                  
   ANSELC=0x00;                  
   ANSELD=0x00;                  
   
   TRISD=0x00;		   
   TRISB=0x00;
   TRISA=0xFF;
    
   PORTD=0x00;
   PORTB=0x00; 
   PORTA=0x00;
   
   GLCDinit();		   //Inicialitzem la pantalla
   clearGLCD(0,7,0,127);   //Esborrem pantalla
   setStartLine(0);        //Definim linia d'inici
   
   int xAddress, yAddress, xValue, yValue, xValueAnt, yValueAnt;
   xAddress = yAddress = xValue = yValue = 0;
   
   // No s'utilitzen les adreçes (de moment)
   setXAddress(&xAddress);
   setYAddress(&yAddress);
      
   int left, right, up, down, timer, block_time;
   left = right = up = down = timer = 1;
   block_time = 50; 					//Quants cicles hi han d'haver entre moviment i moviment
   
   starting_message();
   
   SetDot(yValue, xValue);
   
   while (1)
   {   
	 xValueAnt = xValue;
	 yValueAnt = yValue;

	  //Right control
     	if (PORTAbits.RA0 && !PORTAbits.RA1 && !right) {
	    if (xValue < 127) ++xValue;
	    right = block_time;
	    __delay_ms(1);
	 }
	 else if (right != 0) --right;
	    
	  //Left control
     	 if (PORTAbits.RA1 && !PORTAbits.RA0 && !left) {
	    if (xValue > 0) --xValue;
	    left = block_time;
	    __delay_ms(1);
	 }
	 else if (left != 0) --left;
	    
	 // Up control
      	 if (PORTAbits.RA2 && !PORTAbits.RA3 && !up) {
	    if (yValue > 0) --yValue;
	    up = block_time;
	    __delay_ms(1);
	 }
	 else if (up != 0) --up;
	    
	 // Down control
      	 if (PORTAbits.RA3 && !PORTAbits.RA2 && !down) {
	    if (yValue < 63) ++yValue;
	    down = block_time;
	    __delay_ms(1);
	 }
	 else if (down != 0) --down;
	 
	 if(timer%10 == 0) 
	 {
	    if(timer%100 == 0) clearGLCD(6,7,110,127);   //Esborrem pantalla
	    writeTxt(6, 20, "x=");
	    writeNum(6, 22, xValue);
	    writeTxt(7, 20, "y=");
	    writeNum(7, 22, yValue);
	 }
	 
	 ClearDot(yValueAnt, xValueAnt);
	 SetDot(yValue, xValue);
	 ++timer;
   }
}
