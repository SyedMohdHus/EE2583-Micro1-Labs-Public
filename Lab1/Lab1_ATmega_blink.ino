/*
  Lab 1 ATmega blink (built in LED on D13)
  Final mode for the lab: ON for 1.0 sec, OFF for 0.1 sec.
  Earlier steps of the lab:
    Step A: on 0.1 sec, off 1.0 sec  -> delay(100) on, delay(1000) off
    Step B: on 1.0 sec, off 0.1 sec  -> what is below (leave it like this)
  Tools > Board: Arduino Uno, Tools > Port: the CP2102 port
*/
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // LED on
  delay(1000);                       // 1.0 sec
  digitalWrite(LED_BUILTIN, LOW);    // LED off
  delay(100);                        // 0.1 sec
}
