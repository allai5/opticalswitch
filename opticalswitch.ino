/*Arduino is an open source platform commonly used across the world to build prototypes electronics projects. 
 * Arduino consists of a physical board (the microcontroller) and the integrated desktop environment 
 * (Arduino IDE) used to write and upload code to the microcontroller. The microcontroller is able to read and write 
 * digital/analog signals to devices in the real world through digital/analog pins. Common applications include reading from sensors,
 * The Arduino does not need a separate programmer in order to upload code - only a USB cable is needed.
 
 * This code is used to run on the Arduino Uno to synchronize the camera and the optical switch. First, the Arduino
 * sends an I2C write command to the first optical fiber and reads the I2C read response to see if communication between the fiber and the Arduino
 * is successful. The 2nd byte in the I2C read response determines if communication is successful or not.  
 * Once communication is established to be successful, the Arduino reads the optical switch's BUSY signal, so that the Arduino 
 * knows when to send the digital signal to the camera to take a picture (the Arduino waits until it is not busy to tell the camera to take a picture).
 * After the camera takes a picture, the Arduino switches to the next optical fiber and repeats again until it reaches the last fiber.
 * Appropriate delays are used to ensure the validity of the data that the Arduino receives from the optical switch.
*/

#include <Wire.h> //this is the Arduino library used for I2C communication

int camera = 2; //set digital pin to write digital signals to camera
int address = 0x73; //device address 115
int channel = 0x78; //set output channel (command)
int busy = 7; //BUSY signal from optical switch to let us know when to send the next command
byte data_out[4]; //byte array of command to send to optical switch
byte data_in[4]; //byte array of data read from optical switch
static int status = 0; //whether or not sent data was a success or not (0 means success, 1 means failure)....
static int crclow = 0, crchigh = 0; //two bytes for the CRC16 function (checksum)


void setup() {
  pinMode(camera,OUTPUT); //we want to send an OUTPUT signal to the camera to take a picture
  pinMode(busy, INPUT); //we want to read the "busy" INPUT signal to see if we can send the command
  Wire.begin(); //initialize I2C communication 
  Serial.begin(9600); //initialize serial monitor (for debugging and tracking purposes)
  
  //initialize the optical switch setup
  readData();
  delay(10);
  sendData(0x05); //port number
  delay(10);

  while (status == 0) { //while the sent data was successful
    for (int port = 1; port < 65; port++) { //64 ports in the optical switch
      if (digitalRead(busy) == 1){ //digitalRead(busy) = 0: port is busy | digitalReady(busy) = 1: port is busy
        sendData(port); //send command to port 
      }
      delay(9); //delay for 9 milliseconds
      //Serial.println(digitalRead(busy));
      if (digitalRead(busy) == 1) { //after port is not busy again, read the data (last byte) to see if sent data was successful
        readData();
      }
      delay(1);
      if (digitalRead(busy) == 1 && status == 0) { //if sent data was successful and port not busy
        digitalWrite(camera, HIGH); //camera take picture
        delay(1);
        digitalWrite(camera, LOW);
        delay(10); //add time for camera to take picture
        Serial.print(port); //print the port the optical switch is currently switched to
        Serial.print(" GO");
        Serial.print(" CAM");
        Serial.print("\n");
      } else { 
        Serial.print(port);
        Serial.print(" error");
        Serial.print("\n");
      }
      
    }
    break;    
  }

  Serial.println(""); //spacing
  Serial.println("END");
  
  /*for (int i = 0; i < 4; i++) {
    Serial.print(data_out[i]); 
    Serial.print(" ");
    Serial.print("\n");
  }  

  Serial.println("");
  
  for (int i = 0; i < 4; i++) {
    Serial.print(data_in[i]); 
    Serial.print(" ");
    Serial.print("\n");
  }  */
}

void loop() {
  
}

void readData() {
  Wire.requestFrom(address, 4); //request 4 bytes from optical switch
  if (4 <= Wire.available()) {
    for (int i = 0; i < 4; i++) {
      data_in[i] = Wire.read(); //populate data_in array with 4 bytes
      //Serial.println(data_in[i]);
    } 
  } else {
      Serial.println("not good");
  }

  status = data_in[1];  //2nd byte is the status
  
}

void sendData(int port) {
  int arr[3] = {230, channel, port}; //sequence from datasheet
  crc16(arr); //call checksum function
  data_out[0] = channel; //command for sending data
  data_out[1] = port; //port you want to send data to
  data_out[2] = crclow; //checksum byte 1
  data_out[3] = crchigh; //checksum byte 2

  Wire.beginTransmission(address); //prepare to send data to optical switch at address
  Wire.write(data_out,4); //write out byte array
  Wire.endTransmission(); //end i2c writing
}

//this is the checksum function taken off of the datasheet
void crc16(int data[]) {
  uint16_t crc_table[]={0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040};
  uint16_t crc = 0xFFFF;
  //Serial.println(sizeof(data));
  for (int i = 0; i <= sizeof(data); i++) {
    uint16_t w = data[i];
    //Serial.println(w);
    uint16_t temp = w ^ crc;
    //Serial.println(temp);
    crc >>= 8;
    crc ^= crc_table[temp&255];
  }
   crclow = (crc>>8)&255;
   crchigh = crc&255;
}
