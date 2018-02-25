#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include "consts_and_types.h"
#include "map_drawing.h"

// the variables to be shared across the project, they are declared here!
shared_vars shared;

Adafruit_ILI9341 tft = Adafruit_ILI9341(clientpins::tft_cs, clientpins::tft_dc);

void setup() {
  // initialize Arduino
  init();

  // initialize zoom pins
  pinMode(clientpins::zoom_in_pin, INPUT_PULLUP);
  pinMode(clientpins::zoom_out_pin, INPUT_PULLUP);

  // initialize joystick pins and calibrate centre reading
  pinMode(clientpins::joy_button_pin, INPUT_PULLUP);
  // x and y are reverse because of how our joystick is oriented
  shared.joy_centre = xy_pos(analogRead(clientpins::joy_y_pin), analogRead(clientpins::joy_x_pin));

  // initialize serial port
  Serial.begin(9600);
  Serial.flush(); // get rid of any leftover bits

  // initially no path is stored
  shared.num_waypoints = 0;

  // initialize display
  shared.tft = &tft;
  shared.tft->begin();
  shared.tft->setRotation(3);
  shared.tft->fillScreen(ILI9341_BLUE); // so we know the map redraws properly

  // initialize SD card
  if (!SD.begin(clientpins::sd_cs)) {
    Serial.println("Initialization has failed. Things to check:");
    Serial.println("* Is a card inserted properly?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* Is the chipSelect pin the one for your shield or module?");

    while (1) {} // nothing to do here, fix the card issue and retry
  }

  // initialize the shared variables, from map_drawing.h
  // doesn't actually draw anything, just initializes values
  initialize_display_values();

  // initial draw of the map, from map_drawing.h
  draw_map();
  draw_cursor();

  // initial status message
  status_message("FROM?");
}

void process_input() {
  // read the zoom in and out buttons
  shared.zoom_in_pushed = (digitalRead(clientpins::zoom_in_pin) == LOW);
  shared.zoom_out_pushed = (digitalRead(clientpins::zoom_out_pin) == LOW);

  // read the joystick button
  shared.joy_button_pushed = (digitalRead(clientpins::joy_button_pin) == LOW);

  // joystick speed, higher is faster
  const int16_t step = 64;

  // get the joystick movement, dividing by step discretizes it
  // currently a far joystick push will move the cursor about 5 pixels
  xy_pos delta(
    // the funny x/y swap is because of our joystick orientation
    (analogRead(clientpins::joy_y_pin)-shared.joy_centre.x)/step,
    (analogRead(clientpins::joy_x_pin)-shared.joy_centre.y)/step
  );
  delta.x = -delta.x; // horizontal axis is reversed in our orientation

  // check if there was enough movement to move the cursor
  if (delta.x != 0 || delta.y != 0) {
    // if we are here, there was noticeable movement

    // the next three functions are in map_drawing.h
    erase_cursor();       // erase the current cursor
    move_cursor(delta);   // move the cursor, and the map view if the edge was nudged
    if (shared.redraw_map == 0) {
      // it looks funny if we redraw the cursor before the map scrolls
      draw_cursor();      // draw the new cursor position
    }
  }
}

void communicate(lon_lat_32 start, lon_lat_32 end){
  enum State {Sending_In, Waiting_N, Sending_A, Waiting_W, Ending};
  State curr_state = Sending_In;
  Serial.println("Starting");

  bool check = false;
  int N_path;
  int path = 0;
  int32_t W_lat_long = 0;
  int count = 1;
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
            shared.waypoints[path].lon = -1*W_lat_long;
            path += 1;
            if (path == N_path) {
              curr_state = Ending;
            }
            Serial.write('A');

            check = false;
            count = 1;
            W_lat_long = 0;
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
          count = 2;
          shared.waypoints[path].lat = W_lat_long;
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
            shared.num_waypoints = N_path;
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
      Serial.print("R ");
      Serial.print(start.lat);
      Serial.print(" ");
      Serial.print(start.lon);
      Serial.print(" ");
      Serial.print(end.lat);
      Serial.print(" ");
      Serial.println(end.lon);

      //Serial.println("R 5365486 -11333915 5364728 -11335891"); //Put input here
      Serial.flush();
      curr_state = Waiting_N;
    }

  }

}

