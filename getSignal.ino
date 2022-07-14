const int buttonPin2 = 2;                           // 조이스틱 쉴드의 버튼이 누르는 걸 입력받기 위해 선언
const int buttonPin3 = 3;
const int buttonPin4 = 4;
const int buttonPin5 = 5;
int clicked[4];
int f=0;
void setup() {
 
  Serial.begin(9600);                               // 시리얼 통신을 시작하며, 통신속도는 9600
 
  pinMode(2, INPUT_PULLUP );
  pinMode(3, INPUT_PULLUP );
  pinMode(4, INPUT_PULLUP );
  pinMode(5, INPUT_PULLUP );
}
 
void loop() {
 if(f==0){
  f=1;
  return;
 }  
  int x = analogRead(1);                           // 변수 X에 아날로그 0번핀에 입력되는 신호를 대입
  int y = analogRead(0);                           // 변수 Y에 아날로그 1번핀에 입력되는 신호를 대입
  Serial.print(" ");
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
 for(int i=0;i<4;i++){
  clicked[i]=digitalRead(i+2)==LOW;
  Serial.print(clicked[i]);
  Serial.print(" ");
  }
 Serial.println();
}
