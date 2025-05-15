#include <LPC214x.h>

void  delay_ms(unsigned char time)	   //This Function is used to cause delay between LED ON and OFF events
{  
 unsigned int  i, j;
 for (j=0; j<time; j++)
  for(i=0; i<8002; i++);
}

int  main(void)
{  
	unsigned char val = 0x01;
	PINSEL2 = 0x00000000; 	//Configure PORT1 as GPIO
	IODIR1 = 0xF << 16 ;		//Configure P1.16- P1.19 as output
 while(1)	
 { 
		IOSET1 = val << 16; 	//Turn ON LED
		delay_ms (500);	
		IOCLR1 = val << 16;	//Turn OFF LED
	  val = val << 1; 
		if (val == 0x10) { val = 0x01;}	
 }

}
