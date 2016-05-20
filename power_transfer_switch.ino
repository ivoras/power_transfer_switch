#include <SoftwareSerial.h>
#include <PZEM004T.h>
#include <EEPROM.h>

#define DEFAULT_P1_MIN_V 210
#define WANTED_COUNT_P1_ABOVE_MIN 10
#define WANTED_COUNT_P1_BELOW_MIN 2

#define EEPROM_P1_MIN_ADDR 0

#define P1_PIN1 7
#define P1_PIN2 6
#define P2_PIN1 5
#define P2_PIN2 4

byte p1_min_v = DEFAULT_P1_MIN_V; // minimum p1 voltage

PZEM004T pzem(3,2);  // RX,TX
IPAddress ip(192,168,1,1);

int v; // Currently measured voltage on P1
unsigned int count_p1_above_min = 0;
unsigned int count_p1_below_min = 0;
byte current_source = 2; // 1 or 2: P1 or P2
bool continuous_stats = false;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(200);
  pzem.setAddress(ip);
  byte new_p1_min_v = EEPROM.read(EEPROM_P1_MIN_ADDR);
  if (valid_p1_min_v(new_p1_min_v))
    p1_min_v = new_p1_min_v;
  pinMode(13, OUTPUT);
  pinMode(P1_PIN1, OUTPUT);
  pinMode(P1_PIN2, OUTPUT);
  pinMode(P2_PIN1, OUTPUT);
  pinMode(P2_PIN2, OUTPUT);
  switchToSource(current_source);
}


void loop() {
  digitalWrite(13, HIGH);
  v = pzem.voltage(ip);
  digitalWrite(13, LOW);

  if (v >= p1_min_v) {
    count_p1_above_min++;
    count_p1_below_min = 0;
  } else {
    count_p1_below_min++;
    count_p1_above_min = 0;
  }
  
  if (current_source == 2) {
    if (count_p1_above_min >= WANTED_COUNT_P1_ABOVE_MIN) {
      switchToSource(1);
    }
  } else if (current_source == 1) {
    if (count_p1_below_min >= WANTED_COUNT_P1_BELOW_MIN) {
      switchToSource(2);
    }
  }

  processContinuousStats();
  processCmdLine();
  delay(2000);
}


/**
 * Test if the voltage is acceptable for P1
 */
bool valid_p1_min_v(byte v) {
  return (v < 255 && v > 100);
}


/**
 * Read and process the command line
 */
void processCmdLine() {

  String cmd = Serial.readStringUntil('\n');
  if (cmd[0] == 't' || cmd[0] == 'T') {
    // Change P1 minimum voltage threshold
    byte new_p1_min_v = cmd.substring(1).toInt();
    if (valid_p1_min_v(new_p1_min_v)) {
      p1_min_v = new_p1_min_v;
      EEPROM.write(EEPROM_P1_MIN_ADDR, new_p1_min_v);
      Serial.println("OK");
    }
  } else if (cmd[0] == 'c') {
    continuous_stats = !continuous_stats;
  } else if (cmd[0] == '?') {
    // Print settings and current values
    Serial.print(F("P1 min voltage: "));
    Serial.println(p1_min_v);

    Serial.print("P1: ");
    Serial.print(v);
    Serial.print("V; ");
  
    float i = pzem.current(ip);
    if(i >= 0.0){ Serial.print(i);Serial.print("A; "); }
    
    float p = pzem.power(ip);
    if(p >= 0.0){ Serial.print(p);Serial.print("W; "); }
    
    float e = pzem.energy(ip);
    if(e >= 0.0){ Serial.print(e);Serial.print("Wh; "); }
  
    Serial.println();
  }
}


/**
 * Switch the relays to the given source port and update the current_source variable.
 * It does this by first turning of the previous source port.
 */
void switchToSource(byte s) {
  if (s == 1) {
    digitalWrite(P2_PIN1, HIGH);
    digitalWrite(P2_PIN2, HIGH);
    digitalWrite(P1_PIN1, LOW);
    digitalWrite(P1_PIN2, LOW);
    current_source = s;
  } else if (s == 2) {
    digitalWrite(P1_PIN1, HIGH);
    digitalWrite(P1_PIN2, HIGH);
    digitalWrite(P2_PIN1, LOW);
    digitalWrite(P2_PIN2, LOW);
    current_source = s;
  }
}

void processContinuousStats() {
  if (continuous_stats) {
    Serial.print("P1: ");
    Serial.print(v);
    Serial.println("V");
  }
}

