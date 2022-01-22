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

void writeTxt(byte page, byte y, char * s, int n) {
   int i=0;
   while (*s!='\n' && *s!='\0') 
   {
      putchGLCD(page, y+i, *(s++), n);
      i++;
   };
}

void starting_message()
{
   writeTxt(1, 9, "L7 GLCD", 0);
   writeTxt(3, 3, "Bernat Borras Civil", 0);
   writeTxt(5, 3, "Miquel Torner Vinals", 0);
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
   //setXAddress(&xAddress);
   //setYAddress(&yAddress);
      
   int left, right, up, down, nocturn_timer, timer, block_time;
   left = right = up = down = timer = 1;
   block_time = 10; 					//Quants cicles hi han d'haver entre moviment i moviment
   
   int mode_nocturn;
   mode_nocturn = nocturn_timer = 0;
   
   starting_message();
   
   int act = 1;
   int b = 0;
   
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
	 
	 // Mode nocturn
	 if (PORTCbits.RC0 && !b && !nocturn_timer) {
	    act = 1;
	    b = 1;
	    if (!mode_nocturn) 
	    {
	       setGLCD(0,7,0,127);
	       mode_nocturn = 1;
	    }
	    else 
	    {
	       clearGLCD(0,7,0,127);
	       mode_nocturn = 0;
	    }
	    __delay_ms(5);
	    nocturn_timer = block_time;
	 }
	 else if (nocturn_timer != 0) 
	 {
	    --nocturn_timer;
	 }
	 else if (!PORTCbits.RC0) b = 0;
	    
	 if(!mode_nocturn)
	 {
	    if(timer%10 == 0 && (xValue != xValueAnt || yValue != yValueAnt || act)) 
	    {
	       if(timer%100 == 0) clearGLCD(6,7,110,127);   //Esborrem pantalla
	       writeTxt(6, 20, "x=", 0);
	       writeNum(6, 22, xValue, 0);
	       writeTxt(7, 20, "y=", 0);
	       writeNum(7, 22, yValue, 0);
	    }
	       ClearDot(yValueAnt, xValueAnt);
	       SetDot(yValue, xValue);

	 }
	 else
	 {
	    if(timer%10 == 0 && (xValue != xValueAnt || yValue != yValueAnt || act)) 
	    {
	       if(timer%100 == 0) setGLCD(6,7,110,127);   //Esborrem pantalla
	       writeTxt(6, 20, "x=", 1);
	       writeNum(6, 22, xValue, 1);
	       writeTxt(7, 20, "y=", 1);
	       writeNum(7, 22, yValue, 1);
	    }
	    SetDot(yValueAnt, xValueAnt);
	    ClearDot(yValue, xValue);

	 }
	 ++timer;
   }
}