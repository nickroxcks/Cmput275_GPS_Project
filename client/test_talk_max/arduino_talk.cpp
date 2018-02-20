#include <Arduino.h>

void setup() {
  init();

  Serial.begin(9600);
}
// TODO Communicationseems to work. just need to findout if the the correct values are being sent
int main() {
  enum State {Input_Ready, Sending_In, Waiting_N, Sending_A, Waiting_W, Ending};
  State curr_state = Sending_In;
  setup();
  Serial.println("Starting");
  char buffer[129];
  int used = 0;
  int paths;
  int output;
  while (true) {
    while (Serial.available() == 0);
    if (curr_state == Sending_In){
      Serial.println("R 5365486 -11333915 5364728 -11335891"); //Put input here
      Serial.flush();
      curr_state = Waiting_N;
    }
    if (curr_state == Waiting_N){
      buffer[used] = Serial.read();
      if (buffer[0] == 'N'){
        output = Serial.read();
        Serial.write('A');
        curr_state = Waiting_W;
      }
    }
    if (curr_state == Waiting_W){
      buffer[used] = Serial.read();
      if (buffer[0] == 'W'){
          Serial.write('A');
      }
      if (buffer[0] == 'E'){
        curr_state = Ending;
        Serial.flush();
      }
    }
    if (curr_state == Ending){

      break;
    }
  }
  return 0;
}
