/* specificatie van Android ontvangen kode

	a = zet SMS code aan. Return 10 met SMScode
	b = set SMS code uit. Return 10 met SMScode
	c = test return connectie. Geen Androidinfo. 
	d = geef de raindrop sensor waarde door. Return: 17,18 of 19 + de sensor waarde.
	e = geef de gemiddelde raindrop sensor waarde door. Return: 20, 21 en 22 + de sensor waarde.
	f = geef de files op de SD card door aan Android. Return 11 met de files.
	g = geef de inhoud van de file door aan Android. Return 12 met de inhoud.
	h = geef Humidity en temperatuur door. Return 04 met Humidity en 05 met Temperatuur.
	l = geef LUX door. Return 09 met LUX waarde
	k = lees de Arduino settings uit. Meerdere settings in de info.
	m = stuur de tijd op. Return 8 met tijd.
	n = geef de raindrop droog code door. Return: 23,24 of 25 + de droog code.
	o = stuur de datum op. Return 3 met datum.
	q = delete de file 'bestand' van SD card. Return 13 met bestandsnaam.
	r = geef status VlotterLaag door. Return 2L of 2H
	s = lees SDA Array uit over SD card actie. Return 06 en 07 met Schijfino en bestandsnaam
	t = lees telefoonnummer uit. Return 14 met telefoonnummer
	u = stuur PompStatus. Return: "15" met PompStatus
	v = stuur Pompnummer. Return: "16" met Pompnummer.
	x = set Connect = false. Geen return.
	z = reset Arduino.
	
	
	
	
*/

	boolean VlotterWaarde = false;
	
void ReadBT() {          // Lees de BlueTooth input. en einde = #
	
  char inChar = 0;
  String Androidinfo = "";
  char s;
  String inBuffer = "";

  if (Serial3.available()) {
    while (inChar != '#')  // zolang einde info nog niet bereikt is
    {
      inChar = Serial3.read();
      inBuffer = inBuffer + String(inChar);
      Serial.print("inChar = ");
      Serial.println(inChar);
    }

    s = inBuffer.charAt(0);
    Androidinfo = inBuffer.substring(1, inBuffer.length() - 1);

    Serial.print("kode = ");
    Serial.print(s);
    Serial.print("   Androidinfo = ");
    Serial.println(Androidinfo);

    

    if (s == 'a') {  // zet de SMScode aan
      SMScode = '1';
	  ReactieOpa(SMScode);	// schrijf EPROM en geef door aan Android
    }

    if (s == 'b') { // zet SMScode uit
      SMScode = '0';
	  ReactieOpa(SMScode);	// schrijf EPROM en geef door aan Android
    }
    
    if (s == 'c') {
      Serial3.print(F("01connected#"));  // test return connection
	  
	  ReactieOpa(SMScode);
	  ReactieOpt(telefoonnummer);
	  ReactieOpo();	// stuur de datum op
	  Connected = true;
      delay(2000);
    }
    
    if (s == 'd') {
      ReactieOpd();  // geef de rainDrop door door aan de smartphone
    }
	
	 if (s == 'e') {
		 ReactieOpe();  // geef de gemiddelde rainDrop door door aan de smartphone
	 }
    
     if (s == 'f') {  // geef de files door aan Android
      ReactieOpf();
    }
    
    if (s == 'g') {  // geef de inhoud van de gekozen file door
      ReactieOpg(Androidinfo);
    }
    
    if (s == 'h') {
      ReactieOph();  // geef de humidity en temperatuur door aan de smartphone
    }
    
     if (s == 'l') {
      ReactieOpl();  // geef de lux door door aan de smartphone
    }
	
	 if (s == 'k') {
		 ReactieOpk(Androidinfo);  // geef de pref file door door aan de smartphone
	 }
    
    if (s == 'q') {
      ReactieOpq(Androidinfo);  // delete de file op de SD card
    }
    
    if (s == 'r') {
      ReactieOpr();  // lees de Vlotterstand uit 
    }

    if (s == 's') {
      ReactieOps();  // lees error array SDAction uit
    }
        
    if (s == 't') {  // lees telefoonnummer uit en sla op
      ReactieOpt(Androidinfo);
    }
	
	if (s == 'u') {
		ReactieOpu();  // lees PompStatus uit
	}
	
	if (s == 'x') {
		Connected = false;  // set Connect uit
		Serial.print("Connected: ");
		Serial.println(Connected);
	}
	
	if (s == 'z') { 
		softwareReset( WDTO_60MS);	// restart in 60 milliseconds
		
	}
   
    

  }  // einde available
}  // einde ReadBT

