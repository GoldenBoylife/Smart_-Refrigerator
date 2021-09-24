/*include */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "Text_LCD.h"


/*define BEGIN*/

/*define END*/





/*사용자정의함수 BEGIN*/
/*사용자정의함수 1: UART0,

 전용 Begin*/
void UART0_init(void)
{
	//Universial Asynchronous Receive and Transmit
	UBRR0H = 0x00;
	UBRR0L = 207;
	
	
	UCSR0C |= 0x06;
	
	UCSR0B |= _BV(RXEN0); // 송수신 가능
	UCSR0B |= _BV(TXEN0);
	UCSR0A |= _BV(U2X0);
	//2배속으로
	// 비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드

}


void UART0_transmit(char data)  //
{
	while( !(UCSR0A & (1 << UDRE0)) ); 


	UDR0 = data;

}

void UART0_print_string(char *str)	
{
	for(int i = 0; str[i]; i++)		
	UART0_transmit(str[i]);		
}
unsigned char UART0_receive(void)
{


	while( !(UCSR0A & (1<<RXC0)) );	
	return UDR0;
}


char UART0_GetChar(void)
{
	
	char ch;
	while(!(UCSR0A & ( 1<<RXC0)));	
	ch = UDR0;		
	return ch;
}
/*사용자정의함수 1: UART0,문자열: printf,scanf 전용 End*/
/*사용자정의함수 2: ADC  Begin*/
void ADC_init(unsigned char channel)
{
	ADMUX |= (1 << REFS0); 		// AVCC를 기준 전압으로 선택
	
	ADCSRA |= 0x07;			// 분주비 설정
	ADCSRA |= (1 << ADEN);		// ADC 활성화
	ADCSRA |= (1 << ADFR);		// 프리 러닝 모드

	ADMUX |= ((ADMUX & 0xE0) | channel);	// 채널 선택

	ADCSRA |= (1 << ADSC);		// 변환 시작
}


////ADC 읽는 부분
int read_ADC(void)
{
	while(!(ADCSRA & (1 << ADIF)));	// 변환 종료 대

	
	return ADC;				// 비트 값을 반환
}

/*사용자정의함수 2: ADC  End*/


/*사용자정의함수 END*/
/*Global 변수 BEGIN*/
char lcd_buf[100];
uint8_t MODE = 4;	// 8비트 모드인 경우 8, 4비트 모드인 경우 4

/*Global 변수 END*/
FILE OUTPUT \
= FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
//FDEV_SETUP_STREAM()함수통해서  printf 쓸 수 있다.
//주의할점 UART0_transmit()이 함수인데 뒤에 ()를 붙이지 않더라..
FILE INPUT \
= FDEV_SETUP_STREAM(NULL, UART0_receive,_FDEV_SETUP_READ);
//UART를 통해 전달되는 데이터를 stream 형태로 바꿔 주기 위한 객체를 생성해야 한다.


int main(void)
{
	/*사용하는 핀 정의 Begin*/
		/*회로계획: 풀업스위치... 누르면 0, 떼면 1
		스위치1,2,3 : PB0,PB1,
				
		PB0 풀업스위치 누르면 'a'를 UART로 전송, PB1풀업스위치 누르면 'B'를 UART전송	
		LED는 항상켜져 있어야 함 -> 파워서플라이 5V에 항상 연결시킬 것. 
		textLCD: PC0, PC1, PC2 ,  PD4,PD5,PD6,PD7
		블루투스: PD2(28),PD3
		온도센서: PF3
		uart통신 : PE0, PE1  이유: UART0를 쓰기 때문
		*/
		
	unsigned char sw;


	
	DDRB &= ~0x07;	
	
	/*사용하는 핀 정의 End*/
	
	/*각종 함수 init Begin*/
	ADC_init(3);				// AD 변환기 초기화. 채널 3 사용
	UART0_init();

	
	stdout = &OUTPUT; //PUTTY에서  printf 사용하도록 설정
	stdin = &INPUT;
	
	
	/*main안에서 선언한 변수들 Begin*/
	unsigned int count = 0;
	int i =0;
	char state_pre1=0;
	char state_current1;
	char state_pre2=0;
	char state_current2;
	char state_pre3=0;
	char state_current3;
	
	int read, temperature;
	float input_voltage;
	
	uint8_t data;

	
	
	/*main안에서 선언한 변수들 End*/
	/*사용자 코드  Begin*/
	LCD_init();					
		
	LCD_goto_XY(0,5);			
	LCD_write_string("Hello");		
	LCD_goto_XY(1,3);			 
	LCD_write_string("GoldenBoy");		
	_delay_ms(1000);			


	while(1)
	{

		
		state_current1 = !((PINB & 0x01) >>0);		
		state_current2 = !((PINB & 0x02) >>1);		
		state_current3 = !((PINB & 0x04) >>2);		
		_delay_ms(30);
		if(state_current1 ==1 &&  state_pre1 ==0) 
		{
			UART0_transmit('a');
		}
		state_pre1 = state_current1;		//버튼 상태 업뎃
		if(state_current2 ==1 &&  state_pre2 ==0)
		{
			UART0_transmit('b');
		}
		state_pre2 = state_current2;		//버튼 상태 업뎃
		if(state_current3 ==1 &&  state_pre3 ==0)
		{

			UART0_transmit('c');

		}
		state_pre3 = state_current3;		//버튼 상태 업뎃
		

		

		read = read_ADC();		//ADC로 10비트 나온 값 읽기
		input_voltage = read * 5.0 / 1023.0;


		temperature = (int)(input_voltage * 100.0)- 10.0;


		sprintf( lcd_buf,"now temp %d `C", temperature);	
		
		
		LCD_init();				
		
			
		LCD_goto_XY(0,0);			
		LCD_write_string(lcd_buf);		
		LCD_goto_XY(1,3);			 
		LCD_write_string("GoldenBoy");		

			
		
	/*사용자 코드 End */
	}
	

	return 0;
}
