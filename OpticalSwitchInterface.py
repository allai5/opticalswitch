import serial
import time
class Optical_Switch(object):
    def __init__(self,ser):
        self.command = "" #command to send to Arduino serial
        self.serial = ser #serial port
    def get_last_command(self)
        return self.command
    def setcam_delay(self, before, after): #not done yet #set delays for camera to take picture
        self.command = (("5,%d,%d") % (before,after) + "\n" + "\r").encode()
        self.serial.write(self.command)
    def debug(self): #print out status of the BUSY signal, bytes of the data sent out and data read, and communication status
        self.command = ("4" + "\n" + "\r").encode()
        self.serial.write(self.command)   
        data = "" 
        for i in range(50):
            if self.serial.inWaiting():
                '''while self.serial.inWaiting():
                    data += (self.serial.read().decode())    
                print(data)'''
                while (self.serial.inWaiting()):
                    print(self.serial.readline().decode(), end = "")
                break
            else:
                time.sleep(0.1)
        self.serial.flushInput()
    def scan_all(self):
        self.command = ("1" + "\n" + "\r").encode()
        self.serial.write(self.command)
        for i in range(50):
            if self.serial.inWaiting():
                '''while self.serial.inWaiting():
                    data += (self.serial.read().decode())    
                print(data)'''
                while (self.serial.inWaiting()):
                    print(self.serial.readline().decode(), end = "")
                break
            else:
                time.sleep(0.1)
        self.serial.flushInput()

    def scan_all_continuously(self, t):
        timeout = time.time() + t
        self.command = (("1,1" + "\n"+"\r")).encode()
        while True:
            if(time.time() > timeout):
                break
            self.serial.write(self.command)
            for i in range(50):
                if self.serial.inWaiting():
                    while (self.serial.inWaiting()):
                        print(self.serial.readline().decode(), end = "")
                    break
                else:
                    time.sleep(0.1)
            self.serial.flushInput()
             
    def scan_one(self, port):
        self.command = ((("2,%d") % port) + "\n" + "\r").encode()        
        self.serial.write(self.command)
        for i in range(50):
            if self.serial.inWaiting():
                while (self.serial.inWaiting()):
                    print(self.serial.readline().decode(), end = "")
                break
            else:
                time.sleep(0.1)
        self.serial.flushInput()
    def scan_range(self, port1, port2):
        self.command = (("3,%d,%d") % (port1,port2) + "\n" + "\r").encode()
        self.serial.write(self.command)
        for i in range(50):
            if self.serial.inWaiting():
                while (self.serial.inWaiting()):
                    print(self.serial.readline().decode(), end = "")
                break
            else:
                time.sleep(0.1)
        self.serial.flushInput()
    def scan_range_continously(self, port1, port2, t):
        timeout = time.time() + t
        self.command = (("3,%d,%d,1") % (port1,port2) + "\n" + "\r").encode()
        while True:
            if(time.time() > timeout):
                break
            self.serial.write(self.command)
            for i in range(50):
                if self.serial.inWaiting():
                    while (self.serial.inWaiting()):
                        print(self.serial.readline().decode(), end = "")
                    break
                else:
                    time.sleep(0.1)
            self.serial.flushInput()

if __name__ == '__main__':
    arduino = serial.Serial(
        port = '/dev/ttyUSB0',
        baudrate = 9600, #baud rate
        parity = serial.PARITY_ODD,
        stopbits = serial.STOPBITS_TWO,
        bytesize = serial.EIGHTBITS,
        timeout=None 
    )
    myswitch = Optical_Switch(arduino)
    myswitch.serial.flushInput() #idk this is for intialization
    print(myswitch.serial.readline().decode()) #print START    

    myswitch.scan_all()
    myswitch.scan_one(4)
    myswitch.scan_all_continuously(4)
    myswitch.scan_all()
    myswitch.scan_range_continously(1,5,5)
    myswitch.scan_one(3)
    myswitch.serial.close()
