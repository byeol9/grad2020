#include <LiquidCrystal_I2C.h>

#include <TinyGPS.h>
#include <Wire.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

TinyGPS tinygps;
SoftwareSerial gps(6, 7);
long lat, lon;

int tx = 2;
int rx = 3;
SoftwareSerial BT(tx, rx);

int count = 0;
int s = 0;

int tmp36 = A0;
int sensor = 0;
float celsius = 0;

unsigned char c = 0;

String gps_num, h, t;

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  gps.begin(9600);
  Wire.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {
  if (s == 0)
  {
    delay(1000);
    Wire.requestFrom(0xA0 >> 1, 1);
    if (Wire.available())
    {
      get_temp();
      c = Wire.read();
      if (c < 50 || c > 120 || celsius >= 45 || celsius < 30)
      {
        count++;
        Serial.print("심박수: ");
        Serial.println(c, DEC);
        Serial.print("온도: ");
        Serial.println(celsius);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("STATUS");
        lcd.setCursor(0, 1);
        lcd.print(c);
        lcd.setCursor(7, 1);
        lcd.print(celsius);
      }
      else
      {
        count = 0;
        Serial.print("심박수: ");
        Serial.println(c, DEC);
        Serial.print("온도: ");
        Serial.println(celsius);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("STATUS");
        lcd.setCursor(0, 1);
        lcd.print(c);
        lcd.setCursor(7, 1);
        lcd.print(celsius);

        if(0<=c<50)
        {
          h="너무 낮음";
        }
        else if(c>120)
        {
          h="너무 높음";
        }
        else
        {
          h="정상";
        }

        if(celsius<30)
        {
          t="너무 낮음";
        }
        else if(celsius>=45)
        {
          t="너무 높음";
        }
        else
        {
          t="정상";
        }
      }
    }
    if (count >= 10)
    {
      s = 1;
    }
  }

  else if (s == 1)
  { 
    get_gps();
    Serial.println("HELP");
    Serial.print("심박수: ");
    Serial.println(c, DEC);
    Serial.print("온도: ");
    Serial.println(celsius);

    lcd.clear( );
    lcd.setCursor(0, 0);
    lcd.print("HELP");
    lcd.setCursor(0, 1);
    lcd.print(c);
    lcd.setCursor(7, 1);
    lcd.print(celsius);

    String one="이상이 있습니다";
    String two="\n";
    String three="심박수: ";
    String four="체온: ";

    String str=one+two+three+h+two+four+t+two+gps_num;

    Serial.println(str);
    BT.println(str);

    count = 0;
    s = 2;
  }

  else if (s == 2)
  {
    delay(1000);
    Wire.requestFrom(0xA0 >> 1, 1);
    if (Wire.available())
    {
      get_temp();
      c = Wire.read();
      if (c > 50 && c < 150 && celsius <= 45)
      {
        count++;
        Serial.print("심박수: ");
        Serial.println(c, DEC);
        Serial.print("온도: ");
        Serial.println(celsius);

        lcd.clear( );
        lcd.setCursor(0, 0);
        lcd.print("HELP");
        lcd.setCursor(0, 1);
        lcd.print(c);
        lcd.setCursor(7, 1);
        lcd.print(celsius);
      }
      else
      {
        count = 0;
        Serial.println("HELP");
        Serial.print("심박수: ");
        Serial.println(c, DEC);
        Serial.print("온도: ");
        Serial.println(celsius);

        lcd.clear( );
        lcd.setCursor(0, 0);
        lcd.print("HELP");
        lcd.setCursor(0, 1);
        lcd.print(c);
        lcd.setCursor(7, 1);
        lcd.print(celsius);
      }
    }
    if (count >= 5)
    {
      s = 0;
    }
  }
}

void get_temp()
{
  sensor = analogRead(tmp36);

  float voltage = sensor * 5000.0 / 1024.0;
  celsius = (voltage - 500) / 10.0;
}

void get_gps()
{
  if (gps.available())
  {
    if (tinygps.encode(gps.read()))
    {
      tinygps.get_position(&lat, &lon);
      Serial.print("lat: ");
      Serial.print(lat);
      Serial.print(" ");
      Serial.print("lon: ");
      Serial.println(lon);

      int d1=(int(lat));
      int m1=(int((lat-d1)*60));
      int s1=((lat-d1)*60-m1)*60;

      int d2=int(lon);
      int m2=int((lon-d1)*60);
      int s2=((lon-d1)*60-m1)*60;

      gps_num=String(d1)+"º"+String(m1)+"′"+String(s1)+"″N+"+String(d1)+"º"+String(m1)+"′"+String(s1)+"″E ";
    }
  }
  if(!gps.available())
  {
    Serial.print("lat: 36.6274133");
    Serial.print(" ");
    Serial.println("lon: 127.4546372");
      
    gps_num="google.com/maps/place/36º37′41.5″N+127°27'33.9″E 또는 알 수 없는 실내에 있습니다.";
  }
}
