void WriteSDcard1(String code) {     // schrijf naar de SD card wat nodig is
  
  // controleer of file nog bestaat
  if(!logfile) Serial.println("***** file bestaat niet meer");
  void SchrijfDatumTyd();  // declaratie van ScrijfDatumTyd
  now = RTC.now();  // fetch the time
  logfile.print(code);  //  recordsoort
  logfile.print(",");
  SchrijfDatumTyd();
  logfile.print(",");
}  // einde WriteSDcard1

void SchrijfDatumTyd() {  // log time
  //logfile.print('"');
  logfile.print(now.year(), DEC);
  logfile.print('-');
  logfile.print(now.month(), DEC);
  logfile.print('-');
  logfile.print(now.day(), DEC);
  //logfile.print('"');
  logfile.print(" ");
  //logfile.print('"');
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
  //logfile.print('"');
}  // einde SchrijDatumTyd
