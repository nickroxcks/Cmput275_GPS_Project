# Cmput275Assignmentone
Nicholas Serrano 1508361
Max -----------

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
 joy GND <---> arduino GND
 joy +5v <---> 
    
