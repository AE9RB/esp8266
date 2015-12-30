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

void setup() 
{
    pinMode ( BUILTIN_LED, OUTPUT );
    Serial.begin(115200);

    WiFi.begin ( ssid, password );
    Serial.println ( "" );
    Serial.print ( "Connecting to " );
    Serial.print ( ssid );
    Serial.println ( "..." );
    while ( WiFi.status() != WL_CONNECTED ) {
        digitalWrite (BUILTIN_LED, !digitalRead(BUILTIN_LED));
        delay ( 250 );
    }
    Serial.println ( "Connected." );
    Serial.print ( "IP: " );
    Serial.println ( WiFi.localIP() );

    if ( mdns.begin ( dnshostname, WiFi.localIP() ) ) {
        Serial.print ( String(F("MDNS: ")) + dnshostname + F(".local") );
    }

    setup_EEPROM();
    setup_vcc();
    setup_webserver();
    setup_telnet();
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

