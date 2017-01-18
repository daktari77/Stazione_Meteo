// INSERIMENTO LIBRERIE
#include <SD.h>
//#include <Dhcp.h>
//#include <Dns.h>
//#include <Ethernet.h>
//#include <EthernetClient.h>
//#include <EthernetServer.h>
//#include <EthernetUdp.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>


// DEFINIZIONE PIN
#define DHTPIN 2     // what digital pin we're connected to
//#define SOUNDPIN 3  // PIN MIC

const int CS = 4; //CHIP SELECT PER SD
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // for FEATURE_LCD_YDv1; set the LCD I2C address needed for LCM2004 IIC
char buffer[50];
char lcdBuffer[16];

// DEFINIZIONE TIPI SENSORI
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


DHT dht(DHTPIN, DHTTYPE); //INIZIALIZZAZIONE SENSORE DHT
RTC_DS1307 RTC;   //INIZIALIZZAZIONE RTC

void setup() {
  Serial.begin(57600);  //INIZIALIZZAZIONE SERIALE
  Serial.print("Initializing SD card...");
  pinMode( CS, OUTPUT); //  pinMode (SOUNDPIN, INPUT) ; // input from the Sound Detection Module
  dht.begin();
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__)); //ASSEGNA DATA E ORA CORRENTE
  
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
    lcd.print("DataLogger LCD");
    lcd.setCursor(0,1);
    lcd.print("Michele M.");
    delay( 1000 );
    lcd.clear();  
    lcd.setCursor(0,0);
    lcd.print("Init SD Card...");
    lcd.setCursor(0,1);
    
// VERIFICA INIZIALIZZAZIONE PIN SD 
    if (!SD.begin(CS)) {
      Serial.println("initialization failed!");
      lcd.print("... Failed   ");
      return;
    }
    Serial.println("initialization done.");
    lcd.print("... Ready    ");
 
    delay( 1000 );
 }

void loop() {
  // Wait a few seconds between measurements.
  delay(1000);
  
  DateTime now = RTC.now();
    
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);


  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F ");

    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Umid:");
    lcd.setCursor(9,0);
    lcd.print(h);
    lcd.setCursor(15,0);
    lcd.print("%");
    lcd.setCursor(0,1);
    lcd.print("Temp:");
    lcd.setCursor(9,1);
    lcd.print(t);
    lcd.setCursor(15,1);
    lcd.print("C");

// CREO DELLE STRINGHE CONTENENTI I VALORI DA LOGGARE SU SD    
    String YY = String(now.year(), DEC);
    String MM = String(now.month(), DEC);
    String DD = String(now.day(), DEC);
    String hh = String(now.hour(), DEC);
    String mm = String(now.minute(), DEC);
    String humy = String(dht.readHumidity());
    String temp = String(dht.readTemperature());
    
    String tutto = YY + ';' + MM + ';' +  DD + ';' +  hh + ';' +  mm + ';' +  humy + ';' +  temp;
    
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  // se il file è disponibile, allora scrivi:

if (now.minute() == 0 && now.second() == 0 || now.minute() == 15 && now.second() == 0 || now.minute() == 30 && now.second() == 0 || now.minute() == 45 && now.second() == 0){
  if (dataFile) {
    dataFile.println(tutto);
    dataFile.close();
    // print to the serial port too:
    Serial.println(tutto);
  }
  // se il file non è disponibile allora errore:
  else {
    Serial.println("Errore in dataFile.csv");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Error dataFile");
  }
}
}
