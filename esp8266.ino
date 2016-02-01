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

// CAUTION: This example demonstrates some simple authentication methods.
//          These are not secure. They are examples to remind you about security.
//          Consult a security expert if you need security.

#include <ESP8266WiFi.h> // keep here to supress Arduino error
#include "Embedis.h"

Embedis embedis(Serial);

void setup() 
{
    Serial.begin(115200);
    setup_EEPROM(); // keep first, settings stored here
    setup_commands();
    setup_vcc();
    setup_webserver();
    setup_telnet();
}


void loop() 
{
    embedis.process();
    blink( loop_wifi() );
    loop_webserver();
    loop_telnet();
}


// Blink out a number. More than 2 may be hard to count.
// Using 0 blinks fast and steady.
void blink(int num) {
    static unsigned long heartbeat = 0;
    static int beatcount = 0;
    static uint8_t led = 30;

    unsigned long now = millis();
    if (now > heartbeat) {
        if (digitalRead(led)) {
            digitalWrite (led, 0);
            if (!num) heartbeat = now + 250;
            else heartbeat = now + 1;
        } else {
            digitalWrite (led, 1);
            if (!num) heartbeat = now + 250;
            else {
                if (beatcount) {
                    --beatcount;
                    heartbeat = now + 175;
                } else {
                    led++;
                    if (led > 32) led = 30;
                    beatcount = num - 1;
                    heartbeat = now + 999;
                }
            }
        }
    }
}


// This will log to an embedis channel called "log".
// Use SUBSCRIBE LOG to get these messages.
// Logs are also printed to Serial until an empty message is received.
// Success with WiFi sends an empty message.
void LOG(const String& message) {
    static bool inSetup = true;
    if (inSetup) {
        if (!message.length()) {
            inSetup = false;
            return;
        }
        SERIAL_PORT_MONITOR.println(message);
    }
    Embedis::publish("log", message);
}
