#include <MFRC522.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>


#define chest_1 36 
#define chest_2 39
#define chest_3 34
#define chest_4 35
#define back_1 32
#define back_2 33
#define trigger 13
#define laser 14
#define SCK 18
#define MOSI 23
#define MISO 19
#define RST 4
#define SS 5    // SDA

const char* ssid = "Sameh";
const char* password = "123456SR";
const int chest[] = {chest_1, chest_2, chest_3, chest_4};
bool shot = false;
bool hasShot = false;
int life = 10;
int ammo = 10;

WiFiClient client;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(SS, RST);

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
 // Serial.begin(115200);
  SPI.begin();        
  mfrc522.PCD_Init();
  WiFi.begin(ssid,password);
  lcd.init();
  lcd.createChar(0, heart);
  lcd.createChar(1, bullet);
  lcd.backlight(); 
  pinMode(chest_1,INPUT);
  pinMode(chest_2,INPUT);
  pinMode(chest_3,INPUT);
  pinMode(chest_4,INPUT);
  pinMode(back_1, INPUT);
  pinMode(back_2, INPUT);
  pinMode(trigger,INPUT);
  pinMode(laser,OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {
    delay(700);
    lcd.setCursor(0,0);
    lcd.print("CONNECTING");
    if(digitalRead(trigger) == HIGH)
    break;
  }
  if (client.connect("192.168.63.78", 1234)) {   
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(2,0);
    lcd.print("READY TO PLAY!");
    lcd.setCursor(3,1);
    lcd.print("LASER TAG!");
    client.println("HP:" + String(life) + "|AMMO:" + String(ammo));
    delay(2000);
    updateLCD();
  }
  else {
    lcd.setCursor(0,0);
    lcd.print("CONN. FAILED");
  }
}

void loop() {
  //shooting code
  //char trig1 = digitalRead(trigger);
  //delay(10);
  char trig2 = digitalRead(trigger);
  if(trig2 == HIGH && hasShot){
  isShot();
  hasShot = false;
  delay(25);
  }
  if ((trig2 == HIGH) and !hasShot) {  // Button pressed (assuming pull-down)
    if (ammo > 0){
      ammo--;
      //if (client.connected()) 
      client.println("HP:" + String(life) + "|AMMO:" + String(ammo));
      digitalWrite(laser,HIGH);
    }
    updateLCD();
    hasShot = true;
    delay(50);
    isShot();
    bool shot1 = shot;
    delay(50);
    isShot();
    bool shot2 = shot;
    delay(50);
    isShot();
    bool shot3 = shot;
    delay(50);
    isShot();
    bool shot4 = shot;
    digitalWrite(laser,LOW);
    if(shot1 || shot2 || shot3 || shot4)
    shot = true;
  }
  //shot code
  isShot();
  if(shot){
    life--;
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(4,0);
    lcd.print("OUCH!");    
    if (client.connected()) {
      client.println("HP:" + String(life) + "|AMMO:" + String(ammo));
    }    
    if(life==0){
      //endgame logic
      updateLCD();
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.print("  Game Over!");
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.print("  Game Over!");
      while(digitalRead(trigger) != HIGH)
      delay(10);
    }
    else{
      shot = false;
      delay(300);
      updateLCD();
    }
  }
  //reload code
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("Reloading...");
    delay(300);
    lcd.setCursor(0,0);
    lcd.print("Reloading....");
    delay(300);
    lcd.setCursor(0,0);
    lcd.print("Reloading.....");
    delay(300);
    lcd.setCursor(0,0);
    lcd.print("Reloading......");
    delay(300);
    ammo = 10;
    updateLCD();
  } 
  if (client.available()) {
  String msg = client.readStringUntil('\n');
  msg.trim();
  if (msg == "RESET") {
      life = 10;
      ammo = 10;
      updateLCD();
    }
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
}