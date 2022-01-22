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
int msb;	//msb = duty*10/4 (casualitat dels calculs)
int mode = 0;
int Do = 239;
int Re = 213;
int Mi = 190;
int Fa = 179;
int Sol = 160;
int La = 142;
int Si = 127;
int DO = 119;

void interrupt high_RSI(void)
{
   
   if (TMR2IE && TMR2IF)
   {
      ++it;
      //0s a 1s
      /*
      if (it%5000 <= 500) {
	 if (it%5 == 0) msb += 9;		//Cada 5 cicles duty augmenta 0.9-> 500 cicles +90 duty
	 mode = 0;
	 }
      //1s a 5s: Mantenir (no treure if)
      else if (it%5000 < 2500) {
	 msb = 900;
	 mode = 1;
	 }
      //5s a 6s
      else if (it%5000 < 3000) {
	 --msb;					//Cada cicle msb resta 0.1 -> 500 ciles -50 duty
	 mode = 2;
	 }
      //6s a 7s
      else if (it%5000 < 3500) {
	 if (it%5 == 0) msb -= 2;		//Cada 5 cicles duty resta 0.2-> 500 cicles -20 duty
	 mode = 3;
	 }
      //7s a 8s
      else if (it%5000 < 4000) {
	 if (it%5 == 0) --msb;			//Cada 5 cicles duty resta 0.1-> 500 cicles -10 duty
	 mode = 4;
	 }
      //8s a 9s
      else if (it%5000 < 4500) {
	 if (it%25 == 0) msb -= 4;		//Cada 25 cicles duty resta 0.4-> 500 cicles -8
	 mode = 5;
	 }
      //9s a 10s
      else if (it%5000 < 5000) {
	 if (it%25 == 0) --msb;			//Cada 25 cicles duty resta 0.1-> 500 cicles -2
	  mode = 6;
	 }
      if (it== 5000) {
	 TMR2IE = 0;
	 mode = 6;
	 }*/
     // CCPR1L = msb/4;
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
   ANSELA = 0;
   ANSELE = 0;
   ANSELB=0x00;                  
   ANSELC=0x00;                  
   ANSELD=0x00;   
   
   TRISA = 0xFF;
   TRISD=0x00;		   
   TRISB=0x00;
   TRISE = 0xF;
    
   
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
   int soc = 0;
   int duty, antduty;
   duty = antduty = 0;
   
   InitPIC(); 
   int music[26];
   music[0]= Sol; music[1]= Sol; music[2]= La; music[3]= Sol; music[4]= Do; music[5]= Si; music[6]= Sol; music[7]= Sol; music[8]= La; 
   music[9]= Sol; music[10]= Re; music[11]= Do; music[12]= Sol; music[13]= Sol; music[14]= Sol; music[15]= Mi; music[16]= Do;
   music[17]= Do; music[18]= Si; music[19]= La; music[20]= Fa; music[21]= Fa; music[22]= Mi; music[23]= Do; music[24]= Re;
   music[25] = Do;
   
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
      CCPR1L = 0b01111101;
      for (int i = 0; i <= 25; ++i)
      {
	 PR2 = music[i];
	 __delay_ms(500);
      }
      CCPR1L = 0x00;
      
      /*if (PORTAbits.RA0) {
	PR2 = Do; 
	CCPR1L = 0b01111101;
      }
      else if (PORTAbits.RA1) {
	PR2 = Re; 
	CCPR1L = 0b01111101;
      }
      else if (PORTAbits.RA2) {
	PR2 = Mi; 
	CCPR1L = 0b01111101;
      }
      else if (PORTAbits.RA3) {
	PR2 = Fa; 
	CCPR1L = 0b01111101;
      }
      else if (PORTAbits.RA4) {
	PR2 = Sol; 
	CCPR1L = 0b01111101;
      }
      else if (PORTAbits.RA5) {
	PR2 = La; 
	CCPR1L = 0b01111101;
      }
      else if (PORTEbits.RE0) {
	PR2 = Si; 
	CCPR1L = 0b01111101;
      }
      else if (PORTEbits.RE1) {
	PR2 = DO; 
	CCPR1L = 0b01111101;
      }      
      else CCPR1L = 0b00000000; */
      
   }
}