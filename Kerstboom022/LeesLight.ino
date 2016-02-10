void LeesLight() {       // lees de lichtopbrengst
	String lgData;
	delay(ms);
	unsigned int data0, data1;
	if (light.getData(data0, data1)) { // getData() returned true, communication was successful
		good = light.getLux(gain, ms, data0, data1, lux); // Perform lux calculation
		dtostrf(lux, 5, 1, buffer);
		Light = buffer;
		if (good) lgData = "0,good";
		else lgData = "0,BAD";
		lgData = Light + "," + lgData;
		if ((millis() - syncTimeL) < LOG_L_INTERVAL) return;  // de intervaltijd is nog niet verstreken
		syncTimeL = millis();
		WriteSDcard1("09");
		logfile.println(lgData);
		logfile.flush();
	}
	else  // getData() returned false because of an I2C error, inform the user.
	{
		byte error = light.getError();
		// printError(error);
	}

}  // einde LeesLight
