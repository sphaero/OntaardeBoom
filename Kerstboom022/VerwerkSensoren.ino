
/*	Hier worden de Raindropsensoren uitgelezen
**	Uitgelezen waarde = 0 wil zeggen: drijfnat
**	Uitgelezen waarde hoog wil zeggen: droog
*/

void LeesRaindropSensor() {    // lees de Raindropsensoren
	Rain1 = analogRead(druppel1);
	Rain2 = analogRead(druppel2);
	Rain3 = analogRead(druppel3);
	delay(100);
	SchrijfRaindrop(Rain1,Rain2,Rain3);
	
	RA1.addValue(Rain1);	// bereken de gemiddelde waarde van Rain1
	RA2.addValue(Rain2);
	RA3.addValue(Rain3);
	Rain1Gem = RA1.getAverage();
	Rain2Gem = RA2.getAverage();
	Rain3Gem = RA3.getAverage();
	
	// vaststellen of de sensoren droog zijn
	
	if (Rain1_stuk == false)	// als eerste sensor in orde is
		if (Rain1 > Drooglevel1) Droog = false; else Droog = true;			// kijk of de sensor droog staat
	else   // de eerste sensor is stuk
		if (Rain2_stuk == false)	// als de tweede sensor in orde is
			if (Rain2 > Drooglevel2) Droog = false; else Droog = true;		// kijk of de sensor droog staat
		else   // de tweede sensor is ook stuk
			if (Rain3_stuk == false)	// als de derde sensor in orde is	
				if (Rain3 > Drooglevel3) Droog = false; else Droog = true;	// kijk of de sensor droog staat
			else {		// alle sensoren zijn stuk
				PompStatus = 8;		// zet de pompcyclus stop
				if (Rain_SMS_Gestuurd == false) {
					StuurBericht("09");	// stuur een alarmerende SMS en stop Arduino
					Rain_SMS_Gestuurd = true;	
				}
				digitalWrite(9,LOW);	// zet pomp 1 uit
				digitalWrite(10,HIGH);	// zet pomp 2 aan
			};
}	// einde LeesRaindropSensor

void SchrijfRaindrop (int R1,int R2,int R3) {	// schrijf waarde raindrop in tussenposen van R_INTERVAL naar SD card
	if ((millis() - syncTimeL) < LOG_R_INTERVAL) return;  // de intervaltijd is nog niet verstreken
	syncTimeL = millis();
	WriteSDcard1("04");
	logfile.println(R1);
	WriteSDcard1("05");
	logfile.println(R2);
	WriteSDcard1("06");
	logfile.println(R3);
	logfile.flush();
}

void ControleerSensoren(){
	// een sensor is kapot als de waarde vaak om het gemiddelde schommelt
	// vaak is meer dan de helft van het aantal tellingen
	// aantal tellingen is bv 20
	// elke keer dat deze routine benaderd wordt, wordt er één keer gemeten en geteld
	
	if(PompStatus == 4) {	// doe dit alleen als de pomp in normaal bedrijf is
		
		int Rain1Verschil = 0;
		int Rain2Verschil = 0;
		int Rain3Verschil = 0;
		Teller = Teller +1;		// aantal malen dat gemeten is
		
		Rain1Verschil = Rain1 - Rain1Gem;
		if (Rain1Verschil < -Druppelspeling  & Rain1Pos == true) {
			FoutTeller1 = FoutTeller1 +1;
			Rain1Pos = false;
		}
		else if (Rain1Verschil > Druppelspeling & Rain1Pos == false) {
			FoutTeller1 = FoutTeller1 +1;
			Rain1Pos = true;
		}
		
		Rain2Verschil = Rain2 - Rain2Gem;
		if (Rain2Verschil < -Druppelspeling  & Rain2Pos == true) {
			FoutTeller2 = FoutTeller2 +1;
			Rain2Pos = false;
		}
		else if (Rain2Verschil > Druppelspeling & Rain2Pos == false) {
			FoutTeller2 = FoutTeller2 +1;
			Rain2Pos = true;
		}
		
		Rain3Verschil = Rain3 - Rain3Gem;
		if (Rain3Verschil <  -Druppelspeling  & Rain3Pos == true) {
			FoutTeller3 = FoutTeller3 +1;
			Rain3Pos = false;
		}
		else if (Rain3Verschil > Druppelspeling & Rain3Pos == false) {
			FoutTeller3 = FoutTeller3 +1;
			Rain3Pos = true;
		}
		
		if (Teller > 20	) {	// het maximum tellingen is bereikt
			if (FoutTeller1 > Teller/2) {	// meer dan de helft van het aantal metingen, dan fout
				Rain1_stuk = true; 
				// TODO send sms once
			}
			else Rain1_stuk = false; 
			if (FoutTeller2 > Teller/2) {
				Rain2_stuk = true; 
				// TODO send SMS once
			}
			else Rain2_stuk = false;
			if (FoutTeller3 > Teller/2) {
				Rain3_stuk = true; 
				// TODO send SMS once
			}
			else Rain3_stuk = false;
			
			Teller = 0;		// zet het aantal tellingen weer op 0
			FoutTeller1 = 0;
			FoutTeller2 = 0;
			FoutTeller3 = 0;
		}
	
	Serial.print("Teller = ");
	Serial.println(Teller);
	Serial.print("FoutTeller1 = ");
	Serial.print(FoutTeller1);
	Serial.print(" Rain1_stuk = ");
	Serial.println(Rain1_stuk);
	Serial.print("FoutTeller2 = ");
	Serial.print(FoutTeller2);
	Serial.print(" Rain2_stuk = ");
	Serial.println(Rain2_stuk);
	Serial.print("FoutTeller3 = ");
	Serial.print(FoutTeller3);
	Serial.print(" Rain3_stuk = ");
	Serial.println(Rain3_stuk);
	
	}	// einde if PompStatus == 4
	
}  // einde Controleer Sensoren

