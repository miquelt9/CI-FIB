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

int countdown;

int decimes = 0;
int segons = 0;
int minuts = 0;
int hores = 0;
char run_char = 45; // 45 = '-'; 92 = '\';  124 = '|'; 47 = '/'
int estat = 0; // 0 = start ; 1 = ready ; 2 = go ; 3 = win ; 4 = lose

void interrupt high_RSI(void)
{
   if (TMR0IE && TMR0IF)
   {
      --decimes;
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

void escriure_estat(){
   clearGLCD(2,2,0,127);
   if(estat == 0) writeTxt(2, 10, "START", 0);
   else if(estat == 1) writeTxt(2, 10, "READY", 0);
   else if(estat == 2) writeTxt(2, 12, "GO", 0);
   else if(estat == 3) writeTxt(2, 11, "WIN", 0);
   else if(estat == 4) writeTxt(2, 11, "LOSE", 0);
}

void calcular_temps(int countdown_ini)
{
   countdown = countdown_ini;
   hores = countdown/3600; countdown %= 3600;
   minuts = countdown/60; countdown %= 60;   
   segons = countdown;
   decimes = 0;
   countdown = countdown_ini;
}

void main(void)
{
   InitPIC(); 
   int countdown_ini; //Establim temporitzador en segons
   GLCDinit();		   //Inicialitzem la pantalla
   clearGLCD(0,7,0,127);   //Esborrem pantalla
   setStartLine(0);        //Definim linia d'inici
   
   calcular_temps(countdown_ini);
   escriure_estat();

   int clica_abans = 8; // Temps en dècimes que has de clicar abans per guanyar
   int b = 0;
   int decimesAnt = 0;
   while (1)
   {   
      if (PORTEbits.RE0 && !b) {
	    b = 1;
	    if (!estat) {
	       ++estat;
	       escriure_estat();
	       calcular_temps(countdown_ini);
	       INTCONbits.T0IE = 1;
	       segons = rand()%3 + 1;
	       decimes = rand()%10;
	       }
	       else if (estat == 1) {
		  estat = 4;
		  escriure_estat();
		  }
	       else if (estat == 2) {
		  if (countdown > 0) estat = 3;
		  else estat = 4;
		  escriure_estat();
	       }
	    else if (estat == 3 || estat == 4) 
	    {
	       estat = 0;
	       escriure_estat();
	    }
	    __delay_ms(2);
      }
      else if (!PORTEbits.RE0 && b) b = 0; 
      
      // Actualitzem contador
      if(decimes < 0){
	 decimes = 9;
	 --segons;
	 --countdown;
	 if(segons < 0){
	    segons = 59;
	    --minuts;
	    if(minuts < 0){
	       minuts = 59;
	       if (hores > 0) --hores;
	    }
	 }
      }
	 
      if (estat == 1 && !segons && !decimes)
      {
	 estat = 2;
	 escriure_estat();
	 decimes = clica_abans;
	 countdown = 1;
      }
      if (estat == 2 && !segons && !decimes)
      {
	 estat = 4;
	 escriure_estat();
      }
   }
}