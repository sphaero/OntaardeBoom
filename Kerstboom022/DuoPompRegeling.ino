/*	Twee pompen worden aangestuurd.
	Eerst is pomp1 de PompInGebruik PIG), als vermoed wordt dat de pomp stuk is wordt pomp 2 de PompInGebruik.
	De uitgangen van de Arduino sturen twee relais. Deze schakelen de pompen.
	Pomp wordt aangezet door Arduino uitgang Hoog te maken. Het relais valt dan af, de pomp gaat aan.
	Het relais heeft een wisselschakelaar. De pomp moet aan de juiste schakelstand aangesloten worden. 
	Dat is: Relais uit: verbinding aan.
*/

void DuoPompRegeling() {
	switch (PompStatus) {
		case 0: // init
			Pig = Pomp1;
			PompStatus = 1;
			break;
		case 1:	// laagwater testen
			digitalWrite(Pig,LOW);	// zet de PompInGebruik uit
			if (sw_laagwater == false) 
				PompStatus = 2;			
			break;
		case 2:	// zet de PompInGebruik aan
			digitalWrite(Pig,HIGH);
			// start de tijd om de boom de tijd te geven nat te worden
			DroogtijdLL = millis();		// start de tijd om nat te worden
			PompStatus = 3;
			if (sw_laagwater == true) {
				PompStatus = 1; // een status terug
			}
			break;
		case 3:	// de timer loopt
			if ((millis() - DroogtijdLL) > Droogtijd) PompStatus = 4;
			if (sw_laagwater == true) {
				PompStatus = 1; // een status terug
			}
			break;
		case 4:	// de normale status met een draaiende pomp
			if (sw_laagwater == true) PompStatus = 1; // zet de Pig weer uit
			if (Droog == true) PompStatus = 5;		 // zet de volgende pomp aan
			break;
		case 5:	// zet de Pig uit
			digitalWrite(Pig,LOW);	// zet de PompInGebruik uit
			looptijdLL = 0;			// zet de looptijd laagwater weer op 0
			PompStatus = 6;
			break;
		case 6:	// zet de volgende pomp in gebruik
			if (Pig == Pomp1) {
				Pig = Pomp2;
				PompStatus = 7;
			}
			else {
				StuurBericht("11");	// stuur SMS dat pomp2 uitgezet is.
				PompStatus = 8;
			}
			break;
		case 7:	// stuur SMS dat Pomp1 uitgezet is
			StuurBericht("10");	// stuur SMS dat pomp1 uitgezet is
			PompStatus = 2;
			break;
		case 8:	// zet het systeem uit
			// TODO zet het systeem uit
			break;
		default: // foutsituatie
			// TODO geef foutboodschap
			break;
	}	// einde switch case
	
	// Serial.print("Pompstatus = ");
	// Serial.println(PompStatus);
	
}	// einde Duopompregeling
