# Cmput275Assignmentone
Nicholas Serrano 1508361
Maxwell McEvoy 1288336

This is a program which takes an arduino wired up to a Adrafruit display with a joystick, and loads the whole map of edmonton via sd card and allows the user to travel around the map of edmonton(displayed on the display) with a cursor controled by the joystick, and will find the shortest route between any two places on the map that the user selects. Zoom in functions are included and are done through push buttons.

In this Assignment, the Arduino acts as a client, in which user can select two points on the arduino and send the data. Your computer will act as the server, and will calculate the shortest route from the two selected points and send the data back to the arduino to draw on the display the shortest route.

Arduino wiring instructions:
 
 Wiring of adrafruit display: from bottom to top on the right side (the side with the pins).

    Board Pin <---> Arduino Pin
    ===========================
    GND             GND
    Vin             5V
    3Vo             NOT CONNECTED
    CLK             52
    MISO            50
    MOSI            51
    CS              10
    D/C             9
    RST             NOT CONNECTED
    Lite            NOT CONNECTED
    Y+              A2 (analog pin)
    X+              4  (digital pin)
    Y-              5  (digital pin)
    X-              A3 (analog pin)
    IM0 - IM3       NOT CONNECTED (they expect a 3.3v signal, DON'T CONNECT!)
    CCS             6
    CD              NOT CONNECTED
    
 Wiring of joystick:
 
    Board Pin <---> Arduino Pin
    ===========================
    GND             GND
    5v              5v
    VRx             A1
    Vry             A0
    SW              8
    
 Wiring of pushbuttons:
 
    GND <--> zoomoutbutton <--> Arduinopin 3
    GND <--> zoominbutton <--> Arduinopin 2

How to run code:

- First go into the client directory and make upload the client code to the arduino.
  Keep in mind when uploading your code, our server expects you to be ported in
  /dev/ttyACM0 . If you are not ported here, either change to the correct port or edit
  line 224 in server.py to the desired port.

- Afterwards with your arduino plugged in, go into the server directory and run the 
  python script server.py. This will boot up the server and you will see a bunch of
  timeout messages waiting for you to input two coordinates on the arduino
  
- You can now scroll around the map of edmonton using the joystick. Moving to the edges 
  of the screen will redraw the next portion of the map in the direction you nudged.
  You may also zoom in and zoom out the map by pressing eihter of the zoom push buttons. 
  Scrolling at closer zoom ins however may be a little slower than other zooms.
  A prompt is being displayed asking you where you want to draw your shortest route from.
  Select a location on the map by clicking the joystick, and then select another location you 
  want the path to go to. Here you will see message "recieving waypoints". Now what will happen 
  is the server will take your selected points and use dijkstra's algorithm to create a route.
  You will see serveral "A" being displayed on the server, which is just the arduino and 
  server communicating. Once a shortest route has been found, the route will be drawed to the 
  screen and you can scroll or zoom in and out to follow the drawn route to your destination.
  
- If there is no route between your 2 points you have selected, or there is over 500 waypoints in 
  your route, the server will timeout you will regain control of you cursor to select another 2 points. 
 
 -Longer routes may take longer for Arduino and server to communicate the entire path
 
Assumptions and other key notes:

 - No real uncertainties or assumptions made. Just remember to assign your ports correctly. 
 
 - **No real bugs were found when testing. If you select a point not on the road, we will just 
   take the point on a road closest to you. I have added in this case a extra line drawn, directing 
   you to the next closest point we are drawing the path from. This extra line is just there to make 
   the route look like its fully connected. 
   
Things changed in client.cpp:

- Added function "communicate" which is where all our communication bewtween arduino and server occurs.
  Function is defined above main function. This function is called in main where the first "TODO" was 
  commented, in the else statement after the  joystick has been pressed a second time
  
- Added function "drawingPath", which takes our wavepoint array and draws the found path from the 
  server to the current display. This function is defined above main and is called in main right under 
  where we called  communicate as described above. Note that right before this function is called I've 
  called the function draw_map and draw_cursor just in case a path has been drawn previously so that the 
  old path gets erased. Then changed status prompt back to "From". drawingPath is also called in main 
  again under the if statement which redraws the map(the other TODO). This is called here again in order 
  to draw the the path again when the map scrolls over or is zoomed in and out. 
  
- 
