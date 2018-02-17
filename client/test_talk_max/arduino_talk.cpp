#include <Arduino.h>

void setup() {
  init();

  Serial.begin(9600);
}

int main() {
  setup();

  Serial.println("Hello, this is the Arduino!");
  Serial.flush();

  char buffer[129];
  int used = 0;

  while (true) {
    while (Serial.available() == 0);

    buffer[used] = Serial.read();
    ++used;

    if (buffer[used-1] == '\n') {
      buffer[used-1] = buffer[used] = buffer[used+1] = '.';
      buffer[used+2] = '\n';
      buffer[used+3] = '\0';
      Serial.write("Test"); // does not add \r\n
      used = 0;
    }
  }

  return 0;
}
