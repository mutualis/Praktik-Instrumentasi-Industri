#include <MQUnifiedsensor.h>
#include "DHT.h"
#include <Servo.h>

//Definitions
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0                  //Analog input 0 of your arduino
#define type "MQ-135"           //MQ135
#define ADC_Bit_Resolution 10   // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6  //RS / R0 = 3.6 ppm
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

#define led_R 5
#define led_G 6
#define led_B 7
#define pin_buzzer 8
#define pin_relay1 2
#define pin_relay2 3
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

Servo servo;
const int servoPin = 9;

int ac, ventilasi, venServo, led, buzzer;

void setup() {
  Serial.begin(9600);
  MQ135.setRegressionMethod(1);
  MQ135.init();
  dht.begin();

  pinMode(pin_buzzer, OUTPUT);
  pinMode(pin_relay1, OUTPUT);
  pinMode(pin_relay2, OUTPUT);
  pinMode(led_R, OUTPUT);
  pinMode(led_G, OUTPUT);
  pinMode(led_B, OUTPUT);

  servo.attach(servoPin);
  servo.write(0);

  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
  }
  MQ135.setR0(calcR0 / 10);

  if (isinf(calcR0)) {
    while (1)
      ;
  }
  if (calcR0 == 0) {
    while (1)
      ;
  }
}

void loop() {
  MQ135.update();
  MQ135.setA(605.18);
  MQ135.setB(-3.937);
  float ppm = MQ135.readSensor();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println(String(t, 2) + ";" + String(h, 2) + ";" + String(ppm, 2));


  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');

    while (Serial.available() > 0) {
      Serial.read();
    }

    int firstSemicolon = inputString.indexOf(';');
    int secondSemicolon = inputString.indexOf(';', firstSemicolon + 1);
    int thirdSemicolon = inputString.indexOf(';', secondSemicolon + 1);
    int fourSemicolon = inputString.indexOf(';', thirdSemicolon + 1);
    int fiveSemicolon = inputString.indexOf(';', fourSemicolon + 1);

    String ac_str = inputString.substring(0, firstSemicolon);
    String ventilasi_str = inputString.substring(firstSemicolon + 1, secondSemicolon);
    String venServo_str = inputString.substring(secondSemicolon + 1, thirdSemicolon);
    String led_str = inputString.substring(thirdSemicolon + 1, fourSemicolon);
    String buzzer_str = inputString.substring(fourSemicolon + 1, fiveSemicolon);

    ac_str.trim();
    ventilasi_str.trim();
    venServo_str.trim();
    led_str.trim();
    buzzer_str.trim();

    ac = ac_str.toInt();
    ventilasi = ventilasi_str.toInt();
    venServo = venServo_str.toInt();
    led = led_str.toInt();
    buzzer = buzzer_str.toInt();

    (ac == 1) ? digitalWrite(pin_relay1, LOW) : digitalWrite(pin_relay1, HIGH);
    (ventilasi == 1) ? digitalWrite(pin_relay2, LOW) : digitalWrite(pin_relay2, HIGH);
    (buzzer == 1) ? digitalWrite(pin_buzzer, HIGH) : digitalWrite(pin_buzzer, LOW);

    if(led == 0 )setRGB(0, 1, 0);
    else if (led == 1) setRGB(0, 0, 1);
    else if (led == 2) setRGB(1, 0, 0);

    servo.write(venServo);
  }

  delay(500);
}

void setRGB(int red, int green, int blue) {
  digitalWrite(led_R, red);
  digitalWrite(led_G, green);
  digitalWrite(led_B, blue);
}
