//#define REMOTEXY DEBUGLOG
// RemoteXY select connection mode and include library
#define REMOTEXY_MODE ESP8266_SOFTSERIAL_POINT
#include <SoftwareSerial.h>
#include <RemoteXY.h>
// RemoteXY connection settings
#define REMOTEXY_SERIAL_RX 2
#define REMOTEXY_SERIAL_TX 3
#define REMOTEXY_SERIAL_SPEED 9600
#define REMOTEXY_WIFI_SSID "Rain Roofing"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377
uint8_t RemoteXY_CONF[] = // 124 bytes
{ 255,0,0,14,0,117,0,16,31,1,71,56,3,7,25,25,0,2,24,135,
0,0,0,0,0,0,200,66,0,0,160,65,0,0,32,65,0,0,0,64,
24,0,71,56,34,7,26,26,0,2,24,135,0,0,0,0,0,0,200,66,
0,0,160,65,0,0,32,65,0,0,0,64,24,0,71,56,20,68,25,25,
0,2,24,135,0,0,0,0,0,0,160,65,0,0,0,64,0,0,32,65,
0,0,0,64,24,0,70,16,9,48,9,9,26,37,0,70,16,44,50,9,
9,26,37,0 };
// this structure defines all the variables and events of your control
interface
struct {
/ output variables
float temp_1; // from 0 to 100
float humidity_2; // from 0 to 100
float ph_Sensor1; // from 0 to 20
// uint8_t lRain_Sensor_1; // led state 0 .. 1
// uint8_t Moisture_Sensor_2; // led state 0 .. 1
uint8_t connect_flag; // =1 if wire connected, else =0
} RemoteXY;
#pragma pack(pop)
const int irSensorPin = 12; // Analog pin connected to the IR sensor
const int irSensorPin2 = 5;
const int RainSensor = 7;
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
int in1 = 10;
int in2 = 11;
int flag1;
int flag2;
int a;

const int soilMoisturePin = A1; // Analog pin connected to the soil
moisture sensor
int in3 = 8;
int in4 = 9;
#include <DHT.h>
#define DHT_TYPE DHT11
#define DHT_PIN 6
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define SensorPin A0
// the pH meter Analog output is
connected with the Arduino’s Analog
unsigned long int avgValue; //Store the average value of the sensor
feedback
float b;
int buf[10], temp;
float temperature;
float humidity;
int moisturePercentage ;
float phValue;
//.............................................................................................
void setup() {
RemoteXY_Init ();
lcd.init(); // initialize the lcd
lcd.init();
// Print a message to the LCD.
lcd.backlight();
Serial.begin(9600);
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);
dht.begin();
}
void loop() {
RemoteXY_Handler ();
// RainDetected_back();;
// RainDetected_Forword();
Soil_moisture();
Temp_Check();
check_possition();
ph_sensor();
lcd.setCursor(0, 0);
lcd.print("T=");
RemoteXY.temp_1=temperature;
lcd.setCursor(3, 0);
lcd.print(temperature);
lcd.setCursor(8, 0);
lcd.print("H=");
lcd.setCursor(11, 0);
RemoteXY.humidity_2=humidity;
lcd.print(humidity);
lcd.setCursor(0, 1);

lcd.print("s=");
lcd.setCursor(3, 1);
RemoteXY.ph_Sensor1=phValue;
lcd.print(moisturePercentage);
lcd.setCursor(6, 1);
lcd.print(" ");
lcd.setCursor(7, 1);
lcd.print("PH");
lcd.setCursor(11, 1);
lcd.print(phValue);
int RainSensorrValue = digitalRead(RainSensor);
if (RainSensorrValue == LOW) {
lcd.setCursor(3, 1);
//lcd.print("Rain detected");
RainDetected_Forword();
} else {
RainDetected_back();
Serial.println("NO Rain");
}
}
void check_possition() {
int irSensorValue = digitalRead(irSensorPin);
if (irSensorValue == LOW) {
a = 2;
Serial.println(a);
}
int irSensorValue2 = digitalRead(irSensorPin2);
if (irSensorValue2 == LOW) {
a = 1;
Serial.println(a);
}
}
void RainDetected_back() {
int irSensorValue = digitalRead(irSensorPin);
Serial.println(irSensorValue);
if (irSensorValue == HIGH) {
digitalWrite(in1, LOW);
digitalWrite(in2, HIGH);
Serial.println("Start");
} else {
digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
Serial.println("Stop");

// lcd.clear();
}
}
void RainDetected_Forword() {
int irSensorValue2 = digitalRead(irSensorPin2);
Serial.println(irSensorValue2);
if (irSensorValue2 == HIGH) {
digitalWrite(in1, HIGH);
digitalWrite(in2, LOW);
Serial.println("Start");
} else {
digitalWrite(in1, LOW);
digitalWrite(in2, LOW);
Serial.println("Stop");
// lcd.clear();
}
}
void Soil_moisture() {
int soilMoistureValue = analogRead(soilMoisturePin);
moisturePercentage = map(soilMoistureValue, 0, 1023, 0, 100);
Serial.print("Soil Moisture Value: ");
Serial.print(soilMoistureValue);
Serial.print(" | Moisture Percentage: ");
Serial.print(moisturePercentage);
Serial.println("%");
if (moisturePercentage >= 50) {
digitalWrite(in3, LOW);
digitalWrite(in4, HIGH);
} else {
digitalWrite(in3, LOW);
digitalWrite(in4, LOW);
}
}
void Temp_Check() {
temperature = dht.readTemperature();
humidity = dht.readHumidity();
if (isnan(temperature) || isnan(humidity)) {
Serial.println("Failed to read from DHT sensor!");
return;
}
Serial.print("Temperature: ");

Serial.print(temperature);
Serial.print(" °C | Humidity: ");
Serial.print(humidity);
Serial.println(" %");
}
void ph_sensor() {
for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor
for smooth the value
{
buf[i] = analogRead(SensorPin);
delay(10);
}
for (int i = 0; i < 9; i++) //sort the analog from small to large
{
for (int j = i + 1; j < 10; j++) {
if (buf[i] > buf[j]) {
temp = buf[i];
buf[i] = buf[j];
buf[j] = temp;
}
}
}
avgValue = 0;
for (int i = 2; i < 8; i++) //take the average value of 6 center
sample
avgValue += buf[i];
phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog
into millivolt
phValue = 3.5 * phValue;
phValue=phValue/2;
//convert the millivolt into pH
value
Serial.print(" pH:");
Serial.print(phValue, 2);
Serial.println(" ");
}