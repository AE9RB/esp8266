/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015 PatternAgents, LLC

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "Embedis.h"

Embedis embedis(Serial);
MDNSResponder mdns;

//TODO temporary until configured by embedis
const char *ssid = ".....";
const char *password = ".....";
const char *dnshostname = "esp8266";

//TODO temporary until we have our variant
#undef BUILTIN_LED
#define BUILTIN_LED 0

// This will log to an embedis channel called "log".
// Use SUBSCRIBE LOG to get these messages.
// During setup() logs are also printed to Serial.
void LOG(const String& message) {
    static bool inSetup = true;
    if (!message.length()) {
        inSetup = false;
        return;
    }
    if (inSetup) {
        Serial.println(message);
    }
    Embedis::publish("log", message);
}


void setup() 
{
    pinMode ( BUILTIN_LED, OUTPUT );
    Serial.begin(115200);

    LOG( String() + F("Connecting to ") + ssid + F("..."));

    WiFi.begin ( ssid, password );
    while ( WiFi.status() != WL_CONNECTED ) {
        digitalWrite (BUILTIN_LED, !digitalRead(BUILTIN_LED));
        delay ( 250 );
    }

    LOG(F("Connected"));
    LOG( String() + F("IP: ") + WiFi.localIP().toString() );

    if ( mdns.begin ( dnshostname, WiFi.localIP() ) ) {
        LOG( String() + F("MDNS: ") +  dnshostname + F(".local"));
    }

    setup_EEPROM();
    setup_vcc();
    setup_webserver();
    setup_telnet();

    LOG("READY");
    LOG(""); // End setup logging
}


void loop() 
{
    static unsigned long heartbeat = 0;
    unsigned long now = millis();
    if (now > heartbeat) {
        if (digitalRead(BUILTIN_LED)) {
            digitalWrite (BUILTIN_LED, 0);
            heartbeat = now + 1;
        } else {
            digitalWrite (BUILTIN_LED, 1);
            heartbeat = now + 999;
        }
    }
    
    embedis.process();
    mdns.update();
    
    loop_webserver();
    loop_telnet();
}

