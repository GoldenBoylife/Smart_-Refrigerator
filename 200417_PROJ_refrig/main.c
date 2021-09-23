
//@2. 성공:온도가 lcd에 표시됨 + 블루투스로 온도 내보냄
/*include */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "Text_LCD.h"
#include "UART1.h"
/*define BEGIN*/

/*define END*/







/*사용자정의함수 BEGIN*/




////ADC 초기화
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

	
	return ADC;				// 10비트 값을 반환
}

/*사용자정의함수 END*/
/*Global 변수 BEGIN*/
char lcd_buf[20];
/*Global 변수 END*/
FILE OUTPUT \
= FDEV_SETUP_STREAM(UART1_transmit, NULL, _FDEV_SETUP_WRITE);
//FDEV_SETUP_STREAM()함수통해서  printf 쓸 수 있다.
FILE INPUT \
= FDEV_SETUP_STREAM(NULL, UART1_receive, _FDEV_SETUP_READ);

uint8_t MODE = 4;	// 8비트 모드인 경우 8, 4비트 모드인 경우 4

int main(void)
{



	////사용하고 있는 핀들
	
	unsigned char sw;
	DDRC = 0xFF;		//포트C 출력 설정: led
	DDRA = 0x0;			//포트A 입력 설정: led 전용 스위치
	//0이면 입력모드, 1이면 출력모드이데, 모두 0이니 입력만
	
	PORTA = 0xFF;		//풀업상태에서 출력에 1111 1111이니까 LED 모두 안켜짐
	

	UART1_init(); // UART0 초기화
	
	
	/*main안에서 선언한 변수들 Begin*/
	int read, temperature;
	float input_voltage;
	
	unsigned int count = 0;
	uint8_t data;
	int i =0;
	/*main안에서 선언한 변수들 End*/
	/*사용자 코드  Begin*/


	stdout = &OUTPUT; //PUTTY에서  printf 사용하도록 설정
	stdin = &INPUT;

	ADC_init(3);				// AD 변환기 초기화. 채널 3 사용

	while(1)
	{
		//printf("%d\n\r", count++);
		//_delay_ms(1000);
		
		sw = PINA & 0b00010000;	//포트E4의 비트에서 상태를 받게하고 &연산한다.
		// 뗀상태면 11101111로 신호상태를 포트E에 준다.
		
		//뗀 상태에서
		//0001 0000 입력되고
		//0001 0000 를 &연산하니까
		//0001 0000 되고 그 값을 sw에 넣는다.
		//떼진상태에서 0001 0000로  e에 주고,
		
		if(sw != 0) PORTA = 0xFF;	//누른상태에서1110 1111을 e에 준다.							//	&연산 0001 0000 하니
		//            0000 0000 된다.
		else PORTA = 0x0;			//SW1 ON -> LED 모두 ON
		
		//@온도 센서
		read = read_ADC();		//ADC로 10비트 나온 값 읽기
		input_voltage = read * 5.0 / 1023.0;
		//0에서 1023 사이의 값을 0과 5v 사이 값으로 변환
		
		temperature = (int)(input_voltage * 100.0)- 10.0;

		
		printf("Temp: %d", temperature);
		_delay_ms(100);
			
			
		LCD_init();				// 텍스트 LCD 초기화, while문안에서 lcd초기화해야 작동함
		memset(lcd_buf,0,20);	//lcd_buf문자열을 모두 0으로 초기화
		sprintf(lcd_buf,"Now Temp  %02d C",temperature);
			
		LCD_goto_XY(0,5);			//커서를 0행 5열로 이동시켜라.
		LCD_write_string("Goldenboy");		//문자열 출력
		LCD_goto_XY(1,0);			 //커서를 1행 3열로 이동시켜라.
		LCD_write_string(lcd_buf);		// 문자열 출력
		_delay_ms(1000);
		//@ 블루투스를 사용해서 스마트폰으로 입력하면 앱에서 printf되도록
		//scanf("%c", &data);			// 문자 단위 데이터 수신
		printf("%c", data);			// 문자 단위 데이터 송신
		
		/*사용자 코드 End */
	}
}

//@ 해야 할 것:  앱인벤터( 온도 버튼 누르면 온도 출력됨. 
//
