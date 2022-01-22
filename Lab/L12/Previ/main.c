/* 
 * Created:   2021
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "GLCD.h"
#include "splash.h"
#define _XTAL_FREQ 8388608

char buff[64];
int iteration = 0;
int last_element = 0;

void writeTxt(byte page, byte y, char * s, int n) {
   int i=0;
   while (*s!='\n' && *s!='\0') 
   {
      putchGLCD(page, y+i, *(s++), n);
      i++;
   };
}

char TX1front(void) {
   if (iteration == last_element) return '\0';
   return buff[iteration++];
}


void pushTxt(char str[]) {
   TX1IE = 0;
   if (last_element == iteration) {
      int fi = strlen(str);
      for (int i = 0; i < fi; ++i) buff[i] = str[i];
      iteration = 0;
      last_element = strlen(str);
   }
   else {
      int fi = strlen(str) + last_element;
      for (int i = last_element; i < fi; ++i)  buff[i] = str[i];
      last_element = fi;
   }
   TX1IE = 1;
}

void pushNum(int num) {
   TX1IE = 0;
   char str[16];
   /*int count = 1;
   long n = num;
   while (n > 10) 
   {
      count++; 
      n = n/10;
   }
   writeNum(2,3, num, 0);
   writeNum(3,3, count, 0);*/
   sprintf(str, "%d", num);
   pushTxt(str);
}

void interrupt high_RSI(void) {
   
   if (TX1IE && TX1IF) {
      
     // TXREG1 ='f';
      writeNum(0, 0, iteration, 0); //Per veure que es crida
      writeNum(1, 0, last_element, 0); //Per veure que es crida
      //TX1IE = 0;
      
      char c = TX1front();
      if (c != '\0') TXREG1 = c; 	// Still data to write
      else TX1IE = 0;		 	// No more data to transmit
      }
      // TX1IF is read only and cleared automaticcaly by EUSART   
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
      
   // GLCD
   GLCDinit();		 		//Inicialitzem la pantalla
   clearGLCD(0,7,0,127);   		//Esborrem pantalla
   setStartLine(0);        		//Definim linia d'inici
}

void ConfigConsole() {
   
   ANSELC=0x00; 
   TRISCbits.RC7=1;
   TRISCbits.RC6=1; 
   
   //Linia sèrie config
   SPBRG1 = 16;
   //SPBRGH = 16;				//BaudRate = 117600 (aprox 115200)
   RCSTA1bits.SPEN = 1;			//Serial Port Enable
   TXSTA1bits.BRGH = 1; 		//High mode
   BAUDCON1bits.BRG16 = 1; 		//High baud rate
   TXSTA1bits.SYNC = 0; 		//Asincron
   TXSTA1bits.TX9 = 0;			//8 bits
   TXSTA1bits.TXEN = 1;			//Enable transmit
   
   //enable interrupts
   IPEN = 1;
   PEIE = 1;
   GIE = 1;				// Global high priority interrupt enable	
  // GIEL = 1;				// Global low priority interrupt enable
   TXIE = 1;				//Enable interrupts
   TX1IE = 1;
   }

//Show splashScreen during 1 second
void SplashScreen() {
   for (int i = 0; i < 1024; ++i) {
      writeByte(i/128, i%128, bitmap[i]);
      }
      __delay_ms(1000);
      clearGLCD(0,7,0,127);
   }

void main(void)
{

   InitPIC();
   SplashScreen();
   ConfigConsole();
   TXREG1 =0x11; //XON
   pushTxt("Hello World!\r");
   pushTxt("You can display anything even numbers using pushNum\r");
   __delay_ms(1000);
   pushTxt("Also remember pushNum is limited by the number int limit\r");
   pushNum(1234); 
   __delay_ms(1000);
   pushTxt("\rAlso pushTxt is limited to buff size (actually 64)\r");
   __delay_ms(1000);
   pushTxt("This means that PIC has to write all chars before adding more\r");
   pushTxt("But as writing in console uses interruptions is more\r"); pushTxt("like you cannot use pushTxt with > 64 chars\r");
   __delay_ms(1000);
   pushTxt("If overflow happens strange char may appear"); pushTxt(", then you can increase buff size\r");
   while (1)
   {
      __delay_ms(1000);
      pushTxt("YASSSS\r");
   }
}


