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
//Retorna el caràcter corresponent a la posició (posx, posy)
char* caracter(int x, int y) {
   if (y == 1) {
      if (x == 3) return "1";
      else if (x == 5) return "2";
      else if (x == 7) return "3";
      else if (x == 9) return "4";
      else if (x == 11) return "5";
      else if (x == 13) return "6";
      else if (x == 15) return "7";
      else if (x == 17) return "8";
      else if (x == 19) return "9";
      else if (x == 21) return "0";
      }
   if (y == 2) {
      if (x == 3) return "Q";
      else if (x == 5) return "W";
      else if (x == 7) return "E";
      else if (x == 9) return "R";
      else if (x == 11) return "T";
      else if (x == 13) return "Y";
      else if (x == 15) return "U";
      else if (x == 17) return "I";
      else if (x == 19) return "O";
      else if (x == 21) return "P";
      }
    if (y == 3) {
      if (x == 3) return "A";
      else if (x == 5) return "S";
      else if (x == 7) return "D";
      else if (x == 9) return "F";
      else if (x == 11) return "G";
      else if (x == 13) return "H";
      else if (x == 15) return "J";
      else if (x == 17) return "K";
      else if (x == 19) return "L";
      else if (x == 21) return "@";
      }
     if (y == 4) {
      if (x == 3) return "Z";
      else if (x == 5) return "X";
      else if (x == 7) return "C";
      else if (x == 9) return "V";
      else if (x == 11) return "B";
      else if (x == 13) return "N";
      else if (x == 15) return "M";
      else if (x == 17) return ",";
      else if (x == 19) return ".";
      else if (x == 21) return " ";
      }
   }
   
void linies(int posx, int posy) {
    for (int i = 0; i < 8; ++i) {
       SetDot(posy*8 + i, posx*5 - 1);
       SetDot(posy*8 + i, (posx + 1)*5);
       }
   }
   
void esborrar_linies(int posx, int posy) {
   for (int i = 0; i < 8; ++i) {
       ClearDot(posy*8 + i, posx*5 - 1);
       ClearDot(posy*8 + i, (posx + 1)*5);
       }
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
   
   writeTxt(1, 3, "1 2 3 4 5 6 7 8 9 0", 0);
   writeTxt(2, 3, "Q W E R T Y U I O P", 0);
   writeTxt(3, 3, "A S D F G H J K L @", 0);
   writeTxt(4, 3, "Z X C V B N M , .", 0);
   writeTxt(4, 21, " ", 1);
   
   int posx = 21;
   int posy = 4;
   int b0 = 0;
   int b1 = 0;
   int b2 = 0;
   int b3 = 0;
   int b4 = 0;
   
   int postxt = 3;
   
   for (int i = 11; i <= 113; ++i) {
      SetDot(46, i);
      SetDot(56, i);
      }
   for (int i = 46; i <= 56; ++i) {
      SetDot(i, 11);
      SetDot(i, 113);
      }
      
   linies(posx, posy);
   
   while (1)
   {   
	 //Right control
     	if (PORTAbits.RA0 && !b0) {
	    b0 = 1;
	    if (posx < 21) {
	       writeTxt(posy, posx, caracter(posx, posy), 0);
	       esborrar_linies(posx, posy);
	       posx += 2;
	       writeTxt(posy, posx, caracter(posx, posy), 1);
	       linies(posx, posy);
	       }
	       __delay_ms(2);
	 }
	 else if (!PORTAbits.RA0 && b0) b0 = 0;
	  
	 //Right control 
	 if (PORTAbits.RA1 && !b1) {
	    b1 = 1;
	    if (posx > 3) {
	       writeTxt(posy, posx, caracter(posx, posy), 0);
	       esborrar_linies(posx, posy);
	       posx -= 2;
	       writeTxt(posy, posx, caracter(posx, posy), 1);
	       linies(posx, posy);
	       }
	       __delay_ms(2);
	 }
	 else if (!PORTAbits.RA1 && b1) b1 = 0;
	 
	 //Up control 
	 if (PORTAbits.RA2 && !b2) {
	    b2 = 1;
	    if (posy > 1) {
	       writeTxt(posy, posx, caracter(posx, posy), 0);
	       esborrar_linies(posx, posy);
	       --posy;
	       writeTxt(posy, posx, caracter(posx, posy), 1);
	       linies(posx, posy);
	       }
	       __delay_ms(2);
	 }
	 else if (!PORTAbits.RA2 && b2) b2 = 0;
	 
	 //Down control 
	 if (PORTAbits.RA3 && !b3) {
	    b3 = 1;
	    if (posy < 4) {
	       writeTxt(posy, posx, caracter(posx, posy), 0);
	       esborrar_linies(posx, posy);
	       ++posy;
	       writeTxt(posy, posx, caracter(posx, posy), 1);
	       linies(posx, posy);
	       }
	       __delay_ms(2);
	 }
	 else if (!PORTAbits.RA3 && b3) b3 = 0;
	 
	 //Enter
	 if (!PORTAbits.RA4 && b4) {
	    b4 = 0;
	    if (postxt < 22) {
	       writeTxt(6, postxt, caracter(posx, posy), 0);
	       ++postxt;
	       }
	    __delay_ms(2);
	 }
	 else if (PORTAbits.RA4 && !b4) b4 = 1;
   }
}