/*	Afhandeling SMS en schrijven naar SD card

	code	omschrijving				SMS	SD
	00		Bestand aangemaakt			x	00
	01
	02		temp,hum					x	02
	03	
	04		raindropwaarde 1			x	04
	05		raindropwaarde 2			x	05
	06		raindropwaarde 3			x	06
	07	
	08		bak is leeg					08	08
	09
	10		
	11		wortels krijgen geen water!	11	11
	12		testsignaal gestuurd		12	12
	
*/
void StuurBericht(String kode) {
	if (SMScode == '1') { // stuur SMS
		SIM900power();  // schakel SIM in
		delay(10000);   // time to log on to the network of 20000?
		sendSMS(kode);      // stuur SMS bericht
		SIM900power();  // schakel SIM uit
	}
	
	WriteSDcard1(kode);  // later aanpassen aan wie het verstuurd is
	if (kode == "08") logfile.println(F("0,0,LL SMS gestuurd"));
	if (kode == "11"){
		if (SMScode == '1') logfile.println(F("0,0,RD SMS gestuurd"));
		else logfile.println(F("0,0,geen RD SMS gestuurd"));
	}
	if (kode == "12") logfile.println(F("0,0,TM SMS gestuurd"));
	logfile.flush();

}  // einde StuurBericht

void SIM900power() { // equivalent of pressing the GSM shield "power" button
	digitalWrite(Simpower, HIGH);
	delay(1000);
	digitalWrite(Simpower, LOW);
	delay(5000);
}

void sendSMS(String kode) {
	Serial.println("Bellen begint.");
	String slash = "\"";
	String beginString = "AT + CMGS = ";
	String totaalString = beginString + slash + telefoonnummer + slash;

	Serial1.print("AT+CMGF=1\r"); // AT command to send SMS message
	delay(100);
	Serial1.println(totaalString);
	
	//Serial1.println("AT + CMGS = \"+31653169253\""); // Bernard mobile number, in international format
	delay(100);
	if (kode == "08") Serial1.println(F("De bak van de ontwortelde boom is leeg. HELP!")); // message to send
	if (kode == "11") Serial1.println(F("De wortels van de ontwortelde boom krijgen geen water. HELP!")); // message to send
	if (kode == "12") Serial1.println(F("Testbericht Ontwortelde boom")); // message to send
	delay(100);
	Serial1.println((char)26);  // End AT command with a ^Z, ASCII code 26
	delay(100);
	Serial1.println();
	delay(5000);            // give module time to send SMS
	Serial.print("Gebeld naar: ");
	Serial.println(totaalString);
}

