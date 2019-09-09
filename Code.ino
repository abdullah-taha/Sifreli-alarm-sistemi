#include<LiquidCrystal.h>
#include <Keypad.h>

#define trigPin 12
#define echoPin 11
char tusMap[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte satirPinleri[4] = {A0,A1,A2,A3}; 
byte sutunPinleri[4] = {A4,A5,8,9}; 


String sifre = "1234";  // varsaylan sifre
String tempSifre = "";
int k = 0;
char tus;
String A = "A-Alarmi etkinlestir  " ;
String B = "B-sifreyi degistir    ";
int len = A.length()-16+1;
int len2 = B.length()-16+1;
int mode = 0;
boolean alarm = false;
int baslangicMesafe, yeniMesafe;
long mesafe, sure;
String data;
Keypad kp = Keypad(makeKeymap(tusMap), satirPinleri, sutunPinleri, 4, 4); 
LiquidCrystal lcd(2,3,4,5,6,7);



void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  delay(1000);
  lcd.print(A);
  lcd.setCursor(0,1);
  lcd.print(B);
  delay(1000);
  pinMode(10, OUTPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
}

/*
programimiz modelerden oluşmakta.
mode 0 / ana ekran : bu modda alarmı etkinleşir ve şifreyi değiştir iki seçenek olmak üzere kullanıcıya sunuluyor.
mode 1 / alarm aktif : bu moda geçtiğinde alarm aktif hale gelir
mode 2 / şifre kontrol modu : bu modda geçtiğinde kullanıcıdan eski şifresini girilmesini istenecek. Doğru şifreyi girdiği halde mod 3e geçer
mode 3 / şifre değiştirme modu : bu modda kullanıcıdan  yeni şifre girilmesini istencek ve ana ekrana dönecek

*/
void loop() 
{
  if(mode == 0)
  {
    for(int i=0;i<A.length();i++)   //ana ekrandaki metni kaydırmak için
    {
      tus = kp.getKey();
      if(tus) 
      {
        if(tus == 'B')              // B'ye basılırsa şifreyi değiştirme işlemine geçecek ==> mode 2
        {
          tone(10, 1000, 200);  
          lcd.clear();
          lcd.print("eski sifrenizi: ");
          mode = 2;
          k = 0;
          break;
        }
        else if(tus == 'A')
        {
          tone(10, 1000, 200);  
          lcd.clear();
          digitalWrite(trigPin, LOW);
          delayMicroseconds(2);
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
          sure = pulseIn(echoPin, HIGH);
          baslangicMesafe = sure*0.034/2;
          
          lcd.print("    alarm on ");
          lcd.print(baslangicMesafe);
          mode = 1;
          break;
        }
      }

      // ana ekranı kaydırma işlemin başlangıcı
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.print(A.substring(i,i+16));
      if(i>len){lcd.setCursor(16-(i-len),0);lcd.print(A.substring(0,i));}
      lcd.setCursor(0,1);
      lcd.print(B.substring(i,i+16));
      if(i>len){lcd.setCursor(16-(i-len),1);lcd.print(B.substring(0,i));}
      delay(600);
      // ana ekranı kaydırma işlemin bitişi

    }
  }

  /*  eski şifreyi kontrol etme modu   */
  if(mode == 2)
  {
    tus = kp.getKey();
    if(tus)
    {
      tone(10, 1000, 200);  
      if(tus == '#')      // # 'ye basıldığı zaman girilen stringin doğru olup olmadığını kontrol edecek
      {
        if(tempSifre == sifre )
        {
          lcd.clear();
          lcd.print("yeni sifreniz:");
          mode = 3;
          tempSifre = "";
          k = 0;
         }
        else
        {
          lcd.clear();
          //lcd.print(sifre);
          lcd.setCursor(0,1);
          lcd.print("sifre yanlis");
        }
      }
      else
      {
        if(tus == 'C')    // C'ye basildiğı zaman girilen şifreyi temizleyecek
        {
          tempSifre = "";
          lcd.clear();
          lcd.print("eski sifreniz:");
          k = 0;
        }
        else
        {
          tempSifre += tus;
          lcd.setCursor(k,1);
          lcd.print(tus);
          delay(200);
          lcd.setCursor(k,1);
          lcd.print('*'); 
          k++;
        }
      }
    } 
  }

/*  şifre değiştirme modu   */
if(mode == 3)
{
  
  tus = kp.getKey();
  if(tus)
  {
    tone(10, 1000, 200);  
    if(tus == '#')    // # 'a basıldığı zaman girilen şifreyi yeni şifre olarak kabul edecek
    {
      sifre = tempSifre;  
      lcd.clear();
      lcd.print("sifreniz yenilendi");
      lcd.setCursor(0,1);
      //lcd.print(sifre);
      delay(2000);
      mode = 0;
      tempSifre = "";
    }

    else
    {
      if(tus == 'C')    // C'ye basıldığı zaman girilien şifreyi temizleyecek
      {
        tempSifre = "";
        lcd.clear();
        lcd.print("yeni sifreniz:");
        k = 0; 
        
      }
      else
      {
        tempSifre += tus;
        lcd.setCursor(k,1);
        lcd.print(tus);
        delay(200);
        lcd.setCursor(k,1);
        lcd.print('*');
        k++;
      }
    }  
  }
  }

 if(mode == 1)
 {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  sure = pulseIn(echoPin, HIGH);
  mesafe = (sure*0.034/2) + 1;
  if(alarm == false){lcd.setCursor(0,0);lcd.print(mesafe); } 
  delay(200);
  if ((mesafe < baslangicMesafe)  && alarm == false) 
  { 
    alarm = true;
    digitalWrite(10, HIGH);
    Serial.print('1');  
    tempSifre = "";
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("sifre:");
    k = 0;
  } 

  if(alarm == true)
  {
    while(Serial.available() > 0 )
    {
      data = Serial.readString();
      if(data == sifre )
        {
          lcd.clear();
          digitalWrite(10, LOW);
          alarm = false;
          lcd.print("    alarm off");
          delay(2000);
          lcd.clear();
          mode = 0;
         }
         else
         {lcd.clear();lcd.setCursor(0,1);lcd.print("sifre yanlis");}
    }
    lcd.setCursor(0,0);
    lcd.print("sifre:");
    tus = kp.getKey();
    if(tus)
    { 
      if(tus == '#')      // # 'ye basıldığı zaman girilen stringin doğru olup olmadığını kontrol edecek
      {
        if(tempSifre == sifre )
        {
          lcd.clear();
          digitalWrite(10, LOW);
          alarm = false;
          lcd.print("    alarm off");
          delay(2000);
          lcd.clear();
          mode = 0;
         }
        else
        {
          lcd.clear();
          lcd.print("sifre yanlis");
          delay(1500);
          tempSifre = "";
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("sifre:");
          k = 0;
        }
      }
      else
      {
        if(tus == 'C')    // C'ye basildiğı zaman girilen şifreyi temizleyecek
        {
          tempSifre = "";
          lcd.clear();
          lcd.print("sifre:");
          k = 0;
        }
        else
        {
          tempSifre += tus;
          lcd.setCursor(k,1);
          lcd.print(tus);
          delay(200);
          lcd.setCursor(k,1);
          lcd.print('*'); 
          k++;
        }
      }
    } 
  }
 }
}
