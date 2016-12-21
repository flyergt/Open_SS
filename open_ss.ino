

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // библиотека дисплея, подключение по I2C
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args) write(args);
#else
#define printByte(args) print(args,BYTE);
#endif

int pinSolderOut = 2;  // Выход для паяльника
int pinSolderIn = A2;  // Потенциометр паяльника
int pinSolderTCouple = A0;  // Термопара паяльника
int pinSolderButton = 5; // Кнопка вкл.и выкл. паяльника
int pinHotAirOut = 13;  // Выход для фена
int pinHotAirIn = A1;  // Потенциометр фена
int pinHotAirTCouple = A6;  // Термопара фена
int pinHotAirCoolerOut = 3;  // Выход для вентилятора фена ( PWM )
int pinHotAirCoolerIn = A3;  // Потенциометр вентилятора фена
int pinHotAirButton = 4;  // Кнопка вкл.и выкл. фена
int pinCooler = 11; // Выход кулера

uint8_t char_cel[8] = {
  B00111, B00101, B00111, B00000, B00000, B00000, B00000};
LiquidCrystal_I2C lcd(0x27,16,2); // Адрес LCD 0x27. Количество символов 16. Количество строк 2.

void setup()

{
  TCCR2B = TCCR2B & 0b11111000 | 0x01; // Частота ШИМ 11 и 3
  pinMode(pinSolderOut, OUTPUT);
  pinMode(pinSolderButton, INPUT);
  pinMode(pinHotAirOut, OUTPUT);
  pinMode(pinHotAirButton, INPUT);
  lcd.init();                      // инициализация LCD 
  lcd.backlight();                 // включаем подсветку
  lcd.clear();                     // очистка дисплея
  lcd.createChar(1, char_cel);
  lcd.home();
  // Вывод приветствия
  lcd.setCursor(0, 0);
  lcd.print("Open_SS  v.1.0");
  lcd.setCursor(0, 1);
  analogWrite(pinHotAirCoolerOut, 255);
  //lcd.setCursor(14, 1);
  //lcd.print("HF");
  delay (3000);
  analogWrite(pinHotAirCoolerOut, 0);
  analogWrite(pinCooler, 255);
  //lcd.setCursor(14, 1);
  //lcd.print("CF");
  delay (3000);
  analogWrite(pinCooler, 0);
  lcd.clear();

}

void loop()

