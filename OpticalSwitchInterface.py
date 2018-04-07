class Optical_Switch(object):
    def __init__(self,ser):
        self.command = ""
        self.serial = ser
    def get_last_command(self):
        return self.command
    def scan_all(self):
        self.command = ("1" + "\n" + "\r").encode()
        self.serial.write(self.command)
    def scan_all_continuously(self):
        self.command = (("1,1") + "\n" + "\r").encode()
        self.serial.write(self.command)
    def scan_one(self, port):
        self.command = ((("2,%d") % port) + "\n" + "\r").encode()        
        self.serial.write(self.command)
    def scan_one_continously(self, port):
        self.command = ((("2,%d,1") % port) + "\n" + "\r").encode()
        self.serial.write(self.command)
    def scan_range(self, port1, port2):
        self.command = (("3,%d,%d") % (port1,port2) + "\n" + "\r").encode()
        self.serial.write(self.command)
    def scan_range_continously(self, port1, port2):
        self.command = (("3,%d,%d,1") % (port1,port2) + "\n" + "\r").encode()
        self.serial.write(self.command)

import serial
import time

if __name__ == '__main__':
    arduino = serial.Serial('/dev/ttyACM0', 9600)
    myswitch = Optical_Switch(arduino)
    myswitch.scan_all()
    time.sleep(2)
    myswitch.scan_range(3,50)
    time.sleep(2)
    myswitch.scan_one(4)
    time.sleep(2)
    myswitch.scan_range_continously(4,16)
    time.sleep(2)
    print(myswitch.get_last_command())
