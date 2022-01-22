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
   //Config GLCD
   ANSELB=0x00;                                  
   ANSELD=0x00;   
   TRISD=0x00;		   
   TRISB=0x00;
   PORTD=0x00;
   PORTB=0x00; 
   
   ANSELA=0xFF;				// Set up A port to analog
   
   TRISA=0xFF;		   		// Set up A port to input
    
   PORTA=0x00;				// A port value = 0;						
   
   // ADC
   ADCON0bits.ADON = 1; 		// Turn on ADC
   ADCON0bits.CHS = 0;			// Select ADC Channel
   ADCON1bits.NVCFG = 0;		// Vref- = AVss
   ADCON1bits.PVCFG = 0; 		// Vref+ = AVdd
   ADCON1bits.TRIGSEL = 0;		// Selects the special trigger from CTMU
   ADCON2bits.ACQT = 1;			// Acquition time 7 = 20TAD 2 = 4TAD 1=2TAD
   ADCON2bits.ADCS = 2;			// Clock conversion bits 6= FOSC/64 2=FOSC/32
   ADCON2bits.ADFM = 0; 		// Results format (1= Right justified; 0= Left justified)

   // GLCD
   GLCDinit();		 		//Inicialitzem la pantalla
   clearGLCD(0,7,0,127);   		//Esborrem pantalla
   setStartLine(0);        		//Definim linia d'inici
   
   // Diverses coses a escriure que es mantindran durant la execució
   writeTxt(0, 0, "adc=", 0);
   writeTxt(7, 0, "0", 0);
   writeTxt(2, 0, "0.00V", 0);
   writeTxt(7, 22, "100", 0);
   writeTxt(6, 7, "adc=   %", 0);
   
}

void main(void)
{

   InitPIC(); 
   int high;
   int low;
   int ant_p = 0;
   int p;
   int v;
   
   while (1)
   {  
      GO = 1;
      while(GO);						// Ens esperem fins que s'hagi obtingut la dada
      high = ADRESH;
      low = ADRESL;
      high = high << 2;
      low = low >> 6;
      high += low;
      writeNum(0, 4, high, 0);
      if (high < 10) writeTxt(0, 5, "   ", 0);			// Uns ifs per ficar el resultat maco i esborrar els possibles digits que ha deixat un resultat abans
      else if (high < 100) writeTxt(0, 6, "  ", 0);
      else if (high < 1000) writeTxt(0, 7, " ", 0);
      
      p = (high*0.0977517106549); 				// Resultat de calcular (100/1023) sinó tenim underflow i se'ns menja els bits
      writeNum(6, 11, p, 0);      
      if (p < 10) writeTxt(6, 12, "  ", 0);			// Uns ifs per ficar el resultat maco i esborrar els possibles digits que ha deixat un resultat abans
      else if (p < 100) writeTxt(6, 13, " ", 0);
      
      v = (p*5);						//Voltage multiplicat per 100 per poder representar decimals sense coma flotant
      writeNum(2, 0, v/100, 0);
      if (v%100 >= 10) writeNum(2, 2, v%100, 0);
      else {writeNum(2,2,0,0); writeNum(2,3,v%100,0);}
      
      if (ant_p < p) {						// Representa que la 'j' va pintant de dalt a baix i la 'i' de costat
	    for (int i = ant_p; i < p; ++i)
	    {
	       for (int j = 0; j < 7; ++j) 
	       {
		  SetDot(j+120, 7+i);
	       }
	    }
	 }
	 else {
	    for (int i = p + 3; i >= ant_p; --i)
	    {
	       for (int j = 0; j < 7; ++j) 
	       {
		  ClearDot(j+120, 7+i);
	       }
	    }
	 }
      ant_p = p;
   }
}


