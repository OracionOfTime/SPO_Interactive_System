/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_Fingerprint.h>
#include <fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial fingerprintSerial(D3, D4);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define fingerprintSerial Serial1

#endif
Adafruit_Fingerprint fingerprintfinger =
    Adafruit_Fingerprint(&fingerprintSerial);

void setup_fingerprint() {
    Serial.println("\n\nAdafruit finger detect test");

    // set the data rate for the sensor serial port
    fingerprintfinger.begin(57600);
    Serial.println("ausbiwebgiwgugwogouwe");
    while (true) {
        Serial.println("BBBBBBBBBBBBBBBBBB");
        if (fingerprintfinger.verifyPassword()) {
            Serial.println("Found fingerprint sensor!");
            break;
        } else {
            Serial.println("Did not find fingerprint sensor :(");
            delay(1000);
        }
    }

    Serial.println(F("Reading sensor parameters"));
    fingerprintfinger.getParameters();
    Serial.print(F("Status: 0x"));
    Serial.println(fingerprintfinger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x"));
    Serial.println(fingerprintfinger.system_id, HEX);
    Serial.print(F("Capacity: "));
    Serial.println(fingerprintfinger.capacity);
    Serial.print(F("Security level: "));
    Serial.println(fingerprintfinger.security_level);
    Serial.print(F("Device address: "));
    Serial.println(fingerprintfinger.device_addr, HEX);
    Serial.print(F("Packet len: "));
    Serial.println(fingerprintfinger.packet_len);
    Serial.print(F("Baud rate: "));
    Serial.println(fingerprintfinger.baud_rate);

    fingerprintfinger.getTemplateCount();

    if (fingerprintfinger.templateCount == 0) {
        Serial.print("Sensor doesn't contain any fingerprint data. Please run "
                     "the 'enroll' example.");
    } else {
        Serial.print("Sensor contains ");
        Serial.print(fingerprintfinger.templateCount);
        Serial.println(" templates");
    }
}

bool fingerprint_loop(std::function<void()> before,
                      std::function<void()> after) // run over and over again
{
    Serial.println("Waiting for finger");
    auto res = getFingerprintIDez();
    auto initial_time = millis();
    bool on = true;
    before();
    while (res < 0) {
        auto time = millis();
        if (initial_time - time > 200) {
            on ? after() : before();
            on = !on;
        }
        initial_time = time;
        res = getFingerprintIDez();
        delay(10);
    }
    Serial.println("Finger correct");
    after();
    return true;

    // don't ned to run this at full speed.
}

uint8_t getFingerprintID() {
    uint8_t p = fingerprintfinger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
    case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK success!

    p = fingerprintfinger.image2Tz();
    switch (p) {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    p = fingerprintfinger.fingerSearch();
    if (p == FINGERPRINT_OK) {
        Serial.println("Found a print match!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_NOTFOUND) {
        Serial.println("Did not find a match");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }

    // found a match!
    Serial.print("Found ID #");
    Serial.print(fingerprintfinger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(fingerprintfinger.confidence);

    return fingerprintfinger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
    uint8_t p = fingerprintfinger.getImage();
    if (p != FINGERPRINT_OK)
        return -1;

    p = fingerprintfinger.image2Tz();
    if (p != FINGERPRINT_OK)
        return -1;

    p = fingerprintfinger.fingerFastSearch();
    if (p != FINGERPRINT_OK)
        return -1;

    // found a match!
    Serial.print("Found ID #");
    Serial.print(fingerprintfinger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(fingerprintfinger.confidence);
    return fingerprintfinger.fingerID;
}

uint8_t enrollReadnumber(void) {
    uint8_t num = 0;

    while (num == 0) {
        while (!Serial.available())
            ;
        num = Serial.parseInt();
    }
    return num;
}

uint8_t id;

void enroll_loop(uint16_t _id, std::function<void()> before,
                 std::function<void()> after) // run over and over again
{
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save "
                   "this finger as...");
    id = _id;
    if (id == 0) {
        Serial.println("ID #0 not allowed, try again!");
        return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);

    while (!getFingerprintEnroll(before, after)) {
    }
    after();
    fingerprintfinger.getTemplateCount();
}

uint8_t getFingerprintEnroll(std::function<void()> before,
                             std::function<void()> after) {
    auto initial_time = millis();
    bool on = true;
    before();
    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #");
    Serial.println(id);
    while (p != FINGERPRINT_OK) {
        auto time = millis();
        if (initial_time - time > 200) {
            on ? after() : before();
            on = !on;
        }
        initial_time = time;
        p = fingerprintfinger.getImage();
        switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
        }
    }

    // OK success!

    p = fingerprintfinger.image2Tz(1);
    switch (p) {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    Serial.println("Remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) {
        p = fingerprintfinger.getImage();
    }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    while (p != FINGERPRINT_OK) {
        auto time = millis();
        if (initial_time - time > 200) {
            on ? after() : before();
            on = !on;
        }
        initial_time = time;
        p = fingerprintfinger.getImage();
        switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.print(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
        }
    }

    // OK success!

    p = fingerprintfinger.image2Tz(2);
    switch (p) {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    Serial.print("Creating model for #");
    Serial.println(id);

    p = fingerprintfinger.createModel();
    if (p == FINGERPRINT_OK) {
        Serial.println("Prints matched!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_ENROLLMISMATCH) {
        Serial.println("Fingerprints did not match");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }

    Serial.print("ID ");
    Serial.println(id);
    p = fingerprintfinger.storeModel(id);
    if (p == FINGERPRINT_OK) {
        Serial.println("Stored!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_BADLOCATION) {
        Serial.println("Could not store in that location");
        return p;
    } else if (p == FINGERPRINT_FLASHERR) {
        Serial.println("Error writing to flash");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }

    return true;
}

uint16_t get_num_prints() { return fingerprintfinger.templateCount; }
