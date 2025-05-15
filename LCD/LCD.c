#include <LPC213X.H>

#define LCD_CLEAR		0x01
#define CURSOR_OFF 	0x0C
#define FIRST_ROW		0x80
#define SECOND_ROW	0xC0
#define Enable_Pulse()	IOSET0|=1<<EN;Delay_ms(1);IOCLR0=1<<EN;Delay_ms(1);


#define		RS				2
#define		RW				3
#define		EN				4

void UART0_Init(void);
void UART0_Write(unsigned char value);
void UART0_Write_Text(unsigned char msg[]);
unsigned char UART0_Read(void);

void Lcd_Init(void);
void Lcd_Cmd(unsigned char value);
void Lcd_Write(unsigned char value);
void Lcd_Write_Text(unsigned char msg[]);
void Lcd_Data_Shift(unsigned char value);
void Display_Number_Lcd(unsigned char row,unsigned char col,unsigned char places,unsigned long number);
void Display_Number_Fraction_Lcd(unsigned char row,unsigned char col,unsigned char places,unsigned long number);
void ADC0_Init(void);
unsigned int ADC0_Read(void);

void Delay_ms(unsigned long times);

/*********************************************/
unsigned long adc_data;

int main()
{
	unsigned char msg[] = "Electronics With";
	unsigned char adc_example[] = "   Shreyash ";
	unsigned char data_received[] = "ADC value:";

	
	Lcd_Init();
	UART0_Init();
	Delay_ms(10);
	
	UART0_Write_Text(msg);
	UART0_Write(10);
	UART0_Write(13);
	Lcd_Write_Text(msg);
	Lcd_Cmd(SECOND_ROW);
	Lcd_Write_Text(adc_example);
	UART0_Write_Text(adc_example);
	UART0_Write(10);
	UART0_Write(13);
	
	Delay_ms(1000);
	Lcd_Cmd(LCD_CLEAR);
	Lcd_Write_Text(data_received);
	Lcd_Cmd(SECOND_ROW);
	
	ADC0_Init();
	while(1)
	{
		adc_data = ADC0_Read();
		adc_data = adc_data*3300;
		adc_data = adc_data/1023;			//Value of Voltage in Milli Volts
		Display_Number_Lcd(2,1,4,adc_data);
		Lcd_Write('m');
		Lcd_Write('V');
		Delay_ms(10);
	}	
}


void Delay_ms(unsigned long times)
{
	unsigned long i,j;
	for(j=0;j<times;j++)
		for(i=0;i<7500;i++);
}

void Lcd_Init(void)
{
	PINSEL0 = 0x00;
	IODIR0 |= (1<<RS);					//RS Pin as Output Pin
	IODIR0 |= (1<<RW);					//RW Pin as Output Pin
	IODIR0 |= (1<<EN);					//EN Pin as Output Pin
	
	IODIR0 |= 0x0000FF00;				//P0.8 to P0.15 as Data Line of LCD
	
	Lcd_Cmd(0x38);							//Send 8-bit initialization command to lcd
	Delay_ms(10);
	Lcd_Cmd(CURSOR_OFF);				//Cursor OFF
	Delay_ms(10);
	Lcd_Cmd(LCD_CLEAR);
	Delay_ms(1);
	Lcd_Cmd(FIRST_ROW);
}

void Lcd_Data_Shift(unsigned char value)
{

	unsigned char i;
	
	for(i=0;i<8;i++)
	{
		if(value & 0x01)
		{
			IOSET0 |= (1<<(i+8));
		}
		else
		{
			IOCLR0 |= (1<<(i+8));
		}
		value = value >> 1;
	}
}

void Lcd_Cmd(unsigned char value)
{

	IOCLR0 |= (1<<RS);
	IOCLR0 |= (1<<RW);
	IOSET0 |= (1<<EN);
	Lcd_Data_Shift(value);
	Enable_Pulse();
}

void Lcd_Write(unsigned char value)
{

	IOSET0 |= (1<<RS);
	IOCLR0 |= (1<<RW);
	IOSET0 |= (1<<EN);
	Lcd_Data_Shift(value);
	Enable_Pulse();
}

void Lcd_Write_Text(unsigned char msg[])
{
	while(*msg)
	{
		Lcd_Write(*msg);
		msg++;
	}
}



