#include <SD.h>
#define CSPIN 4

// Set to 1 when SD card detected
int SDpresent = 0;
// Used in name creation of images
int imageNo = 0;

//******************************************************
// Connections :
// RX from MAX3232 to Arduino Serial 1 (TX 1)
// TX from MAX3232 to Arduino Serial 1 (RX 1)
// Vcc from camera to ARduino +5v
// Gnd from camera to Arduino Gnd
//*******************************************************
byte ZERO = 0x00;
byte incomingbyte;
long int j=0,k=0,count=0,i=0x0000;
uint8_t MH,ML;
boolean EndFlag=0;
 
void SendResetCmd()
{
    Serial1.write(0x56);
    Serial1.write(ZERO);
    Serial1.write(0x26);
    Serial1.write(ZERO);
}

void SendResetCmd2()
{
    Serial2.write(0x56);
    Serial2.write(ZERO);
    Serial2.write(0x26);
    Serial2.write(ZERO);
}
 
/*************************************/
/* Set ImageSize :
/* <1> 0x22 : 160*120
/* <2> 0x11 : 320*240
/* <3> 0x00 : 640*480
/* <4> 0x1D : 800*600
/* <5> 0x1C : 1024*768
/* <6> 0x1B : 1280*960
/* <7> 0x21 : 1600*1200
/************************************/
void SetImageSizeCmd(byte Size)
{
    Serial1.write(0x56);
    Serial1.write(ZERO);  
    Serial1.write(0x54);
    Serial1.write(0x01);
    Serial1.write(Size);
}
 
/*************************************/
/* Set BaudRate :
/* <1>¡¡0xAE  :   9600
/* <2>¡¡0x2A  :   38400
/* <3>¡¡0x1C  :   57600
/* <4>¡¡0x0D  :   115200
/* <5>¡¡0xAE  :   128000
/* <6>¡¡0x56  :   256000
/*************************************/
void SetBaudRateCmd(byte baudrate)
{
    Serial1.write(0x56);
    Serial1.write(ZERO);
    Serial1.write(0x24);
    Serial1.write(0x03);
    Serial1.write(0x01);
    Serial1.write(baudrate);
}
 
 void SetBaudRateCmd2(byte baudrate)
{
    Serial2.write(0x56);
    Serial2.write(ZERO);
    Serial2.write(0x24);
    Serial2.write(0x03);
    Serial2.write(0x01);
    Serial2.write(baudrate);
}
void SendTakePhotoCmd()
{
    Serial1.write(0x56);
    Serial1.write(ZERO);
    Serial1.write(0x36);
    Serial1.write(0x01);
    Serial1.write(ZERO); 
}
 
void SendTakePhotoCmd2()
{
    Serial2.write(0x56);
    Serial2.write(ZERO);
    Serial2.write(0x36);
    Serial2.write(0x01);
    Serial2.write(ZERO); 
}

void SendReadDataCmd()
{
    MH=i/0x100;
    ML=i%0x100;
    Serial1.write(0x56);
    Serial1.write(ZERO);
    Serial1.write(0x32);
    Serial1.write(0x0c);
    Serial1.write(ZERO);
    Serial1.write(0x0a);
    Serial1.write(ZERO);
    Serial1.write(ZERO);
    Serial1.write(MH);
    Serial1.write(ML);
    Serial1.write(ZERO);
    Serial1.write(ZERO);
    Serial1.write(ZERO);
    Serial1.write(0x20);
    Serial1.write(ZERO);
    Serial1.write(0x0a);
    i+=0x20;
}
 
void SendReadDataCmd2()
{
    MH=i/0x100;
    ML=i%0x100;
    Serial2.write(0x56);
    Serial2.write(ZERO);
    Serial2.write(0x32);
    Serial2.write(0x0c);
    Serial2.write(ZERO);
    Serial2.write(0x0a);
    Serial2.write(ZERO);
    Serial2.write(ZERO);
    Serial2.write(MH);
    Serial2.write(ML);
    Serial2.write(ZERO);
    Serial2.write(ZERO);
    Serial2.write(ZERO);
    Serial2.write(0x20);
    Serial2.write(ZERO);
    Serial2.write(0x0a);
    i+=0x20;
}
void StopTakePhotoCmd()
{
    Serial1.write(0x56);
    Serial1.write(ZERO);
    Serial1.write(0x36);
    Serial1.write(0x01);
    Serial1.write(0x03);
}
void StopTakePhotoCmd2()
{
    Serial2.write(0x56);
    Serial2.write(ZERO);
    Serial2.write(0x36);
    Serial2.write(0x01);
    Serial2.write(0x03);
}

