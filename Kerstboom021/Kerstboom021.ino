
/* Kerstboom02.2	
** gelijk aan GPRS_MEGA10.0
** maar nu met twee pompen en de kraan wordt afzonderlijk bedient via een vlotterschakelaar en een accu
** aan de boom zitten drie druppelsensoren. Hiermee kan bepaald worden of er een sensor stuk is.
** De overige sensoren blijven hetzelfde
** versie 02 PompinGebruik geïntroduceerd.
** versie 02.1	softwarematige reset toegevoegd
**			
**
** Credit: The following example was used as a reference
** Rui Santos: http://randomnerdtutorials.wordpress.com
**
** aangepast door BDR
** datum: 20151120
** voor project ontwortelde kerstboom
** 
**
*/
#include <MemoryFree.h>
#include <SD.h>				// SDcard lib
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"			// Temp/Hum library
#include <SFE_TSL2561.h>	// lightsensor lib
#include <Streaming.h>
#include "RunningAverage.h"	// running average
#include <avr/wdt.h>		// tbv softwarematige reset

#define LOG_T_INTERVAL  60000 // Temp/Hum interval 60 sec
#define LOG_L_INTERVAL  30000  // Light interval 30 sec
#define LOG_R_INTERVAL  600000 // Raindrop interval 10 min
uint32_t LOG_LL_INTERVAL  = 0;  // SMS LaagWater interval bij start 0
uint32_t LOG_RD_INTERVAL = 0;   // SMS Raindrop interval bij start 0
uint32_t syncTimeT = 0; // time of last sync() Temp en Hum
uint32_t syncTimeL = 0; // time of last sync() Light
uint32_t syncTimeR = 0; // time of last sync() Raindrop
uint32_t syncTimeLL = 0; // time of last SMS LaagWater
uint32_t syncTimeRD = 0; // time of last SMS Raindrop

RTC_DS1307 RTC; // define the Real Time Clock object
const int chipSelect = 53; // 10 For Adafruit card, 53 for MEGA
File logfile; // the logging file
char filename[] = "LOGGER00.CSV";
boolean SDActionS = false;  // SD card failed
boolean SDActionF = false;  // File failed

boolean Connected = false;	// geeft aan of met BT connectie is gemaakt

//------------------------------------------------------------------------------
// call back for file timestamps
// Hiermee wordt de datum aan de nieuwe file gekoppeld
void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = RTC.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}
//------------------------------------------------------------------------------

DateTime now;  // now is een DataTime object

SFE_TSL2561 light; // Create an SFE_TSL2561 object, here called "light":

const int DHTPIN = 2; // gele draad aan pin digital 2
#define DHTTYPE DHT22 // de sensor van Daan
//#define DHTTYPE DHT11  // mijn sensor
//#define DHTTYPE DHT21  // weer een andere sensor
DHT dht(DHTPIN, DHTTYPE); // dht is DHT object

const int VlotterLaag = 8;      // pin 8 is alarmniveau: geeft signaal als HIGH wordt gemeten
const int Pomp1 = 9;			// pin 9 is aansturen pomp 1
const int Pomp2 = 10;			// Pin 10 is aansturen pomp 2
int Pig = Pomp1;				// Starten met PominGebruik = pomp1

const int druppel1 = 0;			// Pin A0 is uitlezen druppelsensor1
const int druppel2 = 1;			// Pin A1 is uitlezen druppelsensor2
const int druppel3 = 2;			// Pin A2 is uitlezen druppelsensor3

// Globale waarde pompregeling
int PompStatus = 0;				// toestand van de Pompstatus
int PompStatusoud = 0;			// de vorige Pompstatus
boolean sw_laagwater = true;	// begin met laagwater
boolean laagwateroud;	// vorige meting laagwater
int laagwater_delay = 10000;	// tijdsvertraging in laagwater om dender te voorkomen
uint32_t looptijdLL = 0;		// loopt tijdens het testen van de laagwatervlotter

// Globale waarde humidity en temperatuur
float h, t, f;    // humidity, temp C, temp f
char buffer[10];   // buffer voor float to char omzetting
String Hum, TempC;
char inChar = 0;

// Global variables voor Light
boolean gain = false;    // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
double lux;    // Resulting lux value
boolean good;  // True if neither sensor is saturated
String Light;

// Global variables voor Raindrop

int ra = 20;				// aantal samples voor running average
RunningAverage RA1(ra);		// create object voor running average sensor waarde 1
RunningAverage RA2(ra);
RunningAverage RA3(ra);

int Rain1;		// Uitgelezen waarde raindrop sensor 1
int Rain2;
int Rain3;
int RIG = 1;		// Rain sensor in gebruik
int Rain1Gem;	// gemiddelde waarde Rain1
int Rain2Gem;
int Rain3Gem;	

int AanlooptijdControle = 30;	// na dit aantal begint pas de controle van de sensoren. 
// TODO AanlooptijdControle van buiten instelbaar maken
int Aanlooptijd;				// lopende variabele aanlooptijdcontrole
int Teller = 0;					// aantal malen dat geteld is
int FoutTeller1 = 0;			// aantal malen dat een fout geteld is voor sensor 1
int FoutTeller2 = 0;
int FoutTeller3 = 0;

