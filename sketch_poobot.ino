#include <Servo.h>

#define BLYNK_PRINT Serial
#define USE_SPIFFS true

#define SERVO_MIN 15
#define SERVO_MAX 150
#define SERVO_DELAY 1500

static const uint8_t D0 = 16;
static const uint8_t D1 = 5;
static const uint8_t D2 = 4;
static const uint8_t D3 = 0;
static const uint8_t D4 = 2;
static const uint8_t D5 = 14;
static const uint8_t D6 = 12;
static const uint8_t D7 = 13;
static const uint8_t D8 = 15;
static const uint8_t D9 = 3;
static const uint8_t D10 = 1;

#include <Adafruit_WS2801.h>
#include <SPI.h>

#include <Blynk.h>
#include <BlynkSimpleEsp8266_SSL_WM.h>  // Use server: blynk-cloud.com

#include <Servo.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

Adafruit_BMP280 bmp; //

char auth[] = "tqEJsQDGkFdgIkvYLyigQ64cEKSKGuMv";
char ssid[] = "cemu-wpa2-personal";
char pass[] = "3iHrWF7xxx";

int servo_min = SERVO_MIN;
int servo_max = SERVO_MAX;
int servo_delay = SERVO_DELAY;
Servo servo;

int led_r = 255;
int led_g = 255;
int led_b = 255;

boolean is_initializing = true;

uint8_t led_data  = D3;
uint8_t led_clock = D4;
Adafruit_WS2801 strip = Adafruit_WS2801(10, led_data, led_clock);

void setup()
{
  Serial.begin(9600);

  servo.attach(D8);
  strip.begin();
  strip.show();

  servo.write(servo_min);
  colorWipe(Color(100, 0, 00), 10);

  // Blynk.begin();  // For use with WM
  Blynk.begin(auth, ssid, pass);  //
  colorWipe(Color(0, 0, 100), 20);
  colorWipe(Color(0, 0, 0), 20);

  servo.write(servo_min);

  if (!bmp.begin()) {
    BLYNK_LOG("Could not find a valid BME280 sensor, check wiring!");
  } else {
    BLYNK_LOG("Temp = %f *F", read_temp());
    BLYNK_LOG("Pressure = %f", bmp.readPressure() / 100.0F);
  }

  is_initializing = false;
}

float read_temp() {
  float temp_c = bmp.readTemperature();
  return temp_c * 9 / 5 + 32;
}

void loop()
{
  Blynk.run();
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V1)
{
  int servo_position = param.asInt();

  BLYNK_LOG("V1 Write: %d", servo_position);

  if (servo_position == 0) {
    BLYNK_LOG(" - servo_min");
    servo.write(servo_min);
    if (!is_initializing) {
      colorWipe(Color(0, 0, 0), 30);
    }
  } else if (servo_position == 1) {
    BLYNK_LOG(" - servo_max");
    servo.write(servo_max);
    if (!is_initializing) {
      colorWipe(Color(led_r, led_b, led_g), 30);
    }
  }
}

BLYNK_WRITE(V2)
{
  BLYNK_LOG("V2 (servo_min) Write: %d\n", param.asInt());
  servo_min = param.asInt();
}

BLYNK_WRITE(V3)
{
  BLYNK_LOG("V3 (servo_max) Write: %d\n", param.asInt());
  servo_max = param.asInt();
}

BLYNK_WRITE(V4)
{
  led_r = param[0].asInt();
  led_g = param[1].asInt();
  led_b = param[2].asInt();
  BLYNK_LOG("V4 (RGB) Write: %d, %d, %d\n", led_r, led_g, led_b);
}

BLYNK_READ(V5)
{
  Blynk.virtualWrite(V5, read_temp());
}

uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  int i, j;

  for (j = 0; j < 256; j++) {   // 3 cycles of all 256 colors in the wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;

  for (j = 0; j < 256 * 5; j++) {   // 5 cycles of all 25 colors in the wheel
    for (i = 0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
