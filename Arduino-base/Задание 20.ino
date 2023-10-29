
#include <LiquidCrystal.h>
#include <TroykaMQ.h>
// имя для пина, к которому подключен датчик
#define PIN_MQ7         A0
// имя для пина, к которому подключен нагреватель датчика
#define PIN_MQ7_HEATER  13
// Датчик углекислого газа 
MQ7 mq7(PIN_MQ7, PIN_MQ7_HEATER);
// Имя пинов дисплея
constexpr uint8_t PIN_RS = 6;
constexpr uint8_t PIN_EN = 7;
constexpr uint8_t PIN_DB4 = 8;
constexpr uint8_t PIN_DB5 = 9;
constexpr uint8_t PIN_DB6 = 10;
constexpr uint8_t PIN_DB7 = 11;
// Переменная с значениями датчика газа
int gasLevel = 0;
//Мин для зелёного светодиода
const int greenLedPin = 1;
// Пин для желтого светодиода
const int yellowLedPin = 2;
// Пин для красного светодиода
const int redLedPin = 3;
// Пин для пищалки
const int buzzerPin = 5;
// Пороговые значения уровня газа
const int medium = 50; 
const int high = 100; 

//Привязываем пины к обьекту экрана
LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);
 
void setup() {
  // Инициализация LCD
  lcd.begin(16, 2);
  lcd.clear();
  
  // Установка пинов светодиодов и бузера в режим OUTPUT
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  // запускаем термоцикл
  mq7.cycleHeat();
}

void loop() { 
  // если прошёл интервал нагрева датчика
  // и калибровка не была совершена
  if (!mq7.isCalibrated() && mq7.atHeatCycleEnd()) {
    // выполняем калибровку датчика на чистом воздухе
    mq7.calibrate();
    // запускаем термоцикл
    mq7.cycleHeat();
  }
  // если прошёл интервал нагрева датчика
  // и калибровка была совершена
  if (mq7.isCalibrated() && mq7.atHeatCycleEnd()) {
    // выводим значения газов в ppm
    gasLevel = mq7.readCarbonMonoxide();
    delay(100);
    // запускаем термоцикл
    mq7.cycleHeat();
  }
  
  // Проверка уровня газа и управление светодиодами и бузером
  if (gasLevel <= medium) {
    // Нормальный уровень газа
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    noTone(buzzerPin);
    lcd.setCursor(0, 1);
    lcd.print("OK ");
  } else if (gasLevel <= high) {
    // Некритическое превышение порогового уровня газа
    digitalWrite(greenLedPin, LOW);
    digitalWrite(yellowLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Control");
    tone(buzzerPin, 1000);
    delay(2000);
    noTone(buzzerPin);
    delay(10000);

  } else {
    // Критическое превышение порогового уровня газа
    digitalWrite(greenLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    tone(buzzerPin, 2000);
    lcd.setCursor(0, 1);
    lcd.print("Critical");
    tone(buzzerPin, 1000);
    delay(1000);
    tone(buzzerPin, 0);
  }
  
  delay(1000); // Пауза между измерениями
}