void SendBT() {		// send info naar Android toestel
		
	if (PompStatus != PompStatusoud) {		// Alleen verzenden als er iets gewijzigd is
		ReactieOpu();		// stuur status	 
		PompStatusoud = PompStatus;
	}
	
	ReactieOph();		// geef hum en temp door
	ReactieOpl();		// geef LUX door
	ReactieOpd();		// geef sensorinfo door
	ReactieOpe();		// geef de gemiddelde sensorwaarden door
	ReactieOpn();		// geef de droog info door
	
	VlotterWaarde = digitalRead(VlotterLaag);
	if (VlotterWaarde != laagwateroud) {	// als de waarde veranderd is
		ReactieOpr();						// geef status VlotterLaag door
		laagwateroud = VlotterWaarde;
	}	
				
	ReactieOpm();		// stuur de tijd door
			 
			 
	
	
}

void ReactieOpa(char SMSkode){	// geef SMScode door aan Android
	String tkode = "";
	if (SMSkode == '0') tkode = "nee"; 
	else tkode = "ja";
	String tydelijk = "10" + String(tkode) + "#";
	Serial3.print(tydelijk);
	EEPROM.write(0, SMScode);	// en schrijf in EPROM
}

void ReactieOps() {  // lees error array SDAction uit
  String tydelijk = filename;
  if (SDActionS) Serial3.print(F("06card initialized#"));
  else Serial3.print(F("06Card failed, or not present#"));
  delay(20);
  if (SDActionF) {
    tydelijk = "07" + tydelijk + "#";
    Serial3.print(tydelijk);
  }
  else {
    Serial3.print(F("07couldnt create file#"));
  }
  delay(20);
}  // einde reactieOps


void ReactieOpr() {		// geeft Vlotterstand door
	if (VlotterWaarde == HIGH) {
		Serial3.print("02L#");
		delay(20);
	}
	else {
		Serial3.print("02H#");
		delay(20);
	}
	
  } // einde ReactieOpr


void ReactieOph() { //stuur de humidity en temperatuur door naar de smartphone
  String tydelijk;

  tydelijk = "04" + Hum + "#";
  if (isnan(h)) {
    Serial3.print(F("04Failed to read from DHT sensor!#"));
    delay(20);
  }
  else {
    Serial3.print(tydelijk);
    delay(20);
  }

  tydelijk = "05" + TempC + "#";
  if (isnan(t)) {
    Serial3.print(F("05Failed to read from DHT sensor!#"));
    delay(20);
  }
  else {
    Serial3.print(tydelijk);
    delay(20);
  }
}    // einde ReactieOph

void ReactieOpl() { //stuur de lux door naar de smartphone
  // hier later de error code aan toevoegen
  String tydelijk;
  tydelijk = "09" + Light + "#";
  Serial3.print(tydelijk);
  delay(20);
}    // einde ReactieOpl

void ReactieOpd() { //stuur de rain sensorwaarden door naar de smartphone
  String tydelijk;
  tydelijk = "17" + String(Rain1) + "#";
  Serial3.print(tydelijk);
  delay(20);
  tydelijk = "18" + String(Rain2) + "#";
  Serial3.print(tydelijk);
  delay(20);
  tydelijk = "19" + String(Rain3) + "#";
  Serial3.print(tydelijk);
  delay(20);
}    // einde ReactieOpd

void ReactieOpe() { //stuur de gemiddelde rain sensorwaarden door naar de smartphone
	String tydelijk;
	tydelijk = "20" + String(Rain1Gem) + "#";
	Serial3.print(tydelijk);
	delay(20);
	tydelijk = "21" + String(Rain2Gem) + "#";
	Serial3.print(tydelijk);
	delay(20);
	tydelijk = "22" + String(Rain3Gem) + "#";
	Serial3.print(tydelijk);
	delay(20);
}    // einde ReactieOpe

