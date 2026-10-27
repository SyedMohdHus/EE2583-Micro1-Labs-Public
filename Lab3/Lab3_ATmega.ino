/*
AT-Lab3 Timer for the PIC Plus counting the INT pulses.
1 ms pulse on pin D4 every 1.0 sec, and counts toggles of the D3 pin
coming from PIC RB5 (toggled by the ISR on the PIC).
Open the Serial Monitor at 9600 baud.
*/
int count = 0;

void setup() {
  pinMode(4, OUTPUT);
  pinMode(3, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial) continue;
}

void loop() {
  attachInterrupt(1, Notify, CHANGE);   // interrupt on both edges of D3
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(4, HIGH);
  delay(1);                             // 1 ms pulse
  digitalWrite(4, LOW);
  delay(799);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
}

void Notify() {
  count += 1;
  Serial.print("Count= dec: ");
  Serial.println(count);
}
