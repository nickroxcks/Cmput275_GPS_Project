from serial import Serial
from time import sleep

temp = ["W 5365488 -11333914",
        "W 5365238 -11334423"]

if __name__ == "__main__":
    # timeout is in seconds, can specify a float like 4.5
    with Serial("/dev/ttyACM3", baudrate=9600, timeout=5) as ser:
        iteration = 0
        while True:
            line = ser.readline()
            if not line:
                print("timeout, restarting...")
                continue
            line_string = line.decode("ASCII")
            stripped = line_string.rstrip("\r\n")
            print("I read line: ", stripped)
            if stripped[0] == 'R':
                if not temp:
                    a = "N 0\n"
                    encoded = a.encode("ASCII")
                    ser.write(encoded)
                    break
                else:
                    a = "N 8\n"
                    encoded = a.encode("ASCII")
                    ser.write(encoded)
                    continue
            elif stripped[0] == 'A':
                if temp:
                    a = temp.pop(0)
                else:
                    a = "E \n"
                encoded = a.encode("ASCII")
                ser.write(encoded)
                continue
            else:
                out_line = "Iteration " + str(iteration) + "\n"
                iteration += 1
                encoded = out_line.encode("ASCII")
                ser.write(encoded)

            sleep(2)