void ReactieOpn() { //stuur de droog kode van sensorwaarden door naar de smartphone
	String tydelijk;
	
	if (Rain1_stuk) tydelijk = "sensor stuk?";
		else if (Rain1 < Drooglevel1) tydelijk = "Droog";
			else tydelijk = "nat";
	tydelijk = "23" + String(tydelijk) + "#";
	Serial3.print(tydelijk);
	delay(20);
	
	if (Rain2_stuk) tydelijk = "sensor stuk?";
	else if (Rain2 < Drooglevel2) tydelijk = "Droog";
	else tydelijk = "nat";
	tydelijk = "24" + String(tydelijk) + "#";
	Serial3.print(tydelijk);
	delay(20);
	
	if (Rain3_stuk) tydelijk = "sensor stuk?";
	else if (Rain3 < Drooglevel3) tydelijk = "Droog";
	else tydelijk = "nat";
	tydelijk = "25" + String(tydelijk) + "#";
	Serial3.print(tydelijk);
	delay(20);
	
}    // einde ReactieOpn

void ReactieOpt(String telnummer) {  // lees telefoonnummer uit en sla op
	String tydelijk = "14" + telnummer + "#";
	Serial3.print(tydelijk);
	delay(20);
	for (int i = 1; i < 11; i = i + 1) {
		EEPROM.write(i, telnummer.charAt(i - 1));
	}
}

void ReactieOpk(String prefinfo) {	// lees de Arduino settings uit. Meerdere settings in info. En sla op in EPROM
	int b = 0;	// beginpositie in string
	int e = prefinfo.length();	// eindpositie in string
	String hulpstring = "";
	
	// opsplitsen prefinfo in afzonderlijke velden. Ze zijn gescheiden door een "$"
	// drooglevel1
	e = prefinfo.indexOf ("$");
	hulpstring = prefinfo.substring(b,e);
	Drooglevel1 = hulpstring.toInt();
	Serial.print("drooglevel1: ");
	Serial.println(Drooglevel1);
	
	// drooglevel2
	b = e + 1;	// het nieuwe begin
	e = prefinfo.indexOf ("$",b);
	hulpstring = prefinfo.substring(b,e);
	Drooglevel2 = hulpstring.toInt();
	Serial.print("drooglevel2: ");
	Serial.println(Drooglevel2);
	
	// drooglevel3
	b = e + 1;	// het nieuwe begin
	e = prefinfo.indexOf ("$",b);
	hulpstring = prefinfo.substring(b,e);
	Drooglevel3 = hulpstring.toInt();
	Serial.print("drooglevel3: ");
	Serial.println(Drooglevel3);
	
	// droogtijd
	b = e + 1;	// het nieuwe begin
	e = prefinfo.indexOf ("$",b);
	hulpstring = prefinfo.substring(b,e);
	Droogtijd = hulpstring.toInt();
	Serial.print("droogtijd: ");
	Serial.println(Droogtijd);
	
	// druppelspeling
	b = e + 1;	// het nieuwe begin
	e = prefinfo.indexOf ("$",b);
	hulpstring = prefinfo.substring(b,e);
	Druppelspeling = hulpstring.toInt();
	Serial.print("druppelspeling: ");
	Serial.println(Druppelspeling);
	
	// aantal samples  OPGEPAST! NA VERANDERING VAN DEZE WAARDE MOET ARDUINO OPNIEUW WORDEN OPGESTART
	b = e + 1;	// het nieuwe begin
	e = prefinfo.indexOf ("$",b);
	hulpstring = prefinfo.substring(b,e);
	//  TODO  uitzoeken of het volgende  wel kan
	// ra = hulpstring.toInt();  // als het kan de // weghalen
	Serial.print("samples: ");
	Serial.println(ra);
	
	// laagwaterdelay
	b = e + 1;	// het nieuwe begin
	e = prefinfo.indexOf ("$",b);
	hulpstring = prefinfo.substring(b,e);
	laagwater_delay = hulpstring.toInt();
	Serial.print("vlotterdelay: ");
	Serial.println(laagwater_delay);
	
	// TODO alle gegevens in EPROM opslaan
}

