#include <Servo.h>
Servo servoo;

#include <String.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

byte master[4] = {0xDA, 0x4F ,0x11,0xB1};

#define green 6
#define red 7
#define buzzer 5

void setup() 
{
    pinMode(green,OUTPUT);
    pinMode(red,OUTPUT);
    pinMode(buzzer,OUTPUT);
    Serial.begin(9600);
    servoo.attach(3);
    servoo.write(0);

    SPI.begin(); // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522

}

void loop() 
{
    Serial.begin(9600);

    if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
    {
        for (byte i = 0; i < 4; i++) 
            nuidPICC[i] = rfid.uid.uidByte[i];
        
        Serial.println(nuidPICC[1]);

        if(getAccessState(master, nuidPICC) == true)
        {
            digitalWrite(green,HIGH);
            for(int i = 0; i <= 90; i++)
            {
                servoo.write(i);
                delay(50);
            }

            delay(5000);

            for(int i = 90; i >= 0; i--)
            {
                servoo.write(i);
                delay(50);
            }

        }

        if (getAccessState(master, nuidPICC) == false)
        {
            digitalWrite(red,HIGH);
            digitalWrite(buzzer,HIGH);
            servoo.write(0);
            delay(2000);
        }

        digitalWrite(green,LOW);
        digitalWrite(red,LOW);
        digitalWrite(buzzer,LOW);

        // Halt PICC
        rfid.PICC_HaltA();

        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();

    }
    if(Serial.available())
    {
        int x= Serial.read();
        Serial.println(x);
        if(x=='5')
        {
            digitalWrite(green,HIGH);
            
            for(int i = 0; i <= 90; i++)
            {
                servoo.write(i);
                delay(50);
            }

            delay(2000);
            digitalWrite(green,LOW);
            
            for(int i = 90; i >= 0; i--)
            {
                servoo.write(i);
                delay(50);
            }
            
            servoo.write(0);
            Serial.end();
        }
        if(x != '5')
        {
            digitalWrite(red,HIGH);
            digitalWrite(buzzer,HIGH);
            Serial.end();
            delay(2000);
            digitalWrite(red,LOW);
            digitalWrite(buzzer,LOW);
        }
    }
}

bool getAccessState(byte accessCode[], byte newCode[])
 {
    for (byte i = 0; i < 4; i++) 
        if (newCode[i] != accessCode[i]) 
            return false;     
    
    return true;
}