/***************UART-0 Functions**************/
void UART0_Init(void)
{
	PINSEL0 = 0x00000005;				//P0.0 as TX0 and P0.1 as RX0
	U0LCR = 0x83;								//Enable access to Divisor Latches
	//and Set 8 bit Character Length with 1 Stop bit and Parity Disabled
	//Access to Divisor Latches is Enabled, in order to write Baud Rate Generator Registers
	
	//Values to be written in Baud Rate Registers U0DLM and U0LL
	/*
	Formula is
	
	Baud_Rate = PCLK*MulVal / [(16*(256*U0DLM+U0DLL)*(MulVal + DivAddVal))]
	
	Example:-
	MulVal = 1;
	DivAddVal = 0;
	Baud_Rate = 9600;
	PCLK = 15MHz
	U0DLM = 0;
	
	Hence,
	U0DLL = 15000000/(9600*16) = 97.65625 = 98
	U0DLL = 98 = 0x62
	*/
	U0DLM = 0x00;
	U0DLL = 0x62;									//Baud Rate of 9600
	U0LCR = 0x03;									//Disable Access to Divisor Latches
}

void UART0_Write(unsigned char value)
{
	/*	
	THRE bit can be extracted by this U0LSR & 0x20
	THRE = 0 means data is present.
	THRE = 1 means register is empty.
	In order to transmit data, we have to wait will the THRE = 1,
	then only we can transmit data.
	*/
	
	while(!(U0LSR&0x20));						//THRE = 0 stay here
	U0THR = value;
}

void UART0_Write_Text(unsigned char msg[])
{
	while(*msg)
	{
		UART0_Write(*msg);
		msg++;
	}
}

unsigned char UART0_Read(void)
{

	while(!(U0LSR & 0x01));						//RDR = 0 stay here
	return (U0RBR);
}

void ADC0_Init(void)
{

	AD0CR = 1<<21;								//A/D is Operational
	AD0CR = 0<<21;								//A/D is in Power Down Mode
	PCONP = (PCONP &0x001817BE) | (1UL<<12);
	PINSEL0 = 0x00;
	PINSEL1 = 0x00400000;					//P0.27 is Configured as Analog to Digital Converter Pin AD0.0
	
	AD0CR = 0x00200401;						//CLKDIV=4,Channel-0.0 Selected,A/D is Operational

}

unsigned int ADC0_Read(void)
{
	unsigned long adc_data;
	
	AD0CR |= 1UL<<24;							//Start Conversion
	do
	{
		adc_data = AD0GDR;
	}while(!(adc_data & 0x80000000));
	//Wait untill the DONE bits Sets
	AD0CR &= ~0x01000000;					//Stops the A/D Conversion   
	
	adc_data = adc_data >> 6;
	adc_data = adc_data & 0x3FF;    //Clearing all other Bits

return (adc_data);
}
/*********************************************/

/******Functions to Display Text on LCD*******/
void Display_Number_Lcd(unsigned char row,unsigned char col,unsigned char places,unsigned long number)
{
	
	unsigned char thousands, hundreds, tens, ones;
	unsigned long temp;
	
	temp = number;
	ones = temp % 10;
	temp = temp / 10;
	tens = temp % 10;
	temp = temp / 10;
	hundreds = temp % 10;
	temp = temp / 10;
	thousands = temp % 10;
	
	if(row == 1)
	{
		Lcd_Cmd(0X80+col-1);
	}
	if(row == 2)
	{
		Lcd_Cmd(0XC0+col-1);
	}
	
	switch(places)
	{
		case 4:
				Lcd_Write(thousands|0x30);
		case 3:
				Lcd_Write(hundreds|0x30);
		case 2:
				Lcd_Write(tens|0x30);
		case 1:
				Lcd_Write(ones|0x30);
				break;
		default:
				break;
	}
}

void Display_Number_Fraction_Lcd(unsigned char row,unsigned char col,unsigned char places,unsigned long number)
{
	
	unsigned char hundreds, tens, ones;
	unsigned long temp;
	
	temp = number/100;
	
	ones = temp % 10;
	temp = temp / 10;
	tens = temp % 10;
	temp = temp / 10;
	hundreds = temp % 10;
	
	if(row == 1)
	{
		Lcd_Cmd(0X80+col-1);
	}
	if(row == 2)
	{
		Lcd_Cmd(0XC0+col-1);
	}
	
	switch(places)
	{
		case 3:
				Lcd_Write(hundreds|0x30);
		case 2:
				Lcd_Write(tens|0x30);
		case 1:
				Lcd_Write(ones|0x30);
				break;
		default:
				break;
	}
	
	temp = number;
	ones = temp % 10;
	temp = temp / 10;
	tens = temp % 10;
	Lcd_Write('.');
	Lcd_Write(tens|0x30);

	
}