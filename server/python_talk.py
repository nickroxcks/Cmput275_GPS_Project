from serial import Serial
from time import sleep

if __name__ == "__main__":
    # timeout is in seconds, can specify a float like 4.5
    with Serial("/dev/ttyACM0", baudrate=9600, timeout=5) as ser:
        iteration = 0
        while True:
            # infinite loop that echoes all messages from
            # the arduino to the terminal
            line = ser.readline()
            # print("I read byte string:", line)

            if not line:
                print("timeout, restarting...")
                continue

            line_string = line.decode("ASCII")
            # print("This is the actual string:", line_string)
            # print("Stripping off the newline and carriage return")
            stripped = line_string.rstrip("\r\n")
            print("I read line: ", stripped)

            # print(len(line_string), len(stripped))

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
