#include <Adafruit_Fingerprint.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "WiFi.h"
#define CAYENNE_PRINT Serial     // Directs Cayenne debugging prints to serial output, Comment this out to disable prints and save space
#include <CayenneMQTTESP32.h>   // // Cayenne MQTT library for ESP32 communication

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "4459c180-724b-11ed-8d53-d7cd1025126a";
char password[] = "c893d40402e5d2b5d6982389bdbf93a7586ee038";
char clientID[] = "4f92fc60-724b-11ed-8d53-d7cd1025126a";


// SoftwareSerial setup for boards without hardware serial
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
    SoftwareSerial mySerial(2, 3);

#else
    #define mySerial Serial2

#endif


// Use Virtual Channel 5 for uptime display.

#define VIRTUAL_CHANNEL 5
 
// WiFi credentials
char ssid[] = "Redmi Note 10 5G";
char wifiPassword[] ="123Buddha";

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);    // Initialize fingerprint sensor
int status = 2;
uint8_t id;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  //// Display setup using U8G2 library, M0/ESP32/ESP8266/mega2560/Uno/Leonardo

void setup()
{
    Serial.begin(115200);  ///initializing serial communication between esp32 and PC

    Cayenne.begin(username, password, clientID, ssid, wifiPassword);  // Initialize Cayenne for cloud integration
    
    // Initialize Cayenne for cloud integration
    pinMode(13, OUTPUT);   
    digitalWrite(13, HIGH);

    u8g2.begin();           //intializing i2c display
    u8g2.setFontPosTop();   /**When you use drawStr to display strings, the default criteria is to display the lower-left
                            * coordinates of the characters.The function can be understood as changing the coordinate position to the upper left
                            * corner of the display string as the coordinate standard.*/
    

    //initializing serial communication between fingerprint sensor and esp32
    while (!Serial);     
    delay(100);
    Serial.println("\n\nAdafruit Fingerprint intialization");

    // set the data rate for the sensor serial port
    finger.begin(57600);

    if (finger.verifyPassword()) 
    {
        Serial.println("Found fingerprint sensor!!!");
    } 
    else 
    {
        Serial.println("Did not find fingerprint sensor!!!");
    }
}


// Utility function to read numbers from serial
uint8_t readnumber(void) 
{
    uint8_t num = 0;
    while (num == 0) 
    {
        while (! Serial.available());
        num = Serial.parseInt();
    }
    return num;
}

void loop()                     // run over and over again
{
    Cayenne.loop();

    u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
    u8g2.setFont(u8g2_font_heavybottom_tr);
    u8g2.drawStr(0, 0, "PLACE YOUR");   
    u8g2.drawStr(0, 20, "FINGER");   
    u8g2.sendBuffer();               
    delay(100);

    getFingerprintID();
    delay(50);         

}

// Function to handle fingerprint identification process
uint8_t getFingerprintID() 
{
    uint8_t p = finger.getImage();
    switch (p) 
    {
        case FINGERPRINT_OK:
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "IMAGE");   
            u8g2.drawStr(0, 20, "TAKEN");   
            u8g2.sendBuffer();               
            delay(2000);
            break;
        case FINGERPRINT_NOFINGER:
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            return p;
        case FINGERPRINT_IMAGEFAIL:
            return p;
        default:
            return p;
    }

  // OK success!

    p = finger.image2Tz();
    switch (p) 
    {
        case FINGERPRINT_OK:
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "IMAGE");   
            u8g2.drawStr(0, 20, "CONVERTED");   
            u8g2.sendBuffer();               
            delay(2000);
            break;
        case FINGERPRINT_IMAGEMESS:
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "IMAGE");   
            u8g2.drawStr(0, 20, "MESSY");   
            u8g2.sendBuffer();               
            delay(2000);
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            return p;
        case FINGERPRINT_FEATUREFAIL:
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            return p;
        default:
            return p;
    }

  // OK converted!
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) 
    {
        Serial.println("Found a print match!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR) 
    {
        //Serial.println("Communication error");
        return p;
    } 
    else if (p == FINGERPRINT_NOTFOUND) 
    {
        Serial.println("Did not find a match");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "NO MATCH");   
        u8g2.drawStr(0, 20, "FOUND");   
        u8g2.sendBuffer();               
        delay(2000);
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "TRY");   
        u8g2.drawStr(0, 20, "AGAIN");   
        u8g2.sendBuffer();               
        delay(2000);  
        return p;
    } 
    else 
    {
        //Serial.println("Unknown error");
        return p;
    }

    // found a match!
    Serial.print("Found ID #"); 
    Serial.print(finger.fingerID);
    char cstr[16];
    itoa(finger.fingerID, cstr, 10);
    Serial.println();
    u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
    u8g2.setFont(u8g2_font_heavybottom_tr);
    u8g2.drawStr(0, 0, "FOUND ID");   
    u8g2.drawStr(0, 20, cstr);   
    u8g2.sendBuffer();               
    delay(2000); 
    if (finger.fingerID == 2)
    {
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "WELCOME");   
        u8g2.drawStr(0, 20, "SIDDHARTHA");   
        u8g2.sendBuffer();
        digitalWrite(13, LOW);               
        delay(10000);
        digitalWrite(13, HIGH); 
    }

    else if (finger.fingerID == 1)
    {
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "WELCOME");   
        u8g2.drawStr(0, 20, "SUBASH");   
        u8g2.sendBuffer();
        digitalWrite(13, LOW);               
        delay(10000);
        digitalWrite(13, HIGH); 
    }

    else if (finger.fingerID == 3)
    {
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "WELCOME");   
        u8g2.drawStr(0, 20, "SULAV");   
        u8g2.sendBuffer();
        digitalWrite(13, LOW);               
        delay(10000);
        digitalWrite(13, HIGH); 
    }

    else
    {
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "WELCOME");      
        u8g2.sendBuffer();
        digitalWrite(13, LOW);               
        delay(10000);
        digitalWrite(13, HIGH); 
    }
    return finger.fingerID;
}


