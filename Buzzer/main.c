
#include  <lpc214x.h>					 //Includes LPC2148 register definitions


#define BUZZER_ON() IO0CLR=(1<<11)		 //Macro Functions to turn ON Buzzer
#define BUZZER_OFF() IO0SET=(1<<11)		 //Macro Functions to turn OFF Buzzer

void  __irq IRQ_Eint3(void);			 //Prototype function for EINT3 ISR
void Debounce_Delay(unsigned char j);
void Buzzer_Delay(unsigned char j);


unsigned char Count=40;

void Buzzer_Delay(unsigned char j)		 //This function is used to generate delay events between buzzer toggling
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}

void Debounce_Delay(unsigned char j)	//This delay function is used to compensate switch debounci
{  
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<60000; i++);
 } 
}
                               
void INT3_Setup()		 //This function setups EINT3
{
 
 EXTMODE = (1<<3);	// EINT3 is edge sensitive
 EXTPOLAR = (0<<3);					// EINT3 in triggered on falling edge

 VICIntSelect = 0x00000000;		// Setting EINT3 as IRQ(Vectored)
 VICVectCntl0 = 0x20|17;		// Assigning Highest Priority Slot to EINT3 and enabling this slot
 VICVectAddr0 = (int)IRQ_Eint3; // Storing vector address of EINT3
 EXTINT = 0x08;	//Clearing EINT3 flag	
 VICIntEnable = (1<<17);	// Enable EINT3
}


int  main(void)
{  
 PINSEL0 = 0x00000000;		// Enable GPIO on all pins
 PINSEL1 = 0x20000000;
 PINSEL2 = 0x00000000;

 IO0DIR = (1<<11) ;  // Set P0.11 as Output	to control buzzer
 INT3_Setup();		
 while(1)	
 { 
  BUZZER_ON();
  Buzzer_Delay(Count);	 //The count value is updated in the EINT3 ISR
  BUZZER_OFF();
  Buzzer_Delay(Count);
 }
}



void  __irq IRQ_Eint3(void)		//EINT3 ISR
{  
   Count-=5;					//Count value is decremented by 5 every time a switch is pressed
   if(Count<5)					//if count value underflows update it
   { 
    Count=40;
   }
   Debounce_Delay(5);
   EXTINT = 0x08;				// Clear EINT3 flag
   VICVectAddr = 0x00;   		//Acknowledge Interrupt
}	