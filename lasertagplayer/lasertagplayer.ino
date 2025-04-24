#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define chest_1 36 
#define chest_2 39
#define chest_3 34
#define chest_4 35
#define Calibrated 4
#define trigger 13
#define laser 14
const int chest[] = {chest_1, chest_2, chest_3, chest_4};
bool shot = false;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int life = 10;
int ammo = 10;

 byte heart[8] = {
  B00000,
  B00000,
  B11011,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};

// Custom bullet character
byte bullet[8] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000
};

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(chest_1,INPUT);
  pinMode(chest_2,INPUT);
  pinMode(chest_3,INPUT);
  pinMode(chest_4,INPUT);
  pinMode(Calibrated, OUTPUT);
  pinMode(trigger,INPUT);
  pinMode(laser,OUTPUT);
  //while(!isCalibrated());
  digitalWrite(Calibrated, HIGH);
  delay(1000);

  lcd.createChar(0, heart);
  lcd.createChar(1, bullet); 
  updateLCD();
}

void loop() {
  //shooting code
  char trig1 = digitalRead(trigger);
  delay(25);
  char trig2 = digitalRead(trigger);
  if ((trig2 == HIGH)&&(trig1==LOW)) {  // Button pressed (assuming pull-up)
    if (ammo > 0){
     ammo--;
     digitalWrite(laser,HIGH);
    }
    updateLCD();
    delay(300);
    digitalWrite(laser,LOW);  // Debounce
  }
  //shot code
  isShot();
  if(shot){
    life--;
    if(life==0){
      //endgame logic
    }
    else{
      shot = false;
      updateLCD();
    }
    delay(300);
  }
}

void updateLCD() {
  //Serial.println(ammo);

  lcd.setCursor(0, 0);
  lcd.print("ammo: ");
  lcd.setCursor(6, 0);
  lcd.print("                "); // Clear previous icons
  lcd.setCursor(6, 0);
  for (int i = 0; i < ammo && i < 10; i++) lcd.write(1); // bullets

  lcd.setCursor(0, 1);
  lcd.print("life: ");
  lcd.setCursor(6, 1);
  lcd.print("                ");
  lcd.setCursor(6, 1);
  for (int i = 0; i < life && i < 10; i++) lcd.write(0); // hearts
}
bool isCalibrated(){
  int data = analogRead(chest_1);
  //Serial.println(data);
  return data>=3000;
}
void isShot(){
 delay(50);
  double data[] = {0,0,0,0};
  for(int i = 0; i<4;i++){
    data[i] = (double)analogRead(chest[i])/4096;
  }
  int index1 = 0;
  double min1 = 1;
  int index2 = 0;
  double min2 = 1; 
  for(int i = 0; i<4; i++){
    if(data[i]<min1){
      min2 = min1;
      min1 = data[i];
    }
    else if(data[i]>min1&&data[i]<min2){
      min2 = data[i];
    }
  }
  if(min1<0.8*min2)
    shot=true;
  else
    Serial.println("no");
}
