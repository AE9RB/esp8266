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


// Telnet server which connects to Embedis.
// Call setup_telnet() from your main setup() function.
// Call loop_telnet() from your main loop() function.


#include <WiFiServer.h>
#include <WiFiClient.h>
#include "Embedis.h"

WiFiServer server23(23);
WiFiClient server23Client;
Embedis embedis23(server23Client);

void setup_telnet() 
{
    server23.begin();
    server23.setNoDelay(true);
}

void loop_telnet() 
{
    static int eat = 0;

    // new connections
    if (server23.hasClient()) {
        if (!server23Client.connected()) {
            server23Client.stop();
            server23Client = server23.available();
            embedis23.reset(true);
            eat = 0;
        } else {
            server23.available().stop();
        }
    }

    // discard negotiation from the client
    while (eat >= 0) {
        if (server23Client.peek() < 0) return;
        if (eat) {
            server23Client.read();
            eat--; 
            continue;
        }
        if (server23Client.peek() == 255) {
            eat = 3;
            continue;
        }
        eat = -1;
        break;
    }

    // handle the stream
    embedis23.process();
}


