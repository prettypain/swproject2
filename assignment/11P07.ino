#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   9   // LED active-low
#define PIN_TRIG  12  // sonar sensor TRIGGER
#define PIN_ECHO  13  // sonar sensor ECHO
#define PIN_SERVO 10  // servo motor

// configurable parameters for sonar
#define SND_VEL 346.0     // sound velocity at 24 degrees Celsius (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultrasonic Pulse Duration (unit: usec)
#define _DIST_MIN 180.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 360.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficient to convert duration to distance

#define _EMA_ALPHA 0.6    // EMA weight of new sample (range: 0 to 1), I choose 0.3 because this tracking many many fast.
                          // Setting EMA to 1 effectively disables EMA filter.

// Target Distance
#define _TARGET_LOW  180.0
#define _TARGET_HIGH 220.0

// Duty duration for myservo.writeMicroseconds()
// NEEDS TUNING (servo by servo)
// configurable parameters
#define _DUTY_MIN 537   //540 ~ 545,         servo full clock-wise position (0 degree)
#define _DUTY_MAX 2397  //2395 ~ 2400,       servo neutral position (90 degree)
#define _DUTY_NEU (_DUTY_MIN + _DUTY_MAX)/2//  servo full counter-clockwise position (180 degree)

// Global variables
float  dist_ema, dist_prev = _DIST_MAX; // unit: mm
unsigned long last_sampling_time;       // unit: ms

Servo myservo;

void setup() {
  // Initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);    // Sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);     // Sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // Turn off Sonar 

  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);

  // Initialize USS related variables
  dist_prev = _DIST_MIN; // raw distance output from USS (unit: mm)

  // Initialize serial port
  Serial.begin(57600);
}



void loop() {
  float  dist_raw;
  
  // wait until next sampling time. 
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO); // read distance
  
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
    dist_raw = dist_prev;           // Cut higher than maximum
    digitalWrite(PIN_LED, 1);       // LED OFF
  } else if (dist_raw < _DIST_MIN) {
    dist_raw = dist_prev;           // cut lower than minimum
    digitalWrite(PIN_LED, 1);       // LED OFF
  } else {    // In desired Range
    dist_prev = dist_raw;
    digitalWrite(PIN_LED, 0);       // LED ON      
  }

  

  // Apply ema filter here  
  dist_ema = _EMA_ALPHA * dist_raw + (1-_EMA_ALPHA)*dist_ema;

  // adjust servo position according to the USS read value
  // add your code here!
  float tic = (_DUTY_MAX - _DUTY_MIN)/18;
  myservo.writeMicroseconds((dist_ema - _DIST_MIN)/10*tic + _DUTY_MIN);

  // output the distance to the serial port
  Serial.print("Min:");    Serial.print(_DIST_MIN);
  Serial.print(",Low:");   Serial.print(_TARGET_LOW);
  Serial.print(",dist:");  Serial.print(dist_raw);
  Serial.print(",Servo:"); Serial.print(myservo.read());  
  Serial.print(",High:");  Serial.print(_TARGET_HIGH);
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.print(",TIC:");   Serial.print((dist_ema - _DIST_MIN)/10*tic + _DUTY_MIN); //537+(2397-537)/18
  Serial.println("");
 
  // update last sampling time
  last_sampling_time += INTERVAL;
}

// Get a distance reading from USS. Return value is in millimeters.
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // Unit: mm
}
