/* 
 * Created:   2021
 * Processor: PIC18F45K22
 * Compiler:  MPLAB XC8
 * Bernat Borràs Civil i Miquel Torner Viñals
 */

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "GLCD.h"
#include "splash.h"
#define _XTAL_FREQ 8388608

char buff[1024];
int iteration = 0;
int last_element = 0;

int decimes = 0;
int segons = 0;
int minuts = 0;
int hores = 0;
char run_char = 45; // 45 = '-'; 92 = '\';  124 = '|'; 47 = '/'

int xValue, yValue, xValueAnt, yValueAnt;
int left, right, up, down;
int t_left, t_right, t_up, t_down;
int cicles_bloquejar;
int move, pre_move;

int high_acd;
int low_acd;
int puntuacio;

int nivell, prenivell;
int update = 0;

//Combustible
int x_jerry_can = 50;
int y_jerry_can = 50;
int jerry_can_grabbed = 0;

//Enemic 1
int x_enemy1 = 127;
int y_enemy1 = 0;
int dir_enemy1;

//Enemic 2
int x_enemy2 = 127;
int y_enemy2 = 0;
int dir_enemy2;

//Enemic 3
int x_enemy3 = 127;
int y_enemy3 = 0;
int dir_enemy3;
int over = 0;

int just_one_inter;

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

   sprintf(str, "%d", num);
   pushTxt(str);
}

