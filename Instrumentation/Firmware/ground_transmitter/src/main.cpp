#include <Arduino.h>

uint16_t channelUs[16];
const int THROTTLE_CH = 3;

// Buffer for incoming characters (kept between passes of loop())
char buf[32];
int pos = 0;

void safe_value() {
  for (int i = 0; i < 16; i++) {
    channelUs[i] = 1500;
  }
  channelUs[THROTTLE_CH - 1] = 988;
}

// Process inputs
void processLine(String line) {
  line.trim(); // remove stray spaces / \r so comparisons work

  if (line == "P") { // handle print command
    for (int i = 0; i < 16; i++) {
      Serial.println(channelUs[i]);
    }
  }
  else if (line == "SAFE") {
    safe_value();
    Serial.println("OK safe");
  }
  else if (line.startsWith("S ")) {
    int ch, us;
    int found = sscanf(line.c_str(), "S %d %d", &ch, &us);
    if (found != 2) { // make sure 2 numbers after S, otherwise command is nonsense
      Serial.println("ERR unknown command");
      return;         // now only leaves processLine(), not loop()
    }
    if (ch < 1 || ch > 16) {
      Serial.println("ERR bad channel");
      return;
    }
    if (us < 988 || us > 2012) {
      Serial.println("ERR bad value");
      return;
    }
    // Now what if our command is actually ok
    channelUs[ch - 1] = us;
    Serial.printf("OK ch%d=%d\n", ch, us);
  }
  else {
    Serial.println("ERR unknown command");
  }
}

void setup() {
  Serial.begin(115200);
  safe_value();
}

void loop() {
  // read whatever characters have arrived, without waiting
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {            // end of line: command is complete
      buf[pos] = '\0';          // terminate the string
      processLine(String(buf)); // handle it
      pos = 0;                  // reset for the next command
    }
    else if (c == '\r') {
      // ignore carriage returns
    }
    else if (pos < (int)sizeof(buf) - 1) {
      buf[pos++] = c;           // store character, move to next slot
    }
    else {
      pos = 0;                  // line too long: discard it
      Serial.println("ERR line too long");
    }
  }

  // Part 2 will go here: send a CRSF frame every 4 ms
}