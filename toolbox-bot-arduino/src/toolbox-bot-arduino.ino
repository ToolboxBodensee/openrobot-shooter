
#include <Servo.h>

int pin_speed_left = 5;
int pin_dir1_left = 7;
int pin_dir2_left = 9;

int pin_speed_right = 6;
int pin_dir1_right = 8;
int pin_dir2_right = 10;

int pin_servo = 4;
Servo servo;

/*
 * Convert input voltage (3S LiPo, max. 12.6V)
 * to measurement range of Arduino ADC (5V)
 * 
 *           ------             ------
 * Vin -----|  R1  |-----.-----|  R2  |------- GND
 *           ------      |      ------
 *                       |
 *                   ADC Input
 *
 * Vadc = Vin      * R2   / (R1  + R2)
 * Vadc = 17V (4S) * 4.3k / (12k + 4.3k) = ~4.5V
 * 
 * Vin = Vadc * (R1 + R2) / R2
 * 
 * R1 = 12k Ohm
 * R2 = 4.3k Ohm
 * 
 * Don't use too-high resistances https://electronics.stackexchange.com/questions/141595/arduino-analogin-minimum-current
 */
int pin_analog_bat = 1;
#define ADC_BAT_R1 12000
#define ADC_BAT_R2 4300

#define SKETCH_ID "Toolbox Robot Motor Controller"

/* drive/turn speed from 0 to 127, servo speed in "degrees", delay in milliseconds */
#define DRIVE_SPEED 60
#define DRIVE_DELAY 400
#define TURN_SPEED 60
#define TURN_DELAY 100
#define SERVO_SPEED 20
#define SERVO_DELAY 400

#define SERIAL_BAUDRATE 9600

void setup() {
  pinMode(pin_speed_left, OUTPUT);
  pinMode(pin_speed_right, OUTPUT);
  pinMode(pin_dir1_left, OUTPUT);
  pinMode(pin_dir2_left, OUTPUT);
  pinMode(pin_dir1_right, OUTPUT);
  pinMode(pin_dir2_right, OUTPUT);

  // apply brakes
  digitalWrite(pin_speed_left, LOW);
  digitalWrite(pin_speed_right, LOW);
  digitalWrite(pin_dir1_left, LOW);
  digitalWrite(pin_dir2_left, LOW);
  digitalWrite(pin_dir1_right, LOW);
  digitalWrite(pin_dir2_right, LOW);

  Serial.begin(SERIAL_BAUDRATE);
  Serial.println(SKETCH_ID " ready!");
}

int batteryVoltage() {
  int val = analogRead(pin_analog_bat);
  uint32_t vadc = val * 500 / 1024; /* scale adc input to 0 - 500 as Volt^-2 */
  uint32_t vbat = vadc * (ADC_BAT_R1 + ADC_BAT_R2) / ADC_BAT_R2;
  return (int)vbat; /* 0 to eg. 1642 for 16.42V */
}

String voltageToString(int volt) {
  String s(volt);
  if (volt >= 10) {
    String s1 = s.substring(0, s.length() - 2);
    String s2 = s.substring(s.length() - 2);
    return s1 + "." + s2 + "V";
  } else {
    return "0." + s + "V";
  }
}

void set_motors(int left, int right) {
  if (left > 127) left = 127;
  if (left < -127) left = -127;
  if (right > 127) right = 127;
  if (right < -127) right = -127;
  
  digitalWrite(pin_dir1_left, (left > 0) ? HIGH : LOW);
  digitalWrite(pin_dir2_left, (left < 0) ? HIGH : LOW);
  
  digitalWrite(pin_dir1_right, (right < 0) ? HIGH : LOW);
  digitalWrite(pin_dir2_right, (right > 0) ? HIGH : LOW);

  analogWrite(pin_speed_left, abs(left) * 2);
  analogWrite(pin_speed_right, abs(right) * 2);
}

void loop() {
  if (Serial.available() > 0) {
    int cmd = Serial.read();
    if ((cmd == 'h') || (cmd == '?')) {
      // print some usage help
      Serial.println(SKETCH_ID " Online Help");
      Serial.println("Use wasd or FBRL to drive");
      Serial.println("Use qe to debug servo movement");
      Serial.println("Use b or v to print battery voltage");
    } else if ((cmd == 'F') || (cmd == 'w')) {
      // drive forward for a bit
      set_motors(DRIVE_SPEED, DRIVE_SPEED);
      delay(DRIVE_DELAY);
      set_motors(0, 0);
    } else if ((cmd == 'B') || (cmd == 's')) {
      // drive backward for a bit
      set_motors(-DRIVE_SPEED, -DRIVE_SPEED);
      delay(DRIVE_DELAY);
      set_motors(0, 0);
    } else if ((cmd == 'R') || (cmd == 'd')) {
      // turn a step right
      set_motors(-TURN_SPEED, TURN_SPEED);
      delay(TURN_DELAY);
      set_motors(0, 0);
    } else if ((cmd == 'L') || (cmd == 'a')) {
      // turn a step left
      set_motors(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DELAY);
      set_motors(0, 0);
    } else if ((cmd == ' ') || (cmd == 0x1B /* ESC */)) {
      // stop all motors
      set_motors(0, 0);
    } else if (cmd == 'q') {
      // debug turn servo left
      servo.attach(pin_servo);
      servo.write(SERVO_SPEED);
      delay(SERVO_DELAY);
      servo.detach();
    } else if (cmd == 'e') {
      // debug turn servo right
      servo.attach(pin_servo);
      servo.write(180 - SERVO_SPEED);
      delay(SERVO_DELAY);
      servo.detach();
    } else if ((cmd == 'b') || (cmd == 'v')) {
      // print battery voltage
      Serial.println(voltageToString(batteryVoltage()));
    }

    // "flush" rest of serial input buffer to avoid endless key-repeat loops
    while (Serial.available() > 0) Serial.read();
  }
}

