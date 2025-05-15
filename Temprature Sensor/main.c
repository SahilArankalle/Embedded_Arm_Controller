#include  <lpc214x.h>	  //Includes LPC2148 register definitions

#define LCD_RS_HIGH() 	IO0SET = (1<<16)		 //Function to select data port on LCD
#define LCD_RS_LOW()		IO0CLR = (1<<16)		 //Function to select command port on LCD

#define LCD_RW_HIGH()		IO0SET = (1<<17)		 //Function to select read operation on LCD
#define LCD_RW_LOW()		IO0CLR = (1<<17)		 //Function to select write operation on LCD

#define LCD_EN_HIGH() 	IO0SET = (1<<18)		 //Function to Enable LCD
#define LCD_EN_LOW() 		IO0CLR = (1<<18)		 //Function to disable LCD
//                         "1234567890123456"
unsigned char String1[]={"  LCD Interface "};
unsigned char String2[]={"  Test Program  "};

void  delay_ms(unsigned char time)	   //This Function is used to cause delay between LED ON and OFF events
{  
 unsigned int  i, j;
 for (j=0; j<time; j++)
  for(i=0; i<8002; i++);
}

void LCD_Command(unsigned char command)
{
		unsigned char temp=0;
	
		LCD_RW_LOW();
	  LCD_RS_LOW();
	
		temp = (command & 0xF0) >> 4;
	  IO0CLR = 0xF <<19;			// Clear LCD Data lines
		IO0SET = temp << 19;  

		LCD_EN_HIGH();
		delay_ms(1);
		LCD_EN_LOW();

		temp = (command & 0x0F);
		IO0CLR = 0xF <<19;			// Clear LCD Data lines
		IO0SET = temp << 19;  

		LCD_EN_HIGH();
		delay_ms(1);
		LCD_EN_LOW();
	
}

void LCD_Data(unsigned char data)
{
		unsigned char temp=0;
	
		LCD_RW_LOW();
	  LCD_RS_HIGH();
	
		temp = (data & 0xF0) >> 4;
	  IO0CLR = 0xF <<19;			// Clear LCD Data lines
		IO0SET = temp << 19;  

		LCD_EN_HIGH();
		delay_ms(1);
		LCD_EN_LOW();

		temp = (data & 0x0F);
		IO0CLR = 0xF <<19;			// Clear LCD Data lines
		IO0SET = temp << 19;  

		LCD_EN_HIGH();
		delay_ms(1);
		LCD_EN_LOW();
	
}

void LCD_Init()
{
		LCD_RW_LOW();
	  LCD_RS_LOW();
	
	  IO0CLR = 0x0F <<19;			// Clear LCD Data lines
		IO0SET = 0x03 << 19;  
	
	  LCD_EN_HIGH();
		delay_ms(1);
		LCD_EN_LOW();
	
	  IO0CLR = 0x0F <<19;			// Clear LCD Data lines
		IO0SET = 0x03 << 19;  
	
	  LCD_EN_HIGH();
		delay_ms(1);
		LCD_EN_LOW();
	
	
	  IO0CLR = 0x0F <<19;			// Clear LCD Data lines
		IO0SET = 0x02 << 19;  
	
	  LCD_EN_HIGH();
		delay_ms(1);
		LCD_EN_LOW();
	
		LCD_Command(0x28);
		delay_ms(20);
		LCD_Command(0x28);
		delay_ms(20);
		LCD_Command(0x0C);
		delay_ms(20);
		LCD_Command(0x06);
		delay_ms(20);
		LCD_Command(0x01);
		delay_ms(20);
}

                               
LCD_DisplayString(unsigned char *string)
{
  while(*string)				//Check for End of String
  LCD_Data(*string++); 	//sending data on LCD byte by byte
}

int main(void)
{
	float temp = 0;
	unsigned int ADC_Result=0;
	unsigned char i, Thousands,Hundreds,Tens,Ones;
	PINSEL0 = 0x00000000; 
	PINSEL1 = 0x00040000; // P0.25 as ADC0 CH4 input
	PINSEL2 = 0X00;		//Configure PORT1 as GPIO
	IO0DIR = 0x7F <<16;			// Set P0.16, P0.17, P0.18, P0.19, P0.20, P0.21, P0.22 as Output
	
	LCD_Init();				//Initialize LCD 16x2
	LCD_DisplayString("LM35 Interface");
	LCD_Command(0xc0);//second line
	LCD_DisplayString("TEMP OUT=");
	
	/* Configure ADC0 for following
	ADC Channel = AD0.2
	ADC Clock = 3 MHz
	Clock Selection = 11 Clock Cycles/10bit
	Start Condition = No start
	Power Down = 1, EDGE = 0, BURST = 0 */
	
	AD0CR = 0x01200410;
	
	while (1)
	{
		
		AD0CR |= 1<<24;	//Start ADC 
		//Wait for the conversion to be completed
		while((AD0DR4&0x80000000)==0);			   
		ADC_Result = AD0DR4; //Store converted data
		ADC_Result = (ADC_Result>>6) & 0x3FF;
		LCD_Command (0xCA);			//Goto 10th place on second line of LCD
  	
		temp = ((float)ADC_Result * 3.3 )/1024;
		
		ADC_Result = (unsigned int) (temp * 1000);
		i = (ADC_Result%1000)/100;	//Get the Hundreds place
  	Hundreds = i + 0x30;		// Convert it to ASCII
  	LCD_Data (Hundreds);		//Display Hundreds place
  
		i = ((ADC_Result%1000)%100)/10; //Get the Tens place
  	Tens = i + 0x30;			// Convert it to ASCII
  	LCD_Data (Tens);			//Display Tens place
		
		LCD_Data ('.');			//Display Tens place
  	
		i = ADC_Result%10 ;			//Get the Ones place
  	Ones = i + 0x30; 			// Convert it to ASCII
  	LCD_Data (Ones);		//Display Ones place
		
		LCD_Data (0xDF);			//Display Tens place
		LCD_Data ('C');			//Display Tens place
 
  	delay_ms(250);		//Delay between two conversions
	}
}