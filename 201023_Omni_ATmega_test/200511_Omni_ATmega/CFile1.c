#include <SoftwareSerial.h> //시리얼 통신하기 위해서
#define EN        8

/*Direction pin*/
//      방향핀     스텝핀
//X축-   5         2
//Y축-   6         3
//Z축-   7         4
#define X_DIR     5
#define Y_DIR     6
#define Z_DIR     7
#define A_DIR     8
//Step pin
#define X_STP     2
#define Y_STP     3
#define Z_STP     4
#define A_STP     5

SoftwareSerial mySerial(10,11); //시리얼 통신할 때 rx= 10, tx=11 포트 쓴다.



//DRV8825
/*전역변수들 Begin*/
char state; //atmega128에서 입력받아오는 값을  저장

int delayTime=40; //Delay between each pause (uS)
//이것으로 속조 조절가능, 40일 때 최대속도, 1000은 최소 속도

int stps=3200;// Steps to move
//이것으로 바퀴 수를 조정 가능
//반바퀴 : 1600
//한바퀴 : 3200
//두바퀴 : 6400


/*전역변수들 End*/
/*사용자정의함수 1: 스텝모터 전용 Begin*/
void step(boolean dir, byte dirPin, byte stepperPin, int steps)

{

	digitalWrite(dirPin, dir);  //출력

	delay(100);

	for (int i = 0; i < steps; i++) { //6400번 반복

		digitalWrite(stepperPin, HIGH);

		delayMicroseconds(delayTime);

		digitalWrite(stepperPin, LOW);

		delayMicroseconds(delayTime);

	}

}
void ini_posi()
{
	
}
/*사용자정의함수 1: 스텝모터 전용 End*/

void setup(){
	mySerial.begin(9600);   //시리얼을 보레잇9600속도로 시작한다.
	pinMode(X_DIR, OUTPUT);
	pinMode(X_STP, OUTPUT);

	pinMode(Y_DIR, OUTPUT);
	pinMode(Y_STP, OUTPUT);

	pinMode(Z_DIR, OUTPUT);
	pinMode(Z_STP, OUTPUT);

	pinMode(Z_DIR, OUTPUT);
	pinMode(Z_STP, OUTPUT);
	
	pinMode(EN, OUTPUT);    //이게 없으면 안돌아간다.

	//digitalWrite(EN, LOW);

}

void loop(){
	//@시계방향으로 계속 돈다..
	if(mySerial.available())      //수신되었는지 상태 확인
	{
		state = mySerial.read();    //수신된 데이터를 1byte 읽어서 state 변수에 저장함
		
		if(state == 'a')
		{

			step(false, X_DIR, X_STP, stps); //X, Clockwise
			step(false, X_DIR, X_STP, stps); //X, Clockwise

			step(false, Z_DIR, Z_STP, stps); //Z, Clockwise
			step(false, Z_DIR, Z_STP, stps); //Z, Clockwise


		}
		else if(state == 'b')
		{

			step(true, X_DIR, X_STP, stps); //X, Counterclockwise
			step(true, X_DIR, X_STP, stps); //X, Counterclockwise
			
			step(true, Z_DIR, Z_STP, stps); //X, Counterclockwise
			step(true, Z_DIR, Z_STP, stps); //X, Counterclockwise
			delay(100);
		}
		else state == 'c';
	}
}