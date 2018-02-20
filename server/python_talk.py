from serial import Serial
from time import sleep
# screen /dev/cu.usbmodemFD121
if __name__ == "__main__":
    # timeout is in seconds, can specify a float like 4.5
    received = []
    with Serial("/dev/tty.usbmodemFA131", baudrate=9600, timeout=5) as ser:
        iteration = 0
        path = 8 # temp
        while True:
            # infinite loop that echoes all messages from
            # the arduino to the terminal
            line = ser.readline()

            if not line:
                print("timeout, restarting...")
                continue

            line_string = line.decode("ASCII")

            stripped = line_string.rstrip("\r\n")
            print("I read line: ", stripped)

            # construct the line you want to print to the
            # Arduino, don't forget the newline
            out_line = "Iteration " + str(iteration) + "\n"
            iteration += 1

            encoded = out_line.encode("ASCII")
            # now encoded is a byte object we can
            # write to the arduino

            ser.write(encoded)

            # rest a bit between rounds of communication
            sleep(2)

            stripped.strip()
            if stripped[0] == 'R':
                received.append = stripped[1]
                received.append = stripped[2]
                received.append = stripped[3]
                received.append = stripped[4]
                encoded = 'N' + len(path)
                encoded = out_line.encode("ASCII")
                ser.write(encoded)
            if stripped[0] == 'A':
                path = 'W' + path