void setup()
{
    Serial.begin(38400);
    while (!Serial)
    {
        ; // wait for Serial1 port to connect. Needed for Leonardo only
    }
 
    //Serial.println("initialization done.");
    //Serial.println("please waiting ....");
 
 
   //Serial.println("Initializing Serial for Cam # 1");
   
    Serial1.begin(115200);
    delay(100);
    SendResetCmd();
    delay(2000);
    SetBaudRateCmd(0x2A);
    delay(500);
    // Serial1.begin(115200);
    Serial1.begin(38400);
    delay(100);
    
    //Serial.println("Initializing Serial for Cam # 2");
    Serial2.begin(115200);
    delay(100);
    SendResetCmd2();
    delay(2000);
    SetBaudRateCmd2(0x2A);
    delay(500);
    // Serial1.begin(115200);
    Serial2.begin(38400);
    delay(100);
    
    // SD Card Initialization
    
    // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
    // Note that even if it's not used as the CS pin, the hardware SS pin 
    // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
    // or the SD library functions will not work.
    pinMode(53, OUTPUT);
    
    if (SD.begin(CSPIN))
    {
      SDpresent = 1;
//      if(!SD.exists("Log.txt"))
//      {
//         File file = SD.open("Log.txt");
//         file.close();
//      } 
    }
    
}
 
void DoCameraOneWork()
{
  byte a[32];
  
  SendResetCmd();
  delay(2000);                            //Wait 2-3 second to send take picture command
  SendTakePhotoCmd();
  delay(1000);
  
  while(Serial1.available() > 0)
  {
    incomingbyte = Serial1.read();
  }

  EndFlag = 0;
  i = 0x0000;
  String str = "picture-" + String(imageNo++) + ".jpg";
  char* filename = (char*) malloc(str.length());
  str.toCharArray(filename, str.length());
  File file = SD.open(filename, FILE_WRITE);
  //file.println("\nNext Image (Camera 1)\n");
  
  while(!EndFlag)
  {  
    j=0;
    k=0;
    count=0;
    SendReadDataCmd();
    
    delay(20);
    
    while(Serial1.available()>0)
    {
      incomingbyte = Serial1.read();
      k++;
      delay(1); //250 for regular
      if( (k > 5) && (j < 32) && (!EndFlag) )
      {
        a[j] = incomingbyte;
        
        if( (a[j-1] == 0xFF) && (a[j] == 0xD9) )     //tell if the picture is finished
        {
          EndFlag=1;
        }
        j++;
        count++;
      }
    } // 'a' has 'count' amount of valid data of the image
    
    // Send all all the image data to the port immediately
    Serial.write(a, count);
    for (int idx = 0; idx < count; idx++)
      file.print(a[idx], HEX);
  } // The complete JPEG file has been transferred
  
  file.close();
}

void DoCameraTwoWork()
{
  byte a[32];
  
  SendResetCmd2();
  delay(2000);                            //Wait 2-3 second to send take picture command
  SendTakePhotoCmd2();
  delay(1000);
  
  while(Serial2.available() > 0)
  {
    incomingbyte = Serial2.read();
  }

  EndFlag = 0;
  i = 0x0000;
  String str = "picture-" + String(imageNo++) + ".jpg";
  char* filename = (char*) malloc(str.length());
  str.toCharArray(filename, str.length());
  File file = SD.open(filename, FILE_WRITE);
//  file.println("\nNext Image (Camera 2)\n");
  
  while(!EndFlag)
  {  
    j=0;
    k=0;
    count=0;
    SendReadDataCmd2();
    
    delay(20);
    
    while(Serial2.available()>0)
    {
      incomingbyte = Serial2.read();
      k++;
      delay(1); //250 for regular
      if( (k > 5) && (j < 32) && (!EndFlag) )
      {
        a[j] = incomingbyte;
        
        if( (a[j-1] == 0xFF) && (a[j] == 0xD9) )     //tell if the picture is finished
        {
          EndFlag=1;
        }
        j++;
        count++;
      }
    } // 'a' has 'count' amount of valid data of the image
    
    // Send all all the image data to the port immediately
    Serial.write(a, count);
    for (int idx = 0; idx < count; idx++)
      file.print(a[idx], HEX);
  } // The complete JPEG file has been transferred

  file.close();
}
void loop()
{
   while(Serial.available() > 0)
   {
     byte input = Serial.read();
     
     switch(input)
     {
       case 0x31:
       DoCameraOneWork();
       break;
       
       case 0x32:
       DoCameraTwoWork();
       break;
       
       default:
       break;
     }
   }
}