// Function to handle fingerprint enrollment process
uint8_t getFingerprintEnroll() 
{
    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #"); 
    Serial.println(id);
    u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
    u8g2.setFont(u8g2_font_heavybottom_tr);
    u8g2.drawStr(0, 0, "PLACE VALID");   
    u8g2.drawStr(0, 20, "FINGER");   
    u8g2.sendBuffer();               
    delay(1000);
    while (p != FINGERPRINT_OK) 
    {
        p = finger.getImage();
        switch (p) 
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "IMAGE");   
            u8g2.drawStr(0, 20, "TAKEN");   
            u8g2.sendBuffer();               
            delay(2000);
            break;
        case FINGERPRINT_NOFINGER:
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "NO FINGER");   
            u8g2.drawStr(0, 20, "PLACED");   
            u8g2.sendBuffer();               
            delay(2000);
            //Serial.println(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            //Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            //Serial.println("Imaging error");
            break;
        default:
            //Serial.println("Unknown error");
            break;
        }
    }

  // OK success!

    p = finger.image2Tz(1);
    switch (p) 
    {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "IMAGE");   
            u8g2.drawStr(0, 20, "CONVERTED");   
            u8g2.sendBuffer();               
            delay(2000);
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "IMAGE");   
            u8g2.drawStr(0, 20, "MESSY");   
            u8g2.sendBuffer();               
            delay(2000);
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            //Serial.println("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            //Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            //Serial.println("Could not find fingerprint features");
            return p;
        default:
            //Serial.println("Unknown error");
            return p;
    }

    Serial.println("Remove finger");
    u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
    u8g2.setFont(u8g2_font_heavybottom_tr);
    u8g2.drawStr(0, 0, "REMOVE");   
    u8g2.drawStr(0, 20, "FINGER");   
    u8g2.sendBuffer();               
    delay(4000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) 
    {
        p = finger.getImage();
    }
    Serial.print("ID "); Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
    u8g2.setFont(u8g2_font_heavybottom_tr);
    u8g2.drawStr(0, 0, "PLACE FINGER");   
    u8g2.drawStr(0, 20, "AGAIN");   
    u8g2.sendBuffer();               
    delay(1000);
    while (p != FINGERPRINT_OK) 
    {
        p = finger.getImage();
        switch (p) 
        {
            case FINGERPRINT_OK:
                Serial.println("Image taken");
                u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
                u8g2.setFont(u8g2_font_heavybottom_tr);
                u8g2.drawStr(0, 0, "IMAGE");   
                u8g2.drawStr(0, 20, "TAKEN");   
                u8g2.sendBuffer();               
                delay(2000);
                break;
            case FINGERPRINT_NOFINGER:
                //Serial.print(".");
                u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
                u8g2.setFont(u8g2_font_heavybottom_tr);
                u8g2.drawStr(0, 0, "NO FINGER");   
                u8g2.drawStr(0, 20, "PLACED");   
                u8g2.sendBuffer();               
                delay(2000);
                break;
            case FINGERPRINT_PACKETRECIEVEERR:
                //Serial.println("Communication error");
                break;
            case FINGERPRINT_IMAGEFAIL:
                //Serial.println("Imaging error");
                break;
            default:
                //Serial.println("Unknown error");
                break;
        }
    }

    // OK success!

    p = finger.image2Tz(2);
    switch (p) 
    {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "IMAGE");   
            u8g2.drawStr(0, 20, "CONVERTED");   
            u8g2.sendBuffer();               
            delay(2000);
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
            u8g2.setFont(u8g2_font_heavybottom_tr);
            u8g2.drawStr(0, 0, "FINGER");   
            u8g2.drawStr(0, 20, "MESSY");   
            u8g2.sendBuffer();               
            delay(2000);
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            //Serial.println("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            //Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            //Serial.println("Could not find fingerprint features");
            return p;
        default:
            //Serial.println("Unknown error");
            return p;
    }

    // OK converted!
    Serial.print("Creating model for #");  
    Serial.println(id);
    u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
    u8g2.setFont(u8g2_font_heavybottom_tr);
    u8g2.drawStr(0, 0, "CREATING");   
    u8g2.drawStr(0, 20, "createModel");   
    u8g2.sendBuffer();               
    delay(2000);

    p = finger.createModel();
    if (p == FINGERPRINT_OK) 
    {
        Serial.println("Prints matched!");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "FINGER");   
        u8g2.drawStr(0, 20, "MATCHED");   
        u8g2.sendBuffer();               
        delay(2000);
    } 
    else if (p == FINGERPRINT_PACKETRECIEVEERR) 
    {
        //Serial.println("Communication error");
        return p;
    } 
    else if (p == FINGERPRINT_ENROLLMISMATCH) 
    {
        Serial.println("Fingerprints did not match");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "FINGER DID");   
        u8g2.drawStr(0, 20, "NOT MATCHED");   
        u8g2.sendBuffer();               
        delay(2000);
        return p;
    } 
    else 
    {
        
        return p;
    }

    Serial.print("ID "); 
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) 
    {
        Serial.println("Stored!");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "FINGER");   
        u8g2.drawStr(0, 20, "STORED");   
        u8g2.sendBuffer();               
        delay(2000);
    } 
    else if (p == FINGERPRINT_PACKETRECIEVEERR) 
    {
        Serial.println("Communication error");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "FINGER NOT");   
        u8g2.drawStr(0, 20, "STORED");   
        u8g2.sendBuffer();               
        delay(2000);
        return p;
    } 
    else if (p == FINGERPRINT_BADLOCATION) 
    {
        Serial.println("Could not store in that location");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "FINGER NOT");   
        u8g2.drawStr(0, 20, "STORED");   
        u8g2.sendBuffer();               
        delay(2000);
        return p;
    } 
    else if (p == FINGERPRINT_FLASHERR) 
    {
        Serial.println("Error writing to flash");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "FINGER NOT");   
        u8g2.drawStr(0, 20, "STORED");   
        u8g2.sendBuffer();               
        delay(2000);
        return p;
    } 
    else 
    {
        Serial.println("Unknown error");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "FINGER NOT");   
        u8g2.drawStr(0, 20, "STORED");   
        u8g2.sendBuffer();               
        delay(2000);
        return p;
    }

    return true;
}