boolean Rain1Pos = false;
boolean Rain2Pos = false;
boolean Rain3Pos = false;
	
boolean Rain1_stuk = false;	// geeft aan of druppelsensor 1 stuk is
boolean Rain2_stuk = false;	// geeft aan of druppelsensor 2 stuk is
boolean Rain3_stuk = false;	// geeft aan of druppelsensor 3 stuk is

int Drooglevel1 = 50;	// boven deze waarde staat er geen of te weinig water op de sensor. Via BT aan te passen .
int Drooglevel2 = 50;
int Drooglevel3 = 50;

boolean Droog = true;			// variable om droog vast te stellen
uint32_t Droogtijd = 20000;		// tijd om druppelsensoren nat te laten worden; van buiten instelbaar
uint32_t DroogtijdLL = 0;		// loopt tijdens het testen van de Droogtijd
int Druppelspeling = 5;		// toegestane verschil rond het gemiddelde

// Global variable for SMS yes or no
char SMScode = '0';  //stuur sms bij alarmsituaties, default uit
String telefoonnummer = "";
const int Simpower = 7;  // voor de sim900 kaart Shield B-v1.1
// const int Simpower = 9;  // voor de "oude" Sim900 kaart

void setup() {
	

  pinMode(Pomp1,OUTPUT);
  digitalWrite(Pomp1,LOW);		// zet pomp1 uit. De pompen zijn active LOW
  pinMode(Pomp2,OUTPUT); 
  digitalWrite(Pomp2,LOW);		// zet pomp2 uit
  
  laagwateroud = digitalRead(VlotterLaag);	// lees de beginstand van de vlotter
  
  
  Serial1.begin(19200); // connection to GPRS network
  Serial.begin(9600);  // output via serial monitor
  // Serial3.begin(19200); // Default connection rate for Daans BT module
  Serial3.begin(9600);		// snelheid BT voor BDR. Kan opgevoerd worden.
  dht.begin();
  
  // initialiseer de Running Average
  RA1.clear();
  RA2.clear();
  RA3.clear();

  // initialize the SD card
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelect, OUTPUT);

  // set date time callback function for date time on disk
  SdFile::dateTimeCallback(dateTime);

  // see if the card is present and can be initialized:
  if (SD.begin(chipSelect)) SDActionS = true;  // SD card herkend

  // create a new file
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    if (! SD.exists(filename)) { // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }
  if (logfile) SDActionF = true; // file aangemaakt

  Wire.begin();
  if (!RTC.begin()) {
    logfile.println("RTC failed");
  }
  WriteSDcard1("00");
  logfile.println(F("0,0,Bestand aangemaakt"));
  logfile.flush();

  light.begin(); // Initialize the SFE_TSL2561 library
  unsigned char time = 2;
  light.setTiming(gain, time, ms);
  light.setPowerUp();

/* EPROM plaats
	0 = SMS code ja/nee
	1 - 10 = telefoonnummer
	11 = 
*/

  SMScode = EEPROM.read(0); // SMScode ophalen uit EPROM op plaats 0
  telefoonnummer = "";
  int x;
  for (int i = 1; i < 11; i = i + 1) {
    x = EEPROM.read(i);  // in x staat een ascii cijfer
    telefoonnummer = telefoonnummer + String(x - 48); // en nu staat er een getalstring
  }
  Serial.println(telefoonnummer);
  //Serial.print("freeMemory()=");
  //Serial.println(freeMemory());
//StuurBericht("12");
}  // einde Setup

void loop() {
  //Serial.println("Start PompRegeling");
  // PompRegeling();		// regelt de pompen aan/uit
  DuoPompRegeling();	// regelt met twee pompen
  //Serial.println("Start LeesHum");
  LeesHumTemp();     // lees Humidity en temperatuur
  //Serial.println("Start LeesLight");
  LeesLight();       // lees de lichtopbrengst
  //Serial.println("Start LeesRaindrop");
  LeesRaindropSensor();	// Lees de raindrop sensoren
  ControleerSensoren();
  //Serial.println("Start Laagwatersignaal");
  LaagWater();	// routine om de laagwatervlotter uit te lezen
  //LaagWaterSignaal();// Stuurt SMS als waterniveau kritisch laag is
  //Serial.println("Start TestSignaal");
  TestSignaal();     // Stuurt een teken van leven naar SMS
  //Serial.println("Start Einde Loop");
  //Serial.print("in loop: ");
  //Serial.println(freeRam());  //test op vrij geheugen SRAM
   //Serial.println("Start ReadBT");
   ReadBT();		// Lees de BlueTooth input
   // Serial.println("Start SendBT");
   if (Connected) SendBT();		// Zend info naar Android toestel	

}  //einde loop

void TestSignaal() {    // Stuurt een teken van leven naar SMS

  // stuur iedere dag om 12:00 uur een Testbericht
	
  if (now.hour() == 12 && now.minute() == 00) {
    StuurBericht("12");
  }

}  // einde TestSignaal

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void softwareReset( uint8_t prescaller) {
	// start watchdog with the provided prescaller
	wdt_enable( prescaller);
	// wait for the prescaller time to expire
	// without sending the reset signal by using
	// the wdt_reset() method
	while(1) {}
}



