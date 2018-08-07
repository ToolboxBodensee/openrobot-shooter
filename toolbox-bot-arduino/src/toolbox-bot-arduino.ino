
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
int pin_analog_bat = A0;
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

#define RGB_LED_PIN 12
#define LED_COUNT 6

#include <NeoPixelBus.h>
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(LED_COUNT, RGB_LED_PIN);

#define STROBE_TIME_MOD 50
#define FADE_TIME_MOD 4000
#define FADE_TIME_PAUSE 2000
#define FADE_TIME_DUR (FADE_TIME_MOD - FADE_TIME_PAUSE)

int color_saturation = 255;
RgbColor red   (color_saturation,                0,                0);
RgbColor green (               0, color_saturation,                0);
RgbColor blue  (               0,                0, color_saturation);
RgbColor white (color_saturation, color_saturation, color_saturation);
RgbColor black (               0,                0,                0);

enum animations {
  single_off,
  single_red,
  single_green,
  single_blue,
  single_white,
  strobe_white_off,
  strobe_red_green,
  fade_rgb,
};

enum animations current_anim = fade_rgb;
int anim_state = 0;
RgbColor last_fade_color = black, fade_color, last_shown_color;
uint64_t fade_time = 0;

void set_pixels(RgbColor color, bool update_other = false) {
  if (update_other) {
    last_shown_color = color;
  } else {
    last_shown_color = color;
    last_fade_color = color;
  }
  for (int i = 0; i < LED_COUNT; i++) {
    strip.SetPixelColor(i, color);
  }
  strip.Show();
}

void anim_strobe() {
  RgbColor *col1, *col2;
  if (current_anim == strobe_white_off) {
    col1 = &white;
    col2 = &black;
  } else if (current_anim == strobe_red_green) {
    col1 = &red;
    col2 = &green;
  }
  
  if ((millis() % STROBE_TIME_MOD) == 0) {
    if (!anim_state) {
      anim_state = 1;
      set_pixels(*col1);
    } else {
      anim_state = 0;
      set_pixels(*col2);
    }
    delay(1);
  }
}

void anim_fade() {
  if ((millis() % FADE_TIME_MOD) == 0) {
    // define new color target
    fade_color = RgbColor(random(256), random(256), random(256));
    fade_time = millis() + FADE_TIME_DUR;
  }

  if (((millis() + 1) % FADE_TIME_MOD) == 0) {
    last_fade_color = last_shown_color;
  }
  
  if ((millis() < fade_time) && ((millis() % 10) == 0)) {
    float progress = ((float)(fade_time - millis())) / (float)FADE_TIME_DUR;
    progress = 1.0f - progress;
    RgbColor color = RgbColor::LinearBlend(last_fade_color, fade_color, progress);
    set_pixels(color, true);
    delay(1);
  }
}

void show_leds() {
  if (current_anim == single_off) {
    set_pixels(black);
  } else if (current_anim == single_red) {
    set_pixels(red);
  } else if (current_anim == single_green) {
    set_pixels(green);
  } else if (current_anim == single_blue) {
    set_pixels(blue);
  } else if (current_anim == single_white) {
    set_pixels(white);
  } else if ((current_anim >= strobe_white_off) && (current_anim <= strobe_red_green)) {
    anim_strobe();
  } else if (current_anim == fade_rgb) {
    anim_fade();
  }
}

void setup() {
  pinMode(pin_analog_bat, INPUT);
  pinMode(pin_speed_left, OUTPUT);
  pinMode(pin_speed_right, OUTPUT);
  pinMode(pin_dir1_left, OUTPUT);
  pinMode(pin_dir2_left, OUTPUT);
  pinMode(pin_dir1_right, OUTPUT);
  pinMode(pin_dir2_right, OUTPUT);
  pinMode(RGB_LED_PIN, OUTPUT);

  // apply brakes
  digitalWrite(pin_speed_left, LOW);
  digitalWrite(pin_speed_right, LOW);
  digitalWrite(pin_dir1_left, LOW);
  digitalWrite(pin_dir2_left, LOW);
  digitalWrite(pin_dir1_right, LOW);
  digitalWrite(pin_dir2_right, LOW);

  Serial.begin(SERIAL_BAUDRATE);
  Serial.println(SKETCH_ID " ready!");

  strip.Begin();
  strip.Show();
}

int batteryVoltage() {
  analogRead(pin_analog_bat);
  delay(500);
  uint32_t val = analogRead(pin_analog_bat);

  Serial.print("Got raw: ");
  Serial.println(val);
  
  uint32_t vadc = val * 500 / 1024; /* scale adc input to 0 - 500 as Volt^-2 */

  Serial.print("Got scaled: ");
  Serial.println(vadc);
  
  uint32_t vbat = vadc * (ADC_BAT_R1 + ADC_BAT_R2) / ADC_BAT_R2;

  Serial.print("Got bat: ");
  Serial.println(vbat);
  
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
    } else if ((cmd == 'f') || (cmd == 'w')) {
      // drive forward for a bit
      set_motors(DRIVE_SPEED, DRIVE_SPEED);
      delay(DRIVE_DELAY);
      set_motors(0, 0);
    } else if ((cmd == 'b') || (cmd == 's')) {
      // drive backward for a bit
      set_motors(-DRIVE_SPEED, -DRIVE_SPEED);
      delay(DRIVE_DELAY);
      set_motors(0, 0);
    } else if ((cmd == 'r') || (cmd == 'd')) {
      // turn a step right
      set_motors(-TURN_SPEED, TURN_SPEED);
      delay(TURN_DELAY);
      set_motors(0, 0);
    } else if ((cmd == 'l') || (cmd == 'a')) {
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
    } else if (cmd == 'v') {
      // print battery voltage
      Serial.println(voltageToString(batteryVoltage()));
    }

    // "flush" rest of serial input buffer to avoid endless key-repeat loops
    while (Serial.available() > 0) Serial.read();
  }

  show_leds();
}

