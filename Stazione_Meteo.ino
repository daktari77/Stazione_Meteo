// INSERIMENTO LIBRERIE
#include <SD.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#define CAYENNE_PRINT Serial        // Comment this out to disable prints and save space
#include <CayenneEthernet.h>        // Comment this out if you uncomment a different Ethernet device below.
// #include <CayenneEthernetW5200.h> // Uncomment this and comment out CayenneEthernet.h to use an Ethernet W5200 shield.
#include "pin.h"
#include "Varibili_e_costanti.h"
#include "Sensors.h"
#include <TimeAlarms.h>
//#include <TimeLib.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // for FEATURE_LCD_YDv1; set the LCD I2C address needed for LCM2004 IIC

DHT dht(DHTPIN, DHTTYPE); //INIZIALIZZAZIONE SENSORE DHT
RTC_DS1307 RTC;   //da ora in avanti RTC_DS1307 sarà RTC


// Mac address should be different for each device in your LAN
byte arduino_mac[] = { 0xFE, 0xA8, 0xE0, 0xA1, 0x6A, 0xDC };
IPAddress arduino_ip(17, 6, 77, 102);
IPAddress dns_ip(8, 8, 8, 8);
IPAddress gateway_ip(17, 6, 77, 1);
IPAddress subnet_mask(255, 255, 255, 0);


void setup() {
  dht.begin();
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__)); //ASSEGNA DATA E ORA CORRENTE

  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  
  Serial.print("Initializing SD card...");

  setTime(20,54,00,21,1,17);

  Alarm.timerRepeat(15, log_sd);
  
 
  Cayenne.begin(token, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac); //INIZIALIZZAZIONE CAYENNE

 
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("DataLogger LCD");
  lcd.setCursor(0, 1);
  lcd.print("Michele M.");
  delay( 1000 );
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Init SD Card...");
  lcd.setCursor(0, 1);


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

void loop()
{
  // Wait a few seconds between measurements.
  Alarm.delay(1000);
  delay(1000);
  Cayenne.run();  //START CAYENNE
  DateTime now = RTC.now();
  outputValue = analogRead(photocellPin);

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

  //CAYENNE_LOG("Data requested for Virtual Pin %d", VIRTUAL_PIN_HUMI);
  Cayenne.virtualWrite(VIRTUAL_PIN_HUMI, h);
  //CAYENNE_LOG("Data requested for Virtual Pin %d", VIRTUAL_PIN_TEMP);
  Cayenne.virtualWrite(VIRTUAL_PIN_TEMP, t);
  //  CAYENNE_LOG("Data requested for Virtual Pin %d", VIRTUAL_PIN_HEAT);
  //  Cayenne.virtualWrite(VIRTUAL_PIN_HEAT, hif);

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
  Serial.print("Fotoresistore: ");
  Serial.println(outputValue);  //STAMPA VALORE LETTO DA FOTORESISTORE


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Umid:");
  lcd.setCursor(9, 0);
  lcd.print(h);
  lcd.setCursor(15, 0);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.setCursor(9, 1);
  lcd.print(t);
  lcd.setCursor(15, 1);
  lcd.print("C");
  
// CREO DELLE STRINGHE CONTENENTI I VALORI DA LOGGARE SU SD
  String YY = String(now.year(), DEC);
  String MM = String(now.month(), DEC);
  String DD = String(now.day(), DEC);
  String hh = String(now.hour(), DEC);
  String ss = String(now.second(), DEC);
  String mm = String(now.minute(), DEC);
  String humy = String(dht.readHumidity());
  String temp = String(dht.readTemperature());
  
  tutto = DD + '/' + MM + '/' +  YY + ';' +  hh + ':' +  mm + ':' + ss + ';' +  humy + ';' +  temp;  //Stringa dichiarata in Variabili_e_costanti.h

  }

void log_sd()
{
//  // open the file. note that only one file can be open at a time,
//  // so you have to close this one before opening another.
//  File dataFile = SD.open("datalog.csv", FILE_WRITE);

//  // se il file è disponibile, allora scrivi:
//  if (dataFile) {
//    dataFile.println(tutto);
//    dataFile.close();
//    // print to the serial port too:
//    Serial.println(tutto);
//  }
//  // se il file non è disponibile allora errore:
//  else {
//    Serial.println("Errore in dataFile.csv");
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("Error dataFile");
  Serial.println("***************log_sd*******************");
//  }
}
