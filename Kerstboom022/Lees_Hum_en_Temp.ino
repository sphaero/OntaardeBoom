void LeesHumTemp() { // lees humidity en temperatuur
	String thData;
	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	h = dht.readHumidity();
	dtostrf(h, 5, 1, buffer);
	Hum = buffer;
	t = dht.readTemperature();  // Read temperature as Celsius
	dtostrf(t, 5, 1, buffer);
	TempC = buffer;
	f = dht.readTemperature(true);  // Read temperature as Fahrenheit
	thData = TempC + "," + Hum;
	if ((millis() - syncTimeT) < LOG_T_INTERVAL) return;  // de intervaltijd is nog niet verstreken
	syncTimeT = millis();
	WriteSDcard1("02");
	logfile.println(thData);
	logfile.flush();

	//Serial.print("in LeesHumTemp: ");s
	//Serial.println(freeRam());  //test op vrij geheugen SRAM

}  // einde leesHumTemp

