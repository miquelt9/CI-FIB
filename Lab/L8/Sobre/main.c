/*
 * Created:   15-11-2021
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
   writeTxt(2, 10, "L8 CI", 0);
   writeTxt(5, 3, "Miquel Torner Vinals", 0);
   __delay_ms(1000);
   clearGLCD(0,7,0,127);   //Esborrem pantalla
}

void latLines(int xValue, int yValue, int mode)
{
   for(int i = 0; i < 8; ++i)
   {
      if(mode == 0) ClearDot(xValue+i, yValue);
      else SetDot(xValue+i, yValue);
   }
   for(int i = 0; i < 8; ++i)
   {
      if(mode == 0) ClearDot(xValue+i, yValue+6);
      else SetDot(xValue+i, yValue+6);
   }
}

// Si mode < 0, ens indicarà el lloc de la caixa on hem d'imprimir el caràcter,
// sinó servirà per actualitzar el teclat (0 mode light//1 mode dark)
// Utilitzem una 'cerca dicotòmica' per reduir el temps d'execució d'aquesta
// funció que es crida cada vegada que s'ha d'actualitzar alguna cosa
void writePanel(int xValue, int yValue, int mode)
{
int xPos, yPos;
if(mode < 0) {
  xPos = 6;
  yPos = -mode;
  mode = 0;
}
else {
  latLines(8+xValue*8, 14+yValue*10, mode);
  xPos = xValue+1;
  yPos = 3 + yValue*2;
}
//writeNum(3,0,xPos,0);
//writeNum(4,0,yPos,0);
//writeNum(7,2,mode,0);

if(xValue == 0){
  if(yValue < 5)
    if(yValue < 2)
      if(yValue == 0) putchGLCD(xPos, yPos, '1', mode);
      else putchGLCD(xPos, yPos, '2', mode);
    else if(yValue > 2)
      if(yValue == 3) putchGLCD(xPos, yPos, '4', mode);
      else putchGLCD(xPos, yPos, '5', mode);
    else putchGLCD(xPos, yPos, '3', mode);
  else
    if(yValue < 7)
      if(yValue == 6) putchGLCD(xPos, yPos, '7', mode);
      else putchGLCD(xPos, yPos, '6', mode);
    else if(yValue > 7)
      if(yValue == 8) putchGLCD(xPos, yPos, '9', mode);
      else putchGLCD(xPos, yPos, '0', mode);
    else putchGLCD(xPos, yPos, '8', mode);

} else if (xValue == 1) {
  if(yValue < 5)
    if(yValue < 2)
      if(yValue == 0) putchGLCD(xPos, yPos, 'Q', mode);
      else putchGLCD(xPos, yPos, 'W', mode);
    else if(yValue > 2)
      if(yValue == 3) putchGLCD(xPos, yPos, 'R', mode);
      else putchGLCD(xPos, yPos, 'T', mode);
    else putchGLCD(xPos, yPos, 'E', mode);
  else
    if(yValue < 7)
      if(yValue == 6) putchGLCD(xPos, yPos, 'U', mode);
      else putchGLCD(xPos, yPos, 'Y', mode);
    else if(yValue > 7)
      if(yValue == 8) putchGLCD(xPos, yPos, 'O', mode);
      else putchGLCD(xPos, yPos, 'P', mode);
   else putchGLCD(xPos, yPos, 'I', mode);

} else if (xValue == 2) {
  if (yValue < 5)
    if (yValue < 2)
      if(yValue == 0) putchGLCD(xPos, yPos, 'A', mode);
      else putchGLCD(xPos, yPos, 'S', mode);
    else if (yValue > 2)
      if(yValue == 3) putchGLCD(xPos, yPos, 'F', mode);
      else putchGLCD(xPos, yPos, 'G', mode);
    else putchGLCD(xPos, yPos, 'D', mode);
  else
    if (yValue < 7)
      if (yValue == 6) putchGLCD(xPos, yPos, 'J', mode);
      else putchGLCD(xPos, yPos, 'H', mode);
    else if (yValue > 7)
      if(yValue == 8) putchGLCD(xPos, yPos, 'L', mode);
      else putchGLCD(xPos, yPos, '@', mode);
    else putchGLCD(xPos, yPos, 'K', mode);

} else if (xValue == 3) {
  if (yValue < 5)
    if (yValue < 2)
      if(yValue == 0) putchGLCD(xPos, yPos, 'Z', mode);
      else putchGLCD(xPos, yPos, 'X', mode);
    else if (yValue > 2)
      if(yValue == 3) putchGLCD(xPos, yPos, 'V', mode);
      else putchGLCD(xPos, yPos, 'B', mode);
    else putchGLCD(xPos, yPos, 'C', mode);
  else
    if (yValue < 7)
      if (yValue == 6) putchGLCD(xPos, yPos, 'M', mode);
      else putchGLCD(xPos, yPos, 'N', mode);
    else if (yValue > 7)
      if(yValue == 8) putchGLCD(xPos, yPos, '.', mode);
      else putchGLCD(xPos, yPos, ' ', mode);
    else putchGLCD(xPos, yPos, ',', mode);
 }
}


void txtBox()
{
   for(int i = 0; i < 11; ++i) SetDot(110+i, 10);
   for(int i = 0; i < 11; ++i) SetDot(110+i, 113);
   for(int i = 0; i < 104; ++i) SetDot(110, 10+i);
   for(int i = 0; i < 104; ++i) SetDot(120, 10+i);
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

   int xValue, yValue, xValueAnt, yValueAnt;
   xValue = xValueAnt = 9, yValue = yValueAnt = 3;
   int placing = 3;

   int left, right, up, down, premut, timer;
   left = right = up = down = timer = 1;
   premut = 0;

   starting_message();

   txtBox();
   writeTxt(1, 3, "1 2 3 4 5 6 7 8 9 0", 0);
   writeTxt(2, 3, "Q W E R T Y U I O P", 0);
   writeTxt(3, 3, "A S D F G H J K L @", 0);
   writeTxt(4, 3, "Z X C V B N M , .", 0);
   putchGLCD(4, 21, ' ', 1);

    while (1)
    {
       //Right control
       if (PORTAbits.RA0 && !PORTAbits.RA1 && !right) {
	 if (xValue < 9) ++xValue;
	 right = 1;
	 __delay_ms(1);
       }
       else if (right && !PORTAbits.RA0) --right;

       //Left control
       if (PORTAbits.RA1 && !PORTAbits.RA0 && !left) {
	 if (xValue > 0) --xValue;
	 left = 1;
	 __delay_ms(1);
	 }
       else if (left && !PORTAbits.RA1) --left;

       // Up control
       if (PORTAbits.RA2 && !PORTAbits.RA3 && !up) {
	 if (yValue > 0) --yValue;
	 up = 1;
	 __delay_ms(1);
       }
       else if (up&& !PORTAbits.RA2) --up;

      // Down control
      if (PORTAbits.RA3 && !PORTAbits.RA2 && !down) {
	 if (yValue < 3) ++yValue;
	 down = 1;
	 __delay_ms(1);
      }
      else if (down && !PORTAbits.RA3) --down;

      // Premut
      if (PORTAbits.RA4 && !premut) {
	 premut = 1;
	 __delay_ms(1);
	 }
      else if (premut && !PORTAbits.RA4){
	 --premut;
	 if(placing < 22)
	 {
	    writePanel(yValue, xValue, -placing);
	    placing += 1;
	 }
      }
      if(timer%10 == 0 && (xValueAnt != xValue || yValueAnt != yValue))
      {
	 writePanel(yValue, xValue, 1);
	 writePanel(yValueAnt, xValueAnt, 0);
	 xValueAnt = xValue, yValueAnt = yValue;
      }
      ++timer;
   }
}
