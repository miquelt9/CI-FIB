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

int it;		//500it = 1s
int msb;	//msb = duty*10 (casualitat dels calculs)
int mode = 0;

void interrupt high_RSI(void)
{
   
   if (TMR2IE && TMR2IF)
   {
      ++it;
      //0s a 1s
      if (it%5000 <= 500) {
	 if (it%5 == 0) msb += 9;		//Cada 5 cicles duty augmenta 0.9-> 500 cicles +90 duty
	 mode = 0;
	 }
      //1s a 5s: Mantenir (no treure if)
      else if (it%5000 < 2500) {
	 msb = 900;
	 }
      //5s a 6s
      else if (it%5000 < 3000) {
	 --msb;					//Cada cicle msb resta 0.1 -> 500 ciles -50 duty
	 mode = 1;
	 }
      //6s a 7s
      else if (it%5000 < 3500) {
	 if (it%5 == 0) msb -= 2;		//Cada 5 cicles duty resta 0.2-> 500 cicles -20 duty
	 mode = 1;
	 }
      //7s a 8s
      else if (it%5000 < 4000) {
	 if (it%5 == 0) --msb;			//Cada 5 cicles duty resta 0.1-> 500 cicles -10 duty
	 }
      //8s a 9s
      else if (it%5000 < 4500) {
	 if (it%25 == 0) msb -= 4;		//Cada 25 cicles duty resta 0.4-> 500 cicles -8
	 }
      //9s a 10s
      else if (it%5000 < 5000) {
	 if (it%25 == 0) --msb;			//Cada 25 cicles duty resta 0.1-> 500 cicles -2
	 }
      if (it== 5000) {
	 TMR2IE = 0;
	 }
      CCPR1L = msb/4;
      TMR2IF = 0;
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
   
   TRISD=0x00;		   
   TRISB=0x00;
    
   PORTD=0x00;
   PORTB=0x00; 
   PORTC=0x00;
   
   GIEH = 1;			// Global high priority interrupt enable	
   GIEL = 1;			// Global low priority interrupt enable
   PEIE = 1;
   //Timer 2 config
   TRISC=0xFF;			//Disable TRISC
   CCPTMRS0 = 0;		//Link CCP1 with Timer 2
   PR2 = 249;			//Set period value
   CCPR1L = 0;			//Set MSB
   CCP1CON = 0b00001100;	//Set LSM and CCP1 in PWM
   TMR2IF = 0;			//Disable flag

   T2CON = 0b00000110;		//Prescaler = 16 and enable timer 2
   TMR2IE   = 1;		//Enable interrupt
   TRISC=0x00;			//Re-enable TRISC
}

void main(void)
{
   it = 0;
   msb = 0;
   double soc = 0;
   int duty, antduty;
   duty = antduty = 0;
   
   InitPIC(); 
   
   GLCDinit();		   //Inicialitzem la pantalla
   clearGLCD(0,7,0,127);   //Esborrem pantalla
   setStartLine(0);        //Definim linia d'inici
   writeTxt(0, 0, "t=", 0);
   writeTxt(7, 0, "0", 0);
   writeTxt(7, 22, "100", 0);
   writeTxt(6, 7, "duty=", 0);
   writeNum (0, 3, it/500, 0);
   writeTxt(0, 18, "SoC=", 0);
   
   while (1)
   {  
	
	if (it%50) {
	 soc += (duty/361.5);
	}
	writeNum(0, 22, soc, 0);
      
      if (it/500 < 10) {
	 //Seconds
	 writeNum (0, 2, it/500, 0);
	 writeTxt(0, 3, ".", 0);
	 //Tens of a second
	 int dec = it/50;
	 dec = dec%10;
	 writeNum (0, 4, dec, 0);
	 }
      else {
	 //Seconds
	 writeNum(0, 2, it/500, 0);
	 writeTxt(0, 4, ".", 0);
	 //Tens of a second
	 int dec = it/50;
	 dec = dec%10;
	 writeNum (0, 5, dec, 0);
      }
      
         //Duty
	 duty = msb/10;
	 writeNum (6, 12, duty, 0);
	 if (duty < 10) clearGLCD(6,6,65,127); 
	    
	 if (mode == 0) {
	    for (int i = antduty; i < duty; ++i)
	    {
	       for (int j = 0; j < 7; ++j) SetDot(j+120, 7+i);
	    }
	    antduty = duty;
	 }
	 else {
	    for (int i = duty + 1; i >= antduty; --i)
	    {
	       for (int j = 0; j < 7; ++j) ClearDot(j+120, 7+i);
	    }
	    antduty = duty;
	 }
      
   }
}