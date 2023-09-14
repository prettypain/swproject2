//00S1
#define PIN_LED 13
unsigned int count, toggle;
void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(++count);
  toggle = toggle_status(toggle);
  digitalWrite(PIN_LED, toggle);
  delay(1000);
}
int toggle_status(int toggle){
  return toggle ? 0 : 1;
}