{

  // Преобразовываем значения
  //=============================================================================
  int setSolderTemp = map(analogRead(pinSolderIn), 0, 1023, 0, 480);           // Sol pot
  int solderTCouple = map(analogRead(pinSolderTCouple), 0, 750, 0, 480);       // Sol T
  int setHotAirTemp = map(analogRead(pinHotAirIn), 0, 1023, 0, 480);           // Air pot
  int hotAirTCouple = map(analogRead(pinHotAirTCouple), 0, 750, 0, 480);       // Air T
  int setHotAirCooler = map(analogRead(pinHotAirCoolerIn), 0, 1023, 130, 255); // Fan pot
  int displayHotAirCooler = map(analogRead(pinHotAirCoolerIn), 0, 1023, 0, 99);// Fan disp
  //=============================================================================





  // Защита, если не работает термопара
  // =======================================================================
  if (solderTCouple > 480)                                               
  {
    setSolderTemp = 0;
  }                                                                      
  if (hotAirTCouple > 480)                                              
  {
    setHotAirTemp = 0;
  }                                                                    
  // =======================================================================





  // Поддержка установленной температуры паяльника
  //=====================================================================================
  if (setSolderTemp >= solderTCouple && digitalRead(pinSolderButton) == HIGH)          
  {                                                                                    
    digitalWrite(pinSolderOut, LOW);                                                   
    delay(20);                                                                         
    digitalWrite(pinSolderOut, HIGH);                                                   
  }                                                                                    
  else                                                                                 
  {
    digitalWrite(pinSolderOut, LOW);
  }                                                                                   
  //=====================================================================================





  // Поддержка установленной температуры фена
  //=============================================================================
  if (setHotAirTemp >= hotAirTCouple && digitalRead(pinHotAirButton) == HIGH)  
  {                                                                             
    digitalWrite(pinHotAirOut, HIGH);                                         
    delay(20);                                                                 
    digitalWrite(pinHotAirOut, LOW);                                         
  }                                                                            
  else                                                                        
  {
    digitalWrite(pinHotAirOut, LOW);
  }                                                                           
  //=============================================================================





  // Установка оборотов вентилятора фена
  //=====================================================================
  if (hotAirTCouple < 65 && digitalRead(pinHotAirButton) == LOW)      
  {
    analogWrite(pinHotAirCoolerOut, 0);
  }                                                                    
  else                                                                  
  {
    analogWrite(pinHotAirCoolerOut, setHotAirCooler);
  }                                                                    
  //=====================================================================




  // Убираем прыганье цифр
  //=====================================================================================
  if ((setSolderTemp + 10) > solderTCouple && (setSolderTemp - 10) < solderTCouple)
  {
    solderTCouple = setSolderTemp;
  }
  if ((setHotAirTemp + 10) > hotAirTCouple && (setHotAirTemp - 10) < hotAirTCouple)
  {
    hotAirTCouple = setHotAirTemp;
  }
  //=====================================================================================






  // Данные паяльника на дисплей
  //=====================================================================================

  if (digitalRead(pinSolderButton) == HIGH)
  {
    analogWrite(pinCooler, 255);
    lcd.setCursor(0, 0);
    lcd.print("Solder:");
    lcd.setCursor(7, 0);
    if (solderTCouple < 480)
    {
      if (setSolderTemp < 10)
      {
        lcd.print(setSolderTemp);
        lcd.print("\1");
        lcd.print("   ");
      }
      if (setSolderTemp < 100)
      {
        lcd.print(setSolderTemp);
        lcd.print("\1");
        lcd.print("  ");
      }
      if (setSolderTemp > 99)
      {
        lcd.print(setSolderTemp);
        lcd.print("\1");
        lcd.print(" ");
      }
      lcd.setCursor(12, 0);
      if (solderTCouple < 10)
      {
        lcd.print(solderTCouple);
        lcd.print("\1");
        lcd.print("   ");
      }
      if (solderTCouple < 100)
      {
        lcd.print(solderTCouple);
        lcd.print("\1");
        lcd.print("  ");
      }
      if (solderTCouple > 99)
      {
        lcd.print(solderTCouple);
        lcd.print("\1");
        lcd.print(" ");
      }
    }
    else 
    {
      lcd.setCursor(7, 0);
      lcd.print(" Error   ");
    }
  }
  else 
  {
    //lcd.setCursor(7, 0);
    //lcd.print(" Off     ");
    lcd.setCursor(0, 0);
    lcd.print("Solder: off     ");
  }
  //=======================================================================================






  // Данные фена на дисплей
  //================================================================

  if (digitalRead(pinHotAirButton) == HIGH)
  {
    analogWrite(pinCooler, 255);
    lcd.setCursor(0, 1);
    lcd.print("Air:");
    lcd.setCursor(4, 1);
    if (hotAirTCouple < 480)
    {
      if (setHotAirTemp < 10)
      {
      lcd.print(setHotAirTemp);
      lcd.print("\1");
      lcd.print("   ");
      }
      if (setHotAirTemp < 100)
      {
      lcd.print(setHotAirTemp);
      lcd.print("\1");
      lcd.print("  ");
      }
      if (setHotAirTemp > 99)
      {
      lcd.print(setHotAirTemp);
      lcd.print("\1");
      lcd.print(" ");
      }
      lcd.setCursor(9, 1);
      if (hotAirTCouple < 10)
      {
      lcd.print(hotAirTCouple);
      lcd.print("\1");
      lcd.print("   ");
      }
      if (hotAirTCouple < 100)
      {
      lcd.print(hotAirTCouple);
      lcd.print("\1");
      lcd.print("  ");
      }
      if (hotAirTCouple > 99)
      {
      lcd.print(hotAirTCouple);
      lcd.print("\1");
      lcd.print(" ");
      }
      // Вентилятор фена на дисплей
      lcd.setCursor(14, 1);
      lcd.print(displayHotAirCooler);
      if (displayHotAirCooler < 10)
      {
      lcd.print(" ");
      }
    }
    else {
      lcd.setCursor(4, 1);
      lcd.print(" Error");
      lcd.print("      ");
    }
  }
  else {
    //lcd.setCursor(4, 1);
    //lcd.print(" Off");
    //lcd.print("        ");
    lcd.setCursor(0, 1);
    lcd.print("Air: off        ");
  }
  //==============================================================
  
  
  
  
  
  
  
  
  
  // Отключение кулера корпуса
  //==============================================================
  if (digitalRead(pinHotAirButton) == LOW && digitalRead(pinSolderButton) == LOW)
  {
    analogWrite(pinCooler, 0);
  }
}