void interrupt high_RSI(void) {
   
   if (TX1IE && TX1IF) { 
      char c = TX1front();
      if (c != '\0') TXREG1 = c; 	// Still data to write
      else TX1IE = 0;		 	// No more data to transmit
   }
   
   if (!just_one_inter && RC1IE && RC1IF) {
      char c = RCREG1;
      if (c == 'p') {
	 if (xValue < 108) {++xValue; move = 'R';}
	 right++; 
	 pushTxt("Right\r");
      }
      else if (c == 'o') {
	 if (xValue > 3) {--xValue; move = 'L';}
	 left++;
	 pushTxt("Left\r");
      }
      else if (c == 'a') {
	 if (yValue < 60) {++yValue; move = 'U';}
	 up++;
	 pushTxt("Down\r");
      }
      else if (c == 'q') {
	 if (yValue > 11) {--yValue; move = 'D';}
	 down++;
	 pushTxt("Up\r");
      }
      just_one_inter = 1;
   }
   
   //Comptador de temps al tmr0
   if (TMR0IE && TMR0IF)
   { 
      if (run_char < 50) {
	 if (run_char == 45) run_char = 92; 
	 else run_char = 45;
      }
      else {
	 if (run_char == 92) run_char = 124;
	 else run_char = 47;
      }
      up = down = left = right = 0;
      if (decimes == 0) update = 1; //Contador per redibuixar elements
      if (decimes == 0 && segons%2 == 0) {
	 dir_enemy1 = rand()%4;	//Actualitzar direccio enemic cada 2 segons
	 dir_enemy2 = rand()%4;
	 dir_enemy3 = rand()%4;
	 RC1IE = 1;		// Tornem a activar-les
	 TX1IE = 1;
	 }
      
      ++decimes;
      if(decimes > 9){
	 decimes = 0;
	 if(segons%2) GO = 1; 		// Només actualitzarem el valor rebut per el potenciometre 1 vegada cada segon
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
      TMR0 = 13107;
      TMR0IF = 0;
   }
   
   // Interrupcio potenciometre A/C
   if (ADIE && ADIF)
   {
      high_acd = ADRESH;
      low_acd = ADRESL;
      high_acd = high_acd << 2;
      low_acd = low_acd >> 6;
      high_acd += low_acd;
      ADIF = 0;
   }
   
   //PWM
   if (TMR2IE && TMR2IF)
   {
      CCPR1L = 8*puntuacio;
      TMR2IF = 0;  
      }
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
   
   ANSELA=0x01;				// Configure the A Port as digital except for potenciometer (AN0)
   ANSELC=0x00;
  
   TRISA=0xFF;
   TRISCbits.RC7=1;
   TRISCbits.RC6=1; 
   
   PORTA=0x00;
   
   //PWM
   TRISC=0xFF;			//Disable TRISC
   CCPTMRS0 = 0;		//Link CCP1 with Timer 2
   PR2 = 249;			//Set period value
   CCPR1L = 0;			//Set MSB
   CCP1CON = 0b00001100;	//Set LSM and CCP1 in PWM
   TMR2IF = 0;			//Disable flag

   T2CON = 0b00000110;		//Prescaler = 16 and enable timer 2
   TMR2IE   = 1;		//Enable interrupt
   TRISC=0x00;			//Re-enable TRISC
   
   TRISCbits.RC7=1;
   TRISCbits.RC6=1; 
   
   //Linia sèrie config
   SPBRG1 = 16;
   //SPBRGH = 16;			//BaudRate = 117600 (aprox 115200)
   RCSTA1bits.SPEN = 1;			//Serial Port Enable
   TXSTA1bits.BRGH = 1; 		//High mode
   BAUDCON1bits.BRG16 = 1; 		//High baud rate
   TXSTA1bits.SYNC = 0; 		//Asincron
   TXSTA1bits.TX9 = 0;			//8 bits
   TXSTA1bits.TXEN = 1;			//Enable transmit
   RCSTA1bits.RX9 = 0;			// 8 bits datsa receiving
   RCSTA1bits.CREN = 1;			// Enable receiving
   RC1IE = 1;				// Enable receiving interrupts (port 1 data)
   RCIE = 1; 				// Enable receiving(data)
   
   //enable interrupts
   IPEN = 1;
   PEIE = 1;
   GIE = 1;				// Global high priority interrupt enable	
   GIEL = 1;				// Global low priority interrupt enable
   TXIE = 1;				// Enable interrupts
   TX1IE = 1;
   
   //TMR0 config
   T0CONbits.T08BIT  = 0;
   T0CONbits.T0CS    = 0;
   T0CONbits.T0SE    = 1;
   T0CONbits.PSA     = 0;
   T0CONbits.T0PS2   = 0;
   T0CONbits.T0PS1   = 0;
   T0CONbits.T0PS0   = 1;
   TMR0H             = 0x00;
   TMR0L             = 0x00;
   INTCONbits.TMR0IF = 0;
   INTCONbits.T0IE   = 0;
   T0CONbits.TMR0ON  = 1;
   
   // ADC
   ADCON0bits.ADON = 1; 		// Turn on ADC
   ADCON0bits.CHS = 0;			// Select ADC Channel
   ADCON1bits.NVCFG = 0;		// Vref- = AVss
   ADCON1bits.PVCFG = 0; 		// Vref+ = AVdd
   ADCON1bits.TRIGSEL = 0;		// Selects the special trigger from CTMU
   ADCON2bits.ACQT = 1;			// Acquition time 7 = 20TAD 2 = 4TAD 1=2TAD
   ADCON2bits.ADCS = 2;			// Clock conversion bits 6= FOSC/64 2=FOSC/32
   ADCON2bits.ADFM = 0; 		// Results format (1= Right justified; 0= Left justified)
   }

//Show splashScreen during 1 second
void SplashScreen() {
   for (int i = 0; i < 1024; ++i) {
      writeByte(i/128, i%128, bitmap[i]);
      }
      __delay_ms(1000);
      clearGLCD(0,7,0,127);
   }

int random_y()
{
   return rand()%35 + 15;
}
int random_x()
{
   return rand()%80 + 15;
}
   

void dibuixar_cors(int n) {
   if (n > 0) writeTxt(0, 0, "$", 0); 		//$ redibuixat com a cor a ascii.h
   else writeTxt(0, 0, " ", 0);
    
   if (n > 1) writeTxt(0, 2, "$", 0);
   else writeTxt(0, 2, " ", 0);
    
   if (n > 2) writeTxt(0, 4, "$", 0);
   else writeTxt(0, 4, " ", 0);
   }
  
void dibuixar_barra_dificultat(){
   for (int i = 0; i < 30; ++i) // Verticals
   {
      SetDot(i+20, 113);
      SetDot(i+20, 122);
   }
   for (int i = 0; i < 10; ++i) //Horitzontals
   {
      SetDot(20, 113+i);
      SetDot(50, 113+i);
   }
}

void dibuixar_barra(int ant_p, int p){
   if (ant_p < p) {						// Representa que la 'j' va pintant de dalt a baix i la 'i' de costat
      for (int i = ant_p; i < p; ++i)
      {
	 for (int j = 108; j < 114; ++j) 
	 {
	    SetDot(48-i, 7+j);
	 }
      }
   }
   else {
      for (int i = ant_p; i >= p; --i)
      {
	 for (int j = 108; j < 114; ++j) 
	 {
	    ClearDot(48-i, 7+j);
	 }
      }
   }
}


void esborrar_pixels(int xValue, int yValue, int mode)
{
   for(int i = 3; i < 5; ++i)
   {
      if(mode == 0) ClearDot(xValue+i, yValue-1);
      else SetDot(xValue+i, yValue-1);
   }
   for(int i = 3; i < 5; ++i)
   {
      if(mode == 0) ClearDot(xValue+i, yValue+5);
      else SetDot(xValue+i, yValue+5);
   }
}

void dibuixar_combustible(int x, int y)
{
   SetDot(x, y + 2);
   SetDot(x + 1, y + 2);
   SetDot(x + 2, y); SetDot(x + 2, y + 1); SetDot(x + 2, y + 2); SetDot(x + 2, y + 3); SetDot(x + 2, y + 4);
   SetDot(x + 3, y); SetDot(x + 3, y + 4);
   SetDot(x + 4, y); SetDot(x + 4, y + 4);
   SetDot(x + 5, y); SetDot(x + 5, y + 4);
   SetDot(x + 6, y + 1); SetDot(x + 6, y + 2); SetDot(x + 6, y + 3);
}

void esborrar_combustible(int x, int y)
{
   ClearDot(x, y + 2);
   ClearDot(x + 1, y + 2);
   ClearDot(x + 2, y); ClearDot(x + 2, y + 1); ClearDot(x + 2, y + 2); ClearDot(x + 2, y + 3); ClearDot(x + 2, y + 4);
   ClearDot(x + 3, y); ClearDot(x + 3, y + 4);
   ClearDot(x + 4, y); ClearDot(x + 4, y + 4);
   ClearDot(x + 5, y); ClearDot(x + 5, y + 4);
   ClearDot(x + 6, y + 1); ClearDot(x + 6, y + 2); ClearDot(x + 6, y + 3);
}


void dibuixar_nau(char mov, char pre_mov, int x, int y)
{
   //Combustible
   if (jerry_can_grabbed == 1) {
      esborrar_combustible(y_jerry_can, x_jerry_can);
      if (mov == 'U') ++y_jerry_can;
      else if (mov == 'D') --y_jerry_can;
      else if (mov == 'L') --x_jerry_can;
      else if (mov == 'R') ++x_jerry_can;
      dibuixar_combustible(y_jerry_can, x_jerry_can);
      }
   //Dibuixar
   if (pre_mov != mov) {
      ClearDot(x-3, y); ClearDot(x+3, y); ClearDot(x, y+3); ClearDot(x, y-3); 
      ClearDot(x-3, y-1); ClearDot(x+3, y-1); ClearDot(x-1, y+3); ClearDot(x-1, y-3);
      ClearDot(x-3, y+1); ClearDot(x+3, y+1); ClearDot(x+1, y+3); ClearDot(x+1, y-3); 
      if (mov == 'U') {      
	 SetDot(x, y); SetDot(x-1, y); SetDot(x-2, y); SetDot(x-3, y); SetDot(x+1, y); SetDot(x+2, y); SetDot(x+3, y); ClearDot(x-4, y);	//Middle
	 SetDot(x, y-1); SetDot(x-2, y-1); SetDot(x-1, y-1); SetDot(x+1, y-1); SetDot(x+2, y-1); ClearDot(x-3, y-1);	//Left
	 SetDot(x, y+1); SetDot(x-2, y+1); SetDot(x-1, y+1); SetDot(x+1, y+1); SetDot(x+2, y+1); ClearDot(x-3, y+1);	//Right
	 SetDot(x+1, y+2); SetDot(x, y+2); SetDot(x-1, y+2); SetDot(x-2, y+2); ClearDot(x-3, y+2);				//Right x2
	 SetDot(x+1, y-2); SetDot(x, y-2); SetDot(x-1, y-2); SetDot(x-2, y-2); ClearDot(x-3, y-2);				//Left x2
      }
      else if (mov == 'D') {
	 SetDot(x, y); SetDot(x-1, y); SetDot(x-2, y); SetDot(x-3, y); SetDot(x+1, y); SetDot(x+2, y); SetDot(x+3, y); ClearDot(x+4, y);	//Middle
	 SetDot(x, y-1); SetDot(x-2, y-1); SetDot(x-1, y-1); SetDot(x+1, y-1); SetDot(x+2, y-1); ClearDot(x+3, y-1);	//Left
	 SetDot(x, y+1); SetDot(x-2, y+1); SetDot(x-1, y+1); SetDot(x+1, y+1); SetDot(x+2, y+1); ClearDot(x+3, y+1);	//Right
	 SetDot(x+1, y+2); SetDot(x, y+2); SetDot(x-1, y+2); SetDot(x+2, y+2); ClearDot(x+3, y+2);				//Right x2
	 SetDot(x+1, y-2); SetDot(x, y-2); SetDot(x-1, y-2); SetDot(x+2, y-2); ClearDot(x+3, y-2);				//Left x2
      }
      else if (mov == 'R') {
	 SetDot(x, y); SetDot(x, y-1); SetDot(x, y-2); SetDot(x, y-3); ClearDot(x, y-4); 			//Middle
	 SetDot(x, y+1); SetDot(x, y+2); SetDot(x, y+3); SetDot(x+1, y+2); SetDot(x-1, y+2);		//Middle and front
	 SetDot(x-1, y); SetDot(x-1, y+1); SetDot(x-1, y-1); SetDot(x-1, y-2); ClearDot(x-1, y-3);		//Down
	 SetDot(x+1, y); SetDot(x+1, y+1); SetDot(x+1, y-1); SetDot(x+1, y-2); ClearDot(x+1, y-3);		//Up
	 SetDot(x+2, y); SetDot(x+2, y+1); SetDot(x+2, y-1); SetDot(x+2, y-2); ClearDot(x+2, y-3);		//Down x1
	 SetDot(x-2, y); SetDot(x-2, y+1); SetDot(x-2, y-1); SetDot(x-2, y-2); ClearDot(x-2, y-3);		//Up x2
      }
      else if (mov == 'L') {
	 SetDot(x, y); SetDot(x, y-1); SetDot(x, y-2); SetDot(x, y-3); ClearDot(x, y+4); 			//Middle
	 SetDot(x, y+1); SetDot(x, y+2); SetDot(x, y+3); SetDot(x+1, y-2); SetDot(x-1, y-2);		//Middle and front
	 SetDot(x-1, y); SetDot(x-1, y+1); SetDot(x-1, y-1); SetDot(x-1, y+2); ClearDot(x-1, y+3);		//Down
	 SetDot(x+1, y); SetDot(x+1, y+1); SetDot(x+1, y-1); SetDot(x+1, y+2); ClearDot(x+1, y+3);		//Up
	 SetDot(x+2, y); SetDot(x+2, y+1); SetDot(x+2, y-1); SetDot(x+2, y+2); ClearDot(x+2, y+3);		//Down x1
	 SetDot(x-2, y); SetDot(x-2, y+1); SetDot(x-2, y-1); SetDot(x-2, y+2); ClearDot(x-2, y+3);		//Up x2
      }
   }
   else {
      if (mov == 'U') {      
	 SetDot(x+3, y); ClearDot(x-4, y);	//Middle
	 SetDot(x+2, y-1); ClearDot(x-3, y-1);	//Left
	 SetDot(x+2, y+1); ClearDot(x-3, y+1);	//Right
	 SetDot(x+1, y+2); ClearDot(x-3, y+2);				//Right x2
	 SetDot(x+1, y-2); ClearDot(x-3, y-2);				//Left x2   
      }
      else if (mov == 'D') {
	 SetDot(x-3, y); ClearDot(x+4, y);	//Middle
	 SetDot(x-2, y-1); ClearDot(x+3, y-1);	//Left
	 SetDot(x-2, y+1); ClearDot(x+3, y+1);	//Right
	 SetDot(x-1, y+2); ClearDot(x+3, y+2);				//Right x2
	 SetDot(x-1, y-2); ClearDot(x+3, y-2);				//Left x2
      }
      else if (mov == 'R') {
	 ClearDot(x, y-4); SetDot(x, y+3); SetDot(x+1, y+2); SetDot(x-1, y+2);		//Middle and front
	 ClearDot(x-1, y-3);		//Down
	 ClearDot(x+1, y-3);		//Up
	 SetDot(x+2, y+1); ClearDot(x+2, y-3);		//Down x2
	 SetDot(x-2, y+1); ClearDot(x-2, y-3);		//Up x2
      }
      else if (mov == 'L') {
	 SetDot(x, y-3); ClearDot(x, y+4); 			//Middle
	 SetDot(x+1, y-2); SetDot(x-1, y-2);		//Middle and front
	 ClearDot(x-1, y+3);		//Down
	 ClearDot(x+1, y+3);		//Up
	 SetDot(x+2, y-1); ClearDot(x+2, y+3);		//Down x2
	 SetDot(x-2, y-1); ClearDot(x-2, y+3);		//Up x2
      }
   }
}

void dibuixar_enemic(int x, int y)
{
   SetDot(x,y); SetDot(x-1,y);					//Middle
   SetDot(x-2,y-1); SetDot(x-3,y-1); SetDot(x+1,y-1); SetDot(x+2,y-1);	//Left
   SetDot(x-4,y-2); SetDot(x-3,y-2); SetDot(x+3,y-2); SetDot(x+2,y-2);	//Left x2
   SetDot(x-2,y+1); SetDot(x-3,y+1); SetDot(x+1,y+1); SetDot(x+2,y+1);	//Right
   SetDot(x-4,y+2); SetDot(x-3,y+2); SetDot(x+3,y+2); SetDot(x+2,y+2);	//Right x2
}

void esborrar_enemic(int x, int y)
{
   ClearDot(x,y); ClearDot(x-1,y);					//Middle
   ClearDot(x-2,y-1); ClearDot(x-3,y-1); ClearDot(x+1,y-1); ClearDot(x+2,y-1);	//Left
   ClearDot(x-4,y-2); ClearDot(x-3,y-2); ClearDot(x+3,y-2); ClearDot(x+2,y-2);	//Left x2
   ClearDot(x-2,y+1); ClearDot(x-3,y+1); ClearDot(x+1,y+1); ClearDot(x+2,y+1);	//Right
   ClearDot(x-4,y+2); ClearDot(x-3,y+2); ClearDot(x+3,y+2); ClearDot(x+2,y+2);	//Right x2
}

void dibuixar_esborrar_enemic(int ant_x, int ant_y, int x, int y)
{
   SetDot(x,y); SetDot(x-1,y);					//Middle
   SetDot(x-2,y-1); SetDot(x-3,y-1); SetDot(x+1,y-1); SetDot(x+2,y-1);	//Left
   SetDot(x-4,y-2); SetDot(x-3,y-2); SetDot(x+3,y-2); SetDot(x+2,y-2);	//Left x2
   SetDot(x-2,y+1); SetDot(x-3,y+1); SetDot(x+1,y+1); SetDot(x+2,y+1);	//Right
   SetDot(x-4,y+2); SetDot(x-3,y+2); SetDot(x+3,y+2); SetDot(x+2,y+2);	//Right x2
   ClearDot(ant_x,ant_y); ClearDot(ant_x-1,ant_y);					//Middle
   ClearDot(ant_x-2,ant_y-1); ClearDot(ant_x-3,ant_y-1); ClearDot(ant_x+1,ant_y-1); ClearDot(ant_x+2,ant_y-1);	//Left
   ClearDot(ant_x-4,ant_y-2); ClearDot(ant_x-3,ant_y-2); ClearDot(ant_x+3,ant_y-2); ClearDot(ant_x+2,ant_y-2);	//Left x2
   ClearDot(ant_x-2,ant_y+1); ClearDot(ant_x-3,ant_y+1); ClearDot(ant_x+1,ant_y+1); ClearDot(ant_x+2,ant_y+1);	//Right
   ClearDot(ant_x-4,ant_y+2); ClearDot(ant_x-3,ant_y+2); ClearDot(ant_x+3,ant_y+2); ClearDot(ant_x+2,ant_y+2);	//Right x2
}

void dibuixar_plataforma(int x, int y)
{
   SetDot(x,y); SetDot(x,y + 4);
   SetDot(x+1,y); SetDot(x+1,y + 4);
   SetDot(x+2,y); SetDot(x+2,y + 1); SetDot(x+2,y+2); SetDot(x+2,y + 3); SetDot(x+2,y+4);
   SetDot(x+3,y + 1); SetDot(x+3,y+2); SetDot(x+3,y + 3);
}

void dibuixar_esborrar_enemic_op(int *x, int *y, int *dir) {
   
   if (*dir == 0) {
      if (*x < 108 && *y < 60) {
	 ++*x; ++*y;
	 SetDot(*y+2, *x+2); SetDot(*y+1, *x+2); ClearDot(*y-1, *x-1); SetDot(*y-3, *x-1); ClearDot(*y-4, *x-3); ClearDot(*y-5, *x-3);
	 SetDot(*y+2, *x-2); SetDot(*y+1, *x-2); SetDot(*y+1, *x-1); SetDot(*y, *x-1); SetDot(*y-2, *x+1); SetDot(*y-3, *x+1); SetDot(*y-3, *x+2); SetDot(*y-4, *x+2);
	 ClearDot(*y+1, *x-3); ClearDot(*y, *x-3); ClearDot(*y, *x-2); ClearDot(*y-1, *x-2); ClearDot(*y-3, *x); ClearDot(*y-4, *x); ClearDot(*y-4, *x+1); ClearDot(*y-5, *x+1);
      }
      else *dir = rand()%4;
   }
   else if (*dir == 1) {
      if (*x > 3 && *y < 60) {
	 --*x; ++*y;
	 SetDot(*y+2, *x-2); ClearDot(*y-1, *x+1); SetDot(*y+1, *x-2); ClearDot(*y-4, *x+3); SetDot(*y-3, *x+1); ClearDot(*y-5, *x+3);
	 SetDot(*y-2, *x-1); SetDot(*y-3, *x-1); SetDot(*y-3, *x-2); SetDot(*y-4, *x-2); SetDot(*y, *x+1); SetDot(*y+1, *x+1); SetDot(*y+1, *x+2); SetDot(*y+2, *x+2); 
	 ClearDot(*y-3, *x); ClearDot(*y-4, *x); ClearDot(*y-4, *x-1); ClearDot(*y-5, *x-1); ClearDot(*y-1, *x+2); ClearDot(*y, *x+2); ClearDot(*y, *x+3); ClearDot(*y+1, *x+3);

      }
      else *dir = rand()%4;
   }
   else if (*dir == 2) {
      if (*x < 108 && *y > 11) {
	 ++*x; --*y;
	 SetDot(*y-4, *x+2); ClearDot(*y-2, *x); SetDot(*y-3, *x+2); ClearDot(*y+2, *x-3); SetDot(*y, *x); ClearDot(*y+3, *x-3);
	 SetDot(*y-2, *x-1); SetDot(*y-3, *x-1); SetDot(*y-3, *x-2); SetDot(*y-4, *x-2); SetDot(*y, *x+1); SetDot(*y+1, *x+1); SetDot(*y+1, *x+2); SetDot(*y+2, *x+2); 
	 ClearDot(*y-1, *x-2); ClearDot(*y-2, *x-2); ClearDot(*y-2, *x-3); ClearDot(*y-3, *x-3); ClearDot(*y+1, *x); ClearDot(*y+2, *x); ClearDot(*y+2, *x+1); ClearDot(*y+3, *x+1);
      }
      else *dir = rand()%4;
   }
   else {
      if (*x > 3 && *y > 11) {
	 --*x; --*y;
	 SetDot(*y-4, *x-2); SetDot(*y-3, *x-2); ClearDot(*y-2, *x); SetDot(*y, *x); ClearDot(*y+3, *x+3); ClearDot(*y+2, *x+3);
	 SetDot(*y+2, *x-2); SetDot(*y+1, *x-2); SetDot(*y+1, *x-1); SetDot(*y, *x-1); SetDot(*y-2, *x+1); SetDot(*y-3, *x+1); SetDot(*y-3, *x+2); SetDot(*y-4, *x+2);
	 ClearDot(*y+3, *x-1); ClearDot(*y+2, *x-1); ClearDot(*y+2, *x); ClearDot(*y+1, *x); ClearDot(*y-1, *x+2); ClearDot(*y-2, *x+2); ClearDot(*y-2, *x+3); ClearDot(*y-3, *x+3);
      }
      else *dir = rand()%4;
   }
}

void Win() {
   nivell = 0;
   over = 1;
   clearGLCD(0,7,0,127);
   writeTxt(3, 7, "Player wins", 0);
   }

void GameOver() {
   nivell = 0;
   over = 1;
   clearGLCD(0,7,0,127);
   writeTxt(3, 7, "Game Over", 0);
   }


   
void main(void)
{

   InitPIC();
   SplashScreen();
   ConfigConsole();
   pushTxt("Game loaded successfully\r");
   writeTxt(0, 14, "00:00:00.0", 0);
   dibuixar_barra_dificultat();
   
   int cors = 3;
   int cors_ant = 0;
   T0IE = 1;
   TMR0IE = 1;
   int dec_ant = 0;
   int high_acd_ant = 0;
   int p; int ant_p;
   int clock = 0;
   int premut = 0;
   int mov_x_dec = 3; // Numbers of pixel movement allowed for the player in a decimal
   puntuacio = 0;
   prenivell = 0;
   
   move ='D';
   pre_move = 'U';
   
   xValue = xValueAnt = 60, yValue = yValueAnt = 59;

   left = right = up = down = 0;
   cicles_bloquejar = 200;
   
   ADIF = 0;
   ADIE = 1;
   GO = 1;
   //writeTxt(3, 3, xValue, 0);
   //writeTxt(4, 3, yValue, 0);
   pushNum(xValue); pushTxt(" ");
   pushNum(yValue); pushTxt("\r");
   //writeNum(0, 8, puntuacio, 0);
   
   dibuixar_nau(move, pre_move, yValue, xValue);
   dibuixar_plataforma(55, 95);
   //dibuixar_enemic(y_enemy1,x_enemy1);
   dibuixar_combustible(y_jerry_can, x_jerry_can);
   
   just_one_inter = 0;
   
   while (1)
   {   
       premut = 0;
       //Right control
       if (PORTAbits.RA2 && !premut && right < mov_x_dec) {
	 if (xValue < 108) {++xValue; move = 'R';}
	 right++; 
	 premut++;
	 pushTxt("Right\r");
       }
	  
       //Left control
       if (PORTAbits.RA4 && !premut && left < mov_x_dec) {
	 if (xValue > 3) {--xValue; move = 'L';}
	 left++;
	 premut++;
	 pushTxt("Left\r");
       }
	  
       //Down control
       if (PORTAbits.RA1 && !premut && up < mov_x_dec) {
	 if (yValue < 60) {++yValue; move = 'U';}
	 up++;
	 premut++;
	 pushTxt("Down\r");
       }
	  
       //Up control
       if (PORTAbits.RA3 && !premut && down < mov_x_dec) {
	 if (yValue > 11) {--yValue; move = 'D';}
	 down++;
	 premut++;
	 pushTxt("Up\r");
       }
       just_one_inter = 0;
       //Agafar combustible
       if (!jerry_can_grabbed && xValue > x_jerry_can - 4 && xValue < x_jerry_can + 8 && yValue > y_jerry_can - 4 && yValue < y_jerry_can + 10) {
	  //mov_x_dec = 2;
	  jerry_can_grabbed = 1;
       }
      
      
       //Deixar combustible
	 if (jerry_can_grabbed == 1 && 94 < x_jerry_can && 100 > x_jerry_can ) {
	    //mov_x_dec = 3;
	    jerry_can_grabbed = 2;	    
	  }
	 
	  
      if (jerry_can_grabbed == 2)
      {	 
	  esborrar_combustible(y_jerry_can, x_jerry_can);
	  dibuixar_combustible(++y_jerry_can, x_jerry_can);
	  if (y_jerry_can > 49) {
	     srand(x_jerry_can*rand()*(decimes+1)); // Fem un nou srand per obtenir diferents nombres
	     esborrar_combustible(y_jerry_can, x_jerry_can);
	     x_jerry_can = random_x();
	     y_jerry_can = random_y();
	     jerry_can_grabbed = 0;
	     dibuixar_barra(puntuacio, puntuacio + 4);
	     puntuacio += 4;
	     //writeNum(0, 8, puntuacio, 0);
	     //pushTxt("New can pos: "); pushNum(x_jerry_can); pushTxt(" "); pushNum(y_jerry_can); pushTxt("\r");
	     dibuixar_combustible(y_jerry_can, x_jerry_can);
	     dibuixar_plataforma(55, 95);
	     if (puntuacio == 28) Win();
	  }
      }
       
      //Cada 3 segons redibuixar elements per possibles bugs
      if (update == 1 && !over) {
	 dibuixar_plataforma(55, 95);
	 dibuixar_combustible(y_jerry_can, x_jerry_can);
	 dibuixar_nau(pre_move, pre_move, yValue, xValue);
	 update = 0;
	 }

      //Perdre vida
      if (xValue > x_enemy1 - 7 && xValue < x_enemy1 + 8 && yValue > y_enemy1 - 4 && yValue < y_enemy1 + 8) {
	  esborrar_enemic(y_enemy1, x_enemy1);
	  --cors;
	 y_enemy1 = 25;
	 x_enemy1 = 50;
	 dibuixar_enemic(y_enemy1, x_enemy1);
	 if (cors == -1) GameOver();
	  }
      if (xValue > x_enemy2 - 7 && xValue < x_enemy2 + 8 && yValue > y_enemy2 - 4 && yValue < y_enemy2 + 8) {
	  esborrar_enemic(y_enemy2, x_enemy2);
	  --cors;
	 y_enemy2 = 25;
	 x_enemy2 = 50;
	 dibuixar_enemic(y_enemy2, x_enemy2);
	 if (cors == -1) GameOver();
	  }
	  
      if (xValue > x_enemy3 - 7 && xValue < x_enemy3 + 8 && yValue > y_enemy3 - 4 && yValue < y_enemy3 + 8) {
	  esborrar_enemic(y_enemy3, x_enemy3);
	  --cors;
	 y_enemy3 = 25;
	 x_enemy3 = 50;
	 dibuixar_enemic(y_enemy3, x_enemy3);
	 if (cors == -1) GameOver();
	    }
	  
      //Actualitzar comptador
      if (dec_ant != decimes && !over) {
	 writeNum(0, 23, decimes, 0);
	 if(segons > 9) writeNum(0, 20, segons, 0);
	 else putchGLCD(0, 20, '0', 0), writeNum(0, 21, segons, 0);
	 if(minuts > 9) writeNum(0, 17, minuts, 0);
	 else putchGLCD(0, 17, '0', 0), writeNum(0, 18, minuts, 0);
	 if(hores > 9) writeNum(0, 14, hores, 0);
	 else putchGLCD(0, 14, '0', 0), writeNum(0, 15, hores, 0);
	 putchGLCD(0, 24, run_char, 0);
	 dec_ant = decimes;
	 }
      
      // Dibuixar cors
      if (cors_ant != cors)
      {
	 dibuixar_cors(cors);
	 cors_ant = cors;
      }
      
      //Dibuixar nau
      if (xValue != xValueAnt || yValue != yValueAnt) {
	 RC1IE = 0;
	 //TX1IE = 0;
	 dibuixar_nau(move, pre_move, yValue, xValue);
	 yValueAnt = yValue;
	 xValueAnt = xValue;
	 //TX1IE = 1;
	 RC1IE = 1;
      }
      
      //Moure enemics
      if (nivell >= 1) {
	 esborrar_enemic(y_enemy1, x_enemy1);
	 if (dir_enemy1 == 0) {
	    if (x_enemy1 < 108 && y_enemy1 < 60) {++x_enemy1; ++y_enemy1;}
	    else dir_enemy1 = rand()%4;
	    }
	 else if (dir_enemy1 == 1) {
	    if (x_enemy1 > 3 && y_enemy1 < 60) {--x_enemy1; ++y_enemy1;}
	    else dir_enemy1 = rand()%4;
	    }
	 else if (dir_enemy1 == 2) {
	    if (x_enemy1 < 108 && y_enemy1 > 11) {++x_enemy1; --y_enemy1;}
	    else dir_enemy1 = rand()%4;
	    }
	 else {
	    if (x_enemy1 > 3 && y_enemy1 > 11) {--x_enemy1; --y_enemy1;}
	    else dir_enemy1 = rand()%4;
	    }
	 dibuixar_enemic(y_enemy1, x_enemy1);
	 //dibuixar_esborrar_enemic_op(&y_enemy1, &x_enemy1, &dir_enemy1);
	 }
	 
      if (nivell >= 2) {
	 esborrar_enemic(y_enemy2, x_enemy2);
	 if (dir_enemy2 == 0) {
	    if (x_enemy2 < 108 && y_enemy2 < 60) {++x_enemy2; ++y_enemy2;}
	    else dir_enemy2 = rand()%4;
	    }
	 else if (dir_enemy2 == 1) {
	    if (x_enemy2 > 3 && y_enemy2 < 60) {--x_enemy2; ++y_enemy2;}
	    else dir_enemy2 = rand()%4;
	    }
	 else if (dir_enemy2 == 2) {
	    if (x_enemy2 < 108 && y_enemy2 > 11) {++x_enemy2; --y_enemy2;}
	    else dir_enemy2 = rand()%4;
	    }
	 else {
	    if (x_enemy2 > 3 && y_enemy2 > 11) {--x_enemy2; --y_enemy2;}
	    else dir_enemy2 = rand()%4;
	    }
	 dibuixar_enemic(y_enemy2, x_enemy2);
	 //dibuixar_esborrar_enemic_op(&y_enemy2, &x_enemy2, &dir_enemy2);
	 }
	 
      if (nivell >= 3) {
	 esborrar_enemic(y_enemy3, x_enemy3);
	 if (dir_enemy3 == 0) {
	    if (x_enemy3 < 108 && y_enemy3 < 60) {++x_enemy3; ++y_enemy3;}
	    else dir_enemy3 = rand()%4;
	    }
	 else if (dir_enemy3 == 1) {
	    if (x_enemy3 > 3 && y_enemy3 < 60) {--x_enemy3; ++y_enemy3;}
	    else dir_enemy3 = rand()%4;
	    }
	 else if (dir_enemy3 == 2) {
	    if (x_enemy3 < 108 && y_enemy3 > 11) {++x_enemy3; --y_enemy3;}
	    else dir_enemy3 = rand()%4;
	    }
	 else {
	    if (x_enemy3 > 3 && y_enemy3 > 11) {--x_enemy3; --y_enemy3;}
	    else dir_enemy3 = rand()%4;
	    }
	 dibuixar_enemic(y_enemy3, x_enemy3);
	 //dibuixar_esborrar_enemic_op(&y_enemy3, &x_enemy3, &dir_enemy3);
	 }
 
      if (high_acd_ant != high_acd)
      {
	 ADIE = 0;
	 nivell = (high_acd*0.00391); 			// Resultat de calcular (n_nivells=5/1023) sinó tenim underflow i se'ns menja els bits

	 //writeNum(1, 23, nivell, 0);      
	 p = (nivell*6.75);					// Resultat de calcular (27/n_nivells=5) sinó tenim underflow i se'ns menja els bits
	 if (nivell != prenivell) {
	    prenivell = nivell;
	    if (nivell == 0) {
	       if (x_enemy3 != 127 || y_enemy3 != 0) {
		  esborrar_enemic(y_enemy3, x_enemy3);
		  x_enemy3 = 127;
		  y_enemy3 = 0;
		  }
	       if (x_enemy2 != 127 || y_enemy2 != 0) {
		  esborrar_enemic(y_enemy2, x_enemy2);
		  x_enemy2 = 127;
		  y_enemy2 = 0;
		  }
	       if (x_enemy1 != 127 || y_enemy1 != 0) {
		  esborrar_enemic(y_enemy1, x_enemy1);
		  x_enemy1 = 127;
		  y_enemy1 = 0;
		  }
	       }
	    else if (nivell == 1) {
	       if (x_enemy3 != 127 || y_enemy3 != 0) {
		  esborrar_enemic(y_enemy3, x_enemy3);
		  x_enemy3 = 127;
		  y_enemy3 = 0;
		  }
	       if (x_enemy2 != 127 || y_enemy2 != 0) {
		  esborrar_enemic(y_enemy2, x_enemy2);
		  x_enemy2 = 127;
		  y_enemy2 = 0;
		  }
	       if (x_enemy1 == 127 && y_enemy1 == 0) {
		  x_enemy1 = 20;
		  y_enemy1 = 20;
		  }
	       }
	    else if (nivell == 2) {
	       if (x_enemy3 != 127 || y_enemy3 != 0) {
		  esborrar_enemic(y_enemy3, x_enemy3);
		  x_enemy3 = 127;
		  y_enemy3 = 0;
		  }
	       if (x_enemy2 == 127 && y_enemy2 == 0) {
		  x_enemy2 = 40;
		  y_enemy2 = 40;
		  }
	       if (x_enemy1 == 127 && y_enemy1 == 0) {
		  x_enemy1 = 20;
		  y_enemy1 = 20;
		  }
	       }
	    else {
	       if (x_enemy3 == 127 && y_enemy3 == 0) {
		  x_enemy3 = 60;
		  y_enemy3 = 20;
		  }
	       if (x_enemy2 == 127 && y_enemy2 == 0) {
		  x_enemy2 = 20;
		  y_enemy2 = 20;
		  }
	       if (x_enemy1 == 127 && y_enemy1 == 0) {
		  x_enemy1 = 20;
		  y_enemy1 = 20;
		  }
	       }
	    }	 
	 ant_p = p;
	 high_acd_ant = high_acd;
	 ADIE = 1;
      }
      
      ++clock;
      pre_move = move;
   }
}