int main() {
  setup();

  // very simple finite state machine:
  // which endpoint are we waiting for?
  enum {WAIT_FOR_START, WAIT_FOR_STOP} curr_mode = WAIT_FOR_START;

  // the two points that are clicked
  lon_lat_32 start, end;

  while (true) {
    // clear entries for new state
    shared.zoom_in_pushed = 0;
    shared.zoom_out_pushed = 0;
    shared.joy_button_pushed = 0;
    shared.redraw_map = 0;

    // reads the three buttons and joystick movement
    // updates the cursor view, map display, and sets the
    // shared.redraw_map flag to 1 if we have to redraw the whole map
    // NOTE: this only updates the internal values representing
    // the cursor and map view, the redrawing occurs at the end of this loop
    process_input();

    // if a zoom button was pushed, update the map and cursor view values
    // for that button push (still need to redraw at the end of this loop)
    // function zoom_map() is from map_drawing.h
    if (shared.zoom_in_pushed) {
      zoom_map(1);
      shared.redraw_map = 1;
    }
    else if (shared.zoom_out_pushed) {
      zoom_map(-1);
      shared.redraw_map = 1;
    }

    // if the joystick button was clicked
    if (shared.joy_button_pushed) {

      if (curr_mode == WAIT_FOR_START) {
        // if we were waiting for the start point, record it
        // and indicate we are waiting for the end point
        start = get_cursor_lonlat();
        curr_mode = WAIT_FOR_STOP;
        status_message("TO?");
        Serial.print("I read the start point (lon,lat) ");
        Serial.print(start.lon);
        Serial.print(",");
        Serial.print(start.lat);
        Serial.print(" which is in map coords(x,y): ");
        Serial.print(longitude_to_x(shared.map_number, start.lon));
        Serial.print(",");
        Serial.println(latitude_to_y(shared.map_number,start.lat));
        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      }
      else {
        // if we were waiting for the end point, record it
        // and then communicate with the server to get the path
        end = get_cursor_lonlat();
        // TODO: communicate with the server to get the waypoints
        Serial.print("I read the end point (lon,lat) ");
        Serial.print(end.lon);
        Serial.print(",");
        Serial.print(end.lat);
        Serial.print(" which is in map coords(x,y): ");
        Serial.print(longitude_to_x(shared.map_number, end.lon));
        Serial.print(",");
        Serial.println(latitude_to_y(shared.map_number,end.lat));

        status_message("Recieving Wavepoints...");
        Serial.print("This is map: ");
        Serial.println(shared.map_number);

        //SERVER HERE WILL MAKE AN ARRAY
        //and assign values to shared.num_waypoints
        //and shared.waypoints[]
        //Ex of the form:
        //shared.num_waypoints = 9;
        //shared.waypoints[0].lat = 5340149;
        //shared.waypoints[0].lon = -11329527;
        //etc....

        communicate(start, end);

        Serial.println(shared.waypoints[2].lat);
        //draw line from start to first Wave(if they are in the display)
        int32_t starty = latitude_to_y(shared.map_number,start.lat)-shared.map_coords.y;
        int32_t startx = longitude_to_x(shared.map_number,start.lon)-shared.map_coords.x;
        int32_t endy = latitude_to_y(shared.map_number,shared.waypoints[0].lat)-shared.map_coords.y;
        int32_t endx = longitude_to_x(shared.map_number,shared.waypoints[0].lon)-shared.map_coords.x;

        if(((shared.map_coords.x <= (startx+shared.map_coords.x))&&((startx+shared.map_coords.x) <= (shared.map_coords.x+320)))
        && ((shared.map_coords.y <= (starty+shared.map_coords.y))&&((starty+shared.map_coords.y)<=(shared.map_coords.y+216)))
        && ((shared.map_coords.x <= (endx+shared.map_coords.x))&&((endx+shared.map_coords.x) <= (shared.map_coords.x+320)))
        && ((shared.map_coords.y <= (endy+shared.map_coords.y))&&((endy+shared.map_coords.y)<=(shared.map_coords.y+216)))){

          shared.tft-> drawLine(longitude_to_x(shared.map_number, start.lon)-shared.map_coords.x,
          latitude_to_y(shared.map_number,start.lat)-shared.map_coords.y,
          longitude_to_x(shared.map_number,shared.waypoints[0].lon)-shared.map_coords.x,
          latitude_to_y(shared.map_number, shared.waypoints[0].lat)-shared.map_coords.y,ILI9341_BLUE);
        }

        //Draw lines in between the wavepoints
        for(int k = 0; k < (shared.num_waypoints-1);k++){
          //calling these a seperate variable so don't have to keep wrting these entire lines
          int32_t startwavey = latitude_to_y(shared.map_number,shared.waypoints[k].lat)-shared.map_coords.y;
          int32_t startwavex = longitude_to_x(shared.map_number,shared.waypoints[k].lon)-shared.map_coords.x;
          int32_t endwavey = latitude_to_y(shared.map_number,shared.waypoints[k+1].lat)-shared.map_coords.y;
          int32_t endwavex = longitude_to_x(shared.map_number,shared.waypoints[k+1].lon)-shared.map_coords.x;

          //if the 2 Wavepoints are is in the same screen as the display(shared.map_coords is the top left of the display)
          //We add 320 as x spams to the width of the display and add 216 becuase the y spams to (240-24)becuase of the
          //message prompt
          if(((shared.map_coords.x <= (startwavex+shared.map_coords.x))&&((startwavex+shared.map_coords.x) <= (shared.map_coords.x+320)))
          && ((shared.map_coords.y <= (startwavey+shared.map_coords.y))&&((startwavey+shared.map_coords.y)<=(shared.map_coords.y+216)))
          && ((shared.map_coords.x <= (endwavex+shared.map_coords.x))&&((endwavex+shared.map_coords.x) <= (shared.map_coords.x+320)))
          && ((shared.map_coords.y <= (endwavey+shared.map_coords.y))&&((endwavey+shared.map_coords.y)<=(shared.map_coords.y+216)))){

            shared.tft-> drawLine(startwavex,startwavey,endwavex,endwavey,ILI9341_BLUE);

          }
        }

        //Draw last line between last wavepoint and destination, if in the display range
        starty = latitude_to_y(shared.map_number,shared.waypoints[shared.num_waypoints-1].lat)-shared.map_coords.y;
        startx = longitude_to_x(shared.map_number,shared.waypoints[shared.num_waypoints-1].lon)-shared.map_coords.x;
        endy = latitude_to_y(shared.map_number,end.lat)-shared.map_coords.y;
        endx = longitude_to_x(shared.map_number,end.lon)-shared.map_coords.x;

        if(((shared.map_coords.x <= (startx+shared.map_coords.x))&&((startx+shared.map_coords.x) <= (shared.map_coords.x+320)))
        && ((shared.map_coords.y <= (starty+shared.map_coords.y))&&((starty+shared.map_coords.y)<=(shared.map_coords.y+216)))
        && ((shared.map_coords.x <= (endx+shared.map_coords.x))&&((endx+shared.map_coords.x) <= (shared.map_coords.x+320)))
        && ((shared.map_coords.y <= (endy+shared.map_coords.y))&&((endy+shared.map_coords.y)<=(shared.map_coords.y+216)))){

          shared.tft-> drawLine(startx,starty,endx,endy,ILI9341_BLUE);
        }

        // now we have stored the path length in
        // shared.num_waypoints and the waypoints themselves in
        // the shared.waypoints[] array, switch back to asking for the
        // start point of a new request


        //LEAVE THIS START
        curr_mode = WAIT_FOR_START;
        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) {}
      }
    }


    if (shared.redraw_map) {
      // redraw the status message
      Serial.print("top left of screen x coord: ");
      Serial.println(shared.map_coords.x);
      Serial.print("top left of screen y coor: ");
      Serial.println(shared.map_coords.y);
      if (curr_mode == WAIT_FOR_START) {
        status_message("FROM?");
      }
      else {
        status_message("TO?");
      }

      // redraw the map and cursor
      draw_map();
      draw_cursor();

      // TODO: draw the route if there is one


    }

  }

  Serial.flush();
  return 0;
}
