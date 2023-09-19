#define PIN_LED 7
int n = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  
  //1초동안 전원 온 하고 0.2초동안 전원 오프
  digitalWrite(PIN_LED, LOW); //전원 온
  delay(1000); //1초 딜레이
  digitalWrite(PIN_LED, HIGH); //전원 오프
  delay(200); //0.2초 딜레이

  Serial.print("repeat 5.\n");

  //현재 꺼짐, 5번 켜짐 시작
  for(int i=0; i<8; i++){ //4번 반복
    digitalWrite(PIN_LED, n); //꺼지고 켜기를 반복
    n = f(n);
    delay(1000); // 0.1초 딜레이
  }

  digitalWrite(PIN_LED, HIGH);
  Serial.print("done\n\n\n");
  while(true); //break
}

int f(int n){
  return n ? 0 : 1;
}
