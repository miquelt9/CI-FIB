/* 
 * Created:   2021
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#include <string.h>
#include "config.h"
#include "GLCD.h"
#define _XTAL_FREQ 8388608

int decimes = 0;
int segons = 0;
int minuts = 0;
int hores = 0;
char run_char = 45; // 45 = '-'; 92 = '\';  124 = '|'; 47 = '/'

void interrupt high_RSI(void)
{
   if (TMR0IF)
   {
      if (run_char < 50) {
	 if (run_char == 45) run_char = 92; 
	 else run_char = 45;
      }
      else {
	 if (run_char == 92) run_char = 124;
	 else run_char = 47;
      }
      
      ++decimes;
      if(decimes > 9){
	 decimes = 0;
	 ++segons;
	 if(segons > 59){
	    segons = 0;
	    ++minuts;
	    if(minuts > 59){
	       minuts = 0;
	       ++hores;
	    }
	 }
      }
      writeNum(4, 17, decimes, 0);
      putchGLCD(0, 14, run_char, 0);
      if(segons > 9) writeNum(4, 14, segons, 0);
      else putchGLCD(4, 14, '0', 0), writeNum(4, 15, segons, 0);
	    
      if(minuts > 9) writeNum(4, 11, minuts, 0);
      else putchGLCD(4, 11, '0', 0), writeNum(4, 12, minuts, 0);
	    
      if(hores > 9) writeNum(4, 8, hores, 0);
      else putchGLCD(4, 8, '0', 0), writeNum(4, 9, hores, 0);
	 
      TMR0 = 15536; //2^16 - ticks a cronometrar
      TMR0IF = 0;
   }
}


void writeTxt(byte page, byte y, char * s, int n) {
   int i=0;
   while (*s!='\n' && *s!='\0') 
   {
      putchGLCD(page, y+i, *(s++), n);
      i++;
   };
}

void InitPIC()
{
   ANSELB=0x00;                  
   ANSELC=0x00;                  
   ANSELD=0x00;
   ANSELE=0x00;    
   
   TRISD=0x00;		   
   TRISB=0x00;
   TRISE=0x0F;
    
   PORTD=0x00;
   PORTB=0x00; 
   PORTE=0x00;
   
   GIEH = 1;			// Global high priority interrupt enable	
   GIEL = 1;			// Global low priority interrupt enable
   T0CONbits.T08BIT  = 0;
   T0CONbits.T0CS    = 0;
   T0CONbits.T0SE    = 1;
   T0CONbits.PSA     = 0;
   T0CONbits.T0PS2   = 0;
   T0CONbits.T0PS1   = 0;
   T0CONbits.T0PS0   = 1;
   TMR0H             = 0x3C;
   TMR0L             = 0xB0;
   INTCONbits.TMR0IF = 0;
   INTCONbits.T0IE   = 0;
   T0CONbits.TMR0ON  = 1;
}
//3cb0
void main(void)
{
   InitPIC(); 
   
   GLCDinit();		   //Inicialitzem la pantalla
   clearGLCD(0,7,0,127);   //Esborrem pantalla
   setStartLine(0);        //Definim linia d'inici
   
   writeTxt(0, 0, "Press RE0 to start", 0);
   writeTxt(4, 8, "00:00:00.0", 0);
   int b = 0;
   int estat = 0; // 0 = init ; 1 = running ; 2 = stopped
   while (1)
   {   
      if (PORTEbits.RE0 && !b) {
	    b = 1;
	    if (!estat) {
	       ++estat;
	       clearGLCD(0,0,0,127);
	       writeTxt(0, 0, "Timer started", 0);
	       INTCONbits.T0IE = 1;

	       }
	     else if (estat == 1) {
		++estat;
		clearGLCD(0,0,0,127);
		writeTxt(0, 0, "Timer stopped", 0);
		INTCONbits.T0IE = 0;
		}
	     else {
		estat = 0;
		decimes = segons = minuts = hores = 0;
		clearGLCD(0,0,0,127);
		writeTxt(4, 8, "00:00:00.0", 0);
		writeTxt(0, 0, "Press RE0 to start", 0);
		}
	    __delay_ms(2);
      }
      else if (!PORTEbits.RE0 && b) b = 0; 
   }
}