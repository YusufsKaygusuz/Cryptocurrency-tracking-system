#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

int rxpin = 8;
int txpin = 9;
SoftwareSerial esp(rxpin,txpin);

LiquidCrystal lcd(12,11,5,4,3,2);
const int arka_isik=1;

 String agAdi= "*********";
 String agSifresi= "***********";
 String ip = "184.106.153.149";

 byte ethereum [8] =
 {
  B00000,
  B00100,
  B01010,
  B10001,
  B10001,
  B01010,
  B00100,
  B00000,
 };

void setup()
{
  pinMode(arka_isik, OUTPUT);
  analogWrite(arka_isik, 1023);
  lcd.begin(16,2);

  Serial.begin(9600);
  Serial.println("ESP8266 is being started");
  
  esp.begin(115200);
  esp.println("AT");
  Serial.println("AT massage was sent.");
  delay(10);

  while(!esp.find("OK"))
  {
    esp.println("AT");
    Serial.println("Esp hasn't found. We're trying to solve the problems.");
  }
  Serial.println("OK command received.");

  esp.println("AT+CWMODE=1");
  while(!esp.find("OK")){
    esp.println("AT+CWMODE=1");
    Serial.println("Setting are configuring...");
    delay(50);
  }

  Serial.println("Esp's setup was done.");
  Serial.println("it is connecting the network.");

  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");
  delay(10);
  while(!esp.find("OK"));
  Serial.println("Connection is OK.");
  delay(1000);
}
  void loop()
  {
    esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");
    if(esp.find("EROR"))
    {
      Serial.println("AT+CIPSTART Error");
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Live Ethereum:");

    lcd.createChar(1, ethereum);
    lcd.setCursor(2,1);
    lcd.write(1);

    lcd.setCursor(4,1);
    lcd.print(demonstrater());
    lcd.setCursor(12,1);
    lcd.print("$");
    Serial.println("Connection was stopped");
    esp.println("AT+CIPCLOSE");
    delay(5000);

  }

  String demonstrater()
  {
    String rest ="AT+CIPSEND=90";
    rest += "\r\n";
    sendData(rest, 2000,0);

    String host = "GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=DA2QMVBTVXHFTH9N";
    host += "\r\n";
    host += "Host:api.thingspeak.com";
    host += "\r\n\r\n\r\n\r\n\r\n";
    String Shower = sendData(host, 2000 ,1);

    int baslangic = Shower.indexOf(':');
    Shower = Shower.substring(baslangic+1, baslangic+9);
    return(Shower);
  }

  String sendData(String komut, const int zamangecen, boolean debug)
{
  String gelenveri = ""; // Sonradan gelecek verileri alabilmemiz icin baslangcta ici bos bir string olusturduk. 
  esp.print(komut); // komut satırını espnin çıkışına yazdırıyoruz.

  long int Zaman = millis();
  while ( (Zaman + zamangecen) > millis())
  {
    while (esp.available())
    {
      // esp'nin bağlantısından gelen veri var ise
      char aradeger = esp.read(); // her bir karakteri okuyup aradeger adlı char degiskenine aktarıyoruz.
      gelenveri += aradeger; // Char formatında toplanan her veriyi, 
                             //'gelenveri' adresinin sonuna ekliyor ve alacağımız teksti oluşturuyoruz.
    }
  }
  if (debug)
  {
    Serial.println(gelenveri);
  }
  return gelenveri;
}
