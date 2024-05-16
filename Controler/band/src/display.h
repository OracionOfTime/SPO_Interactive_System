#include "../../shared/structures.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <sstream>
#include <string>

Adafruit_SSD1306 dp(128, 64, &Wire, -1);

class Display {
public:
    Display(){};
    void init() {
        if (!dp.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
            Serial.println("SSD1306 allocation failed");
            while (true)
                ;
        }
        dp.clearDisplay();
        dp.setTextSize(1);
        dp.setTextColor(WHITE);
        dp.setCursor(0, 28);
        dp.println("SPO-Medband");
        dp.display();
    };

    void display(std::string str) {
        dp.clearDisplay();
        dp.setCursor(0, 28);
        dp.println(str.c_str());
        dp.display();
    }
    void display(Entry entry) {
        auto num = String(entry.unit_value);
        Serial.println(num);
        std::stringstream strstr;
        strstr << num.c_str();
        if (entry.unit_value == 1) {
            strstr << " Pill\n";
        } else {
            strstr << " Pills\n";
        }
        strstr << "Color: ";
        switch (entry.color) {
        case Red:
            strstr << " Red";
            break;
        case Green:
            strstr << " Green";
            break;
        case Blue:
            strstr << " Blue";
            break;
        }
        Serial.println(strstr.str().c_str());
        display(strstr.str());
    }
    void clear() {
        dp.clearDisplay();
        dp.display();
    }
};