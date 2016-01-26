
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
	
	switch (RIG) {
		case 1:	
			if (Rain1_stuk == true) RIG = RIG +1;
			else
				if (Rain1 > Drooglevel1) Droog = false; else Droog = true;
				
			/*
			Serial.print("Rain1 = ");
			Serial.print(Rain1);
			Serial.print(" RIG = ");
			Serial.print(RIG);
			Serial.print(" Droog = ");
			Serial.print(Droog);
			Serial.print(" Rain1_stuk = ");
			Serial.println(Rain1_stuk);
			*/
			break;
		case 2:
			if (Rain2_stuk == true) RIG = RIG +1;
			else
				if (Rain2 > Drooglevel2) Droog = false; else Droog = true;
			
			/*
			Serial.print("Rain2 = ");
			Serial.print(Rain2);
			Serial.print(" RIG = ");
			Serial.print(RIG);
			Serial.print(" Droog = ");
			Serial.print(Droog);
			Serial.print(" Rain2_stuk = ");
			Serial.println(Rain2_stuk);
			*/
			break;
		case 3:
			if (Rain3_stuk == true) RIG = RIG +1;
			else
				if (Rain3 > Drooglevel3) Droog = false; else Droog = true;
			/*
			Serial.print("Rain3 = ");
			Serial.print(Rain3);
			Serial.print(" RIG = ");
			Serial.print(RIG);
			Serial.print(" Droog = ");
			Serial.print(Droog);
			Serial.print(" Rain3_stuk = ");
			Serial.println(Rain3_stuk);
			*/
			break;
		default:
			// TODO stuur dringend SMS en stop Arduino
			break;
	}	// einde switch
	
	
	//Serial.print("Rain1 = ");
	//Serial.println(Rain1);
						
	
}	// einde LeesRaindropSensor

void SchrijfRaindrop(int R1,int R2,int R3) {
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
	
	Rain1_stuk = false;
	Rain2_stuk = false;
	Rain3_stuk = false;

/* even uit gezet omdat ik een nieuwe procedure moet schrijven	
	// een sensor is kapot als de waarde vaak om het gemiddelde schommelt
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
		
		if (Teller > ra) {
			if (FoutTeller1 > Teller/2) Rain1_stuk = true; // meer dan de helft van het aantal metingen?
			if (FoutTeller2 > Teller/2) Rain2_stuk = true;
			if (FoutTeller3 > Teller/2) Rain3_stuk = true;
			Teller = 0;
			FoutTeller1 = 0;
			FoutTeller2 = 0;
			FoutTeller3 = 0;
		}
	
	
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
	
	*/
	
}





  
  /* TODO belangrijk voor het sturen van een SMS:
  if (Rain > 900) { // de wortels staan erg droog
    if ((millis() - syncTimeRD) > LOG_RD_INTERVAL) {  // de intervaltijd is verstreken
      syncTimeRD = millis();
      LOG_RD_INTERVAL = 1800000; // set nieuwe interval voor SMS
      StuurBericht("11");
    }
  }
  if ((millis() - syncTimeR) < LOG_R_INTERVAL) return;  // de intervaltijd is nog niet verstreken
  syncTimeR = millis();
  WriteSDcard1("10");
  logfile.println(Rain);
  logfile.flush();

  
}  // einde LeesRaindrop
*/