// Cayenne function for remote lock control
CAYENNE_IN(4)    
{
    CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
    if (getValue.asInt() == 1) 
    {
        digitalWrite(13, LOW);
    } 
   else if (getValue.asInt() == 0) 
    {
        digitalWrite(13, HIGH);
    }

}

// Handle fingerprint enrollment from Cayenne dashboard
CAYENNE_IN(6)   
{
    CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
    if (getValue.asInt() == 1) 
    {
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "ENROLL YOUR");   
        u8g2.drawStr(0, 20, "FINGER");   
        u8g2.sendBuffer();               
        delay(2000); 
        Serial.println("Ready to enroll a fingerprint!");
        Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
        finger.getTemplateCount();
        Serial.println(finger.templateCount);
        id = finger.templateCount + 1;
        if (id == 0) 
        {
            return;  // ID #0 not allowed, try again!
        }
        char cstr[16];
        itoa(finger.fingerID, cstr, 10);
        Serial.print("Enrolling ID #");
        u8g2.clearBuffer();     // Clears all pixel in the memory frame buffer.      
        u8g2.setFont(u8g2_font_heavybottom_tr);
        u8g2.drawStr(0, 0, "ENROLL ID:");   
        u8g2.drawStr(0, 20, cstr);   
        u8g2.sendBuffer();               
        delay(2000); 

        while (! getFingerprintEnroll() ); 
    }
}


// Cayenne function to display number of users
CAYENNE_OUT(VIRTUAL_CHANNEL)  
{
    CAYENNE_LOG("Send data for Virtual Channel %d", VIRTUAL_CHANNEL);
    
    // Sends the count of enrolled fingerprints to Cayenne
    finger.getTemplateCount();
    int counterValue =  finger.templateCount;
    Serial.println(counterValue);
    Cayenne.virtualWrite(VIRTUAL_CHANNEL, counterValue);
}
