#include <SoftwareSerial.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>

//custom
//每位量測體溫的使用者至少要間隔多久(ms)
const int interval_measure_timer = 15000;
//量測的ms數
const int measure_timer = 8000;
//模組量測出的溫度, 需要再加多少
const float adjust_temp = 1.75;
//量完溫度後, 顯示溫度幾秒後, 才清空
const int temperature_display = 1000;
//量測管轉動的速度()愈大愈慢
const int servo_move_speed = 10;
//量測管每次移動的距離(愈小愈慢)
const int move_unit = 1;
//量測管移動到量測點的角度
const int measure_angle = 95;

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

void empty_oled() {
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print(" sleep");
  display.display();
  display.clearDisplay();
  delay(1000);
  
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

  objTempe = objTempe + adjust_temp;
  /*
  Serial.print(" Temperature:");
  Serial.print(envTempe);
  Serial.print("/");
  Serial.print(objTempe);
  Serial.print("/");
  Serial.print(maxT);
  */
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
  for (pos = measure_angle; pos >= 0; pos -= move_unit)
  {
    myservo.write(pos);
    delay(servo_move_speed);
  }

  unsigned long last_measure = millis();
  float maxTemp = 0.0;
  while (millis() - last_measure < measure_timer) {
    float now_temp = display_oled(maxTemp);
    if (now_temp > maxTemp) {
      maxTemp = now_temp;
    }
  }

  for (pos = 0; pos <= measure_angle; pos += move_unit)
  {
    myservo.write(pos);
    delay(servo_move_speed);
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
  /*
  Serial.print("PIR:");
  Serial.print(valPIR);
  Serial.println();
  Serial.println(millis() - servo_lasttime);
  */
  if (valPIR == 1) {
    if (millis() - servo_lasttime > interval_measure_timer) {
      welcome_oled();
      servo_lasttime = millis();
      move_servo();
    }
    delay(temperature_display);
    empty_oled();
  }

  delay(250);
}
