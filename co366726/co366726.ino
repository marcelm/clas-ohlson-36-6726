/*
 * Simple sketch for reading temperature and humidity from a Clas Ohlson Cotech 36-6726 wireless sensor
 * 
 * Public Domain dedication:
 * To the extent possible under law, Marcel Martin has waived all copyright and related or neighboring rights to this work.
*/
#define PIN 8  // PIN connected to the 433 MHz receiver
#define DELAY 100
#define N_BITS 36

int buffer[N_BITS];

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN, INPUT);
  Serial.println("Cotech 36-6726 monitor");
}


/* Repeatedly read the digital input pin until it no longer is equal to b
 * Return the time in microseconds that the pin was b.
 */
int get_length(int b) {
  int value;
  int len = 0;
  while (digitalRead(PIN) == b) {
    len += 1;
    delayMicroseconds(DELAY);
  }
  return len * DELAY;
}


void loop() {
  unsigned long time;
  int l = 0;
  int w = 0;
  int n_bits = 0;
  int ones;
  int header;
  int channel;
  while (!(w >= 3500 && w <= 3800)) {
    w = get_length(0);
  }
  int initial = w;
  while (n_bits < N_BITS) {
    w = get_length(1);
    // we expect a high pulse for 500 µs
    if (w < 350 || w > 600) {
      return;
    }
    w = get_length(0);
    if (w < 700) {
      return;
    } else if (w < 1200) {
      // found a short (~850 µs) low pulse, this is a zero
      buffer[n_bits] = 0;
      n_bits += 1;
    } else if (w < 2100) {
      // found a long low (~1850 µs) low pulse, this is a one
      buffer[n_bits] = 1;
      n_bits += 1;
    } else {
      return;
    }
  }
  time = millis();
  Serial.print((float)time / 1000., 1);
  Serial.print(" s ");

  header = 0;
  for (int i = 0; i < 8; ++i) {
    header = (header << 1) | buffer[i];
  }
  channel = (buffer[10] << 1) | buffer[11] + 1;

  int rawtemp = 0;
  for (int i = 12; i < 24; ++i) {
     rawtemp = (rawtemp << 1) | buffer[i];
  }
  if (rawtemp & 0x800) {
    // extend MSB in the 12-bit rawtemp to the full int
    rawtemp |= 0xF000;
  }
  float temp = rawtemp / 10.0;
  int const_bits = 0;
  for (int i = 24; i < 28; ++i) {
    const_bits = (const_bits << 1) | buffer[i];
  }
  int humidity = 0;
  for (int i = 28; i < 36; ++i) {
    humidity = (humidity << 1) | buffer[i];
  }

  Serial.print("Ch ");
  Serial.print(channel);
  Serial.print(": ");
  Serial.print(temp, 1);
  Serial.print("°C ");
  Serial.print(humidity);
  Serial.print("%rH");
  if (const_bits != 0xF) {
    Serial.print(" const bits not 0xF");
  }
  if (header != 0x5B) {
    Serial.print(" header not 0x5B");
  }
  if (buffer[8] != 1 || buffer[9] != 0) {
    Serial.print(" 2nd header not 10");
  }
  Serial.println();

  // The data is sent twelve times. If we got here,
  // we received it successfully, so we can just wait
  // a bit to skip the next transmissions
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
}

