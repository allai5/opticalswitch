class Optical_Switch(object):
    def __init__(self,ser):
        self.command = ""
        self.serial = ser
    def get_last_command(self):
        return self.command
    def scan_all(self):
        self.command = b"1"
        self.serial.write(self.command)
    def scan_all_continuously(self):
        self.command = b"1,1"
        self.serial.write(self.command)
    def scan_one(self, port):
        self.command = (b"2,%d") % port
        self.serial.write(self.command)
    def scan_one_continously(self, port):
        self.command = (b"2,%d,1") % port
        self.serial.write(self.command)
    def scan_range(self, port1, port2):
        self.command = (b"3,%d,%d") % (port1,port2)
        self.serial.write(self.command)
    def scan_range_continously(self, port1, port2):
        self.command = (b"3,%d,%d,1") % (port1,port2)
        self.serial.write(self.command)

#carriage return + convert everything to bytes
import serial
if __name__ == '__main__':
    arduino = serial.Serial('/dev/ttyACM0', 9600)
    myswitch = Optical_Switch(arduino)
    myswitch.scan_one(3)
    print(myswitch.get_last_command())
    myswitch.scan_all_continuously()
    print(myswitch.get_last_command())
    myswitch.scan_range_continously(3,50)
    print(myswitch.get_last_command())