#include <SoftwareSerial.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>

//servo
const int pinServo = 9;
Servo myservo;
unsigned long servo_lasttime = millis();

//OLED
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

//PIR
const int pirPin = 8;

//MLX90614
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
int measure_timer = 8000;

void empty_oled() {
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print(" sleep");
  display.display();
  display.clearDisplay();
  delay(1500);
  
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print("       ");
  display.display();
  display.clearDisplay();
}

void welcome_oled() {
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.print(" Welcome");
  display.display();
  display.clearDisplay();

}

float display_oled(float maxT) {
  float envTempe = mlx.readAmbientTempC();
  float objTempe = mlx.readObjectTempC();
  float adjust_temp = 1.25;

  objTempe = objTempe + adjust_temp;
  Serial.print(" Temperature:");
  Serial.print(envTempe);
  Serial.print("/");
  Serial.print(objTempe);
  Serial.print("/");
  Serial.print(maxT);

  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print(envTempe);
  display.print('/');
  display.println(objTempe);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print(maxT);

  display.display();
  display.clearDisplay();

  return objTempe;
}

void move_servo() {
  int pos = 0;
  myservo.write(0);
  for (pos = 95; pos >= 0; pos -= 1)
  {
    myservo.write(pos);
    delay(15);
  }

  unsigned long last_measure = millis();
  float maxTemp = 0.0;
  while (millis() - last_measure < measure_timer) {
    float now_temp = display_oled(maxTemp);
    if (now_temp > maxTemp) {
      maxTemp = now_temp;
    }
  }

  for (pos = 0; pos <= 95; pos += 1)
  {
    myservo.write(pos);
    delay(15);
  }

}
void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  myservo.attach(pinServo);
  mlx.begin();
  // initialize with the I2C addr 0x3D (for the 128x64)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();
}

void loop() {
  int valPIR = digitalRead(pirPin); //read PIR output

  Serial.print("PIR:");
  Serial.print(valPIR);
  Serial.println();
  Serial.println(millis() - servo_lasttime);
  if (valPIR == 1) {
    if (millis() - servo_lasttime > 15000) {
      welcome_oled();
      servo_lasttime = millis();
      move_servo();
    }
    delay(1000);
    empty_oled();
  }

  delay(250);
}
