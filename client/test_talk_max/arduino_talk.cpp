#include <Arduino.h>
void setup() {
  init();

  Serial.begin(9600);
}
// TODO Communicationseems to work. just need to findout if the the correct values are being sent
int main() {
  enum State {Sending_In, Waiting_N, Sending_A, Waiting_W, Ending};
  State curr_state = Sending_In;
  setup();
  Serial.println("Starting");
  char buffer[501];
  int paths;
  int N_path;
  int W_lat_long;
  int used = 0;
  int check;
  char lat[6];
  char lon[7];
  while (true) {
    while (Serial.available() == 0);
    if (curr_state == Ending){

      Serial.write("End");
      Serial.flush();

      break;
    }

    if (curr_state == Waiting_W){
      check = Serial.read();
      if (check == 'W'){
        W_lat_long = check;
        Serial.write('A');
        Serial.flush();
      }
      if (check == 'E'){
        curr_state = Ending;
      }
    }

    if (curr_state == Waiting_N){
      N_path = Serial.read();
      Serial.write(N_path);
      if (N_path == 'p'){
        Serial.write('A');
        Serial.flush();
        curr_state = Waiting_W;
      }
    }
    if (curr_state == Sending_In){
      Serial.println("R 5365486 -11333915 5364728 -11335891"); //Put input here
      Serial.flush();
      curr_state = Waiting_N;
    }

  }
  return 0;
}
