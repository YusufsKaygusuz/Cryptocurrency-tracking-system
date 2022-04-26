#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

int rxpin = 8;
int txpin = 9;
SoftwareSerial esp(rxpin,txpin);

LiquidCrystal lcd(12,11,5,4,3,2);
const int backlamb=1;

String AgAdi = "********";
String AgSifresi = "*************";
String ip = "184.106.153.149";

byte chainlink[8] =
{
  B00100,
  B01010,
  B10001,
  B10001,
  B10001,
  B01010,
  B00100,
  B00000,
};


void setup() {
  pinMode(backlamb, OUTPUT);
  lcd.begin(16,2);
  
  Serial.begin(9600);
  Serial.println("ESP8266 was Started.");

  esp.begin(115200);
  esp.println("AT");
  Serial.println("AT message was sent.");
  
  while(!esp.find("OK"))
  {
    esp.println("AT");
    Serial.println("Esp8266 hasn't found. We're trying to solve the problems.");
  }

  Serial.println("OK command received."); 

   esp.println("AT+CWMODE=1");
   
   while(!esp.find("OK")){
   esp.println("AT+CWMODE=1");
   Serial.println("Settings are configuring...");
}
 
  Serial.println("Modül Client olarak ayarlandı.");
  Serial.println("Aga baglaniliyor...");

  esp.println("AT+CWJAP=\""+AgAdi+"\",\""+AgSifresi+"\"");
  while(!esp.find("OK"));
  Serial.println("Aga Baglandi");
  delay(1000);
}

void loop() {  
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");
  
  if(esp.find("EROR"))
  {
    Serial.println("AT+CIPSTART Error");
  }
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Live Link Price:");
  
  lcd.createChar(1, chainlink);// Chainlink Simgesi
  lcd.setCursor(2,1);
  lcd.write(1);
  
  lcd.setCursor(4,1);
  lcd.print(demonstrater());
  lcd.setCursor(9,1);
  lcd.print("$");
  Serial.println("Connection was stopped");
  esp.println("AT+CIPCLOSE");
  delay(5000);
}

String demonstrater()
{
  String rest = "AT+CIPSEND=90"; //Gönderilecek Karakter Sayisi 90. ("AT+CIPSEND=90")
  rest += "\r\n";
  sendData(rest, 2000, 0);
  
  String host = "GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=B4RJ5VQE56PBVQ32";
  host += "\r\n";
  host += "Host:api.thingspeak.com";
  host += "\r\n\r\n\r\n\r\n\r\n";
  String Shower = sendData(host, 2000, 1);  // GET request ( GET /apps/thinghttp/send_request?api_key=XXXXXXXXXXXXXXXX 
                                            //               Host: Host_server_name ) 

// gelen verinin içinden sadece ilgili bölümü alıyoruz. 
 int baslangic=Shower.indexOf(':');
 Shower=Shower.substring(baslangic+1,baslangic+6);
 return (Shower);
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