void ReactieOpf() {  // geef de files op de SD card door aan Android
  // tijdens deze actie worden er geen metingen verricht!!!
  File root;
  root = SD.open("/");
  root.rewindDirectory();
  printDirectory(root, 0);
}

void printDirectory(File dir, int numTabs) {
	Serial3.print("11");
	while (true) {
		File entry =  dir.openNextFile();
		if (!entry) {
			Serial.println(" no more files");
			Serial3.print("#");
			dir.rewindDirectory();
			break;
		}
		for (uint8_t i = 0; i < numTabs; i++) {
			//Serial.print('\t');
		}
		//Serial.print(entry.name());
		Serial3.print(entry.name());
		if (entry.isDirectory()) {
			//Serial.println("/");
			printDirectory(entry, numTabs + 1);
			} else {
			// files have sizes, directories do not
			Serial3.print(" ");
			Serial3.print(entry.size(),DEC);
			Serial3.print(",");
			//Serial.print("\t");
			//Serial.println(entry.size(), DEC);
			
		}
		entry.close();
	}
}

void ReactieOpg(String bestand){
	int L = bestand.length()+1;
	char in;
	Serial.print("bestand: ");
	Serial.println(bestand);
	char fileNameCharArray[L];
	bestand.toCharArray(fileNameCharArray, L);
	File dataFile = SD.open(fileNameCharArray,FILE_READ);
	// if the file is available, read it:
	if (dataFile) {
		while (dataFile.available()) {
			Serial3.print("12");
			in=(dataFile.read());
			while (in!= '\n') {
				Serial3.write(in);
				in=(dataFile.read());
			}
			Serial3.print("#");
		}
		dataFile.close();
		Serial3.print("12");
		Serial3.print("einde");
		Serial3.print("#");
	}
	// if the file isn't open, pop up an error:
	else {
		Serial.print("error opening ");
		for (int i=0;i<L;i=i+1) Serial.print(fileNameCharArray[i]);
		Serial.println();
	}
	
}

void ReactieOpq(String bestand){  // delete de file 'bestand' van SD card
	int L = bestand.length()+1;
	char in;
	char fileNameCharArray[L];
	bestand.toCharArray(fileNameCharArray, L);
	if (SD.exists(fileNameCharArray)) {
		SD.remove(fileNameCharArray);
		Serial.print("File: ");
		Serial.print(bestand);
		Serial.println(" deleted.");
		Serial3.print("13");
		Serial3.print(bestand);
		Serial3.print(" deleted#");
	}
	else {
		Serial.print("File: ");
		Serial.print(bestand);
		Serial.println(" bestaat niet.");
		Serial3.print("13");
		Serial3.print(bestand);
		Serial3.print(" bestaat niet.#");
	}
}

void ReactieOpu(){		// stuur PompStatus. Return: "15" met PompStatus
	if (PompStatus == 4) ReactieOpv();	// stuur dan eerst het Pompnummer op
		
	String tydelijk;	
	tydelijk = "15" + String(PompStatus) + "#";
	Serial3.print(tydelijk);
	delay(20);	
	
	//Serial.print("Pompstatus verzonden = ");
	//Serial.println(PompStatus);
}

void ReactieOpv() {		// stuur Pompnummer. Return: "16" met Pompnummer.
	String tydelijk;
	tydelijk = "16" + String(Pig-8) + "#";
	Serial3.print(tydelijk);
	delay(20);
	
}

void ReactieOpm() {		// stuur tijd
	now = RTC.now();  // fetch the time
	String tydelijk = "08" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "#";
	Serial3.print(tydelijk);
	delay(20);
	
}

void ReactieOpo() {		// stuur datum
	now = RTC.now();  // fetch the time
	String tydelijk = "03" + String(now.day()) + ":" + String(now.month()) + ":" + String(now.year()) + "#";
	Serial3.print(tydelijk);
	delay(20);
	
}	