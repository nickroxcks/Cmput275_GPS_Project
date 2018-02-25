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

  bool check = false;
  int N_path;
  int path = 0;
  int W_lat_long;
  int count = 0;
  char incomingByte;
  while (true) {
    while (Serial.available() == 0);


    if (curr_state == Ending){
      Serial.write("End");
      Serial.flush();
      break;
    }
    if (curr_state == Waiting_W){
      while(true){
        incomingByte = Serial.read();
        if (incomingByte == '\n') {
          if (check){
            // shared.waypoints[path].lon = W_lat_long;
            path += 1;
            if (path == N_path) {
              curr_state = Ending;
            }
            Serial.write('A');
            check = false;
          }
          break;
        }
        if (incomingByte == -1) continue;
        if (incomingByte == '%') continue;
        if (incomingByte == '-') continue;
        if (incomingByte == 'W') {
          check = true;
          continue;
        }
        if ((incomingByte == ' ') && (count == 1)) {
          count = 0;
          continue;
        }
        if ((incomingByte == ' ') && (count == 0)) {
          count = 1;
          // shared.waypoints[path].lat = W_lat_long;
          W_lat_long = 0;
          continue;
        }

        W_lat_long *= 10;
        W_lat_long += (incomingByte - 48);

      }
    }


    if (curr_state == Waiting_N){
      while (true){
        incomingByte = Serial.read();
        if (incomingByte == '\n') {
          if (check){
            //shared.num_waypoints = N_path;
            curr_state = Waiting_W;
            Serial.write('A');
            check = false;
          }
          break;
        }
        if (incomingByte == -1) continue;
        if (incomingByte == '%') continue;
        if (incomingByte == 'N') {
          check = true;
          continue;
        }
        if (incomingByte == ' ') continue;
        N_path = incomingByte - 48;
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
