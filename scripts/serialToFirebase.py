import serial

port = ""
baud = 9060


try:
    with serial.Serial(port, baud, timeout=1) as ser:
        print(f"Connected to {port}")
        while True:
            line = ser.readline().decode("utf-8").strip()
            if line:
                print(line)
except serial.SerialException as e:
    print("Error:", e)




def convertFormat(format):
    pass