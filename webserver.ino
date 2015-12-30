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


// HTTP server which serves static files from SPIFFS.
// Call setup_webserver() from your main setup() function.
// Call loop_webserver from your main loop() function.

#include <ESP8266WebServer.h>
#include <FS.h>

ESP8266WebServer server80( 80 );

void setup_webserver() 
{
    setup_webserver("");
}

// You can serve from a subdirectory by supplying a root.
// e.g. setup_webserver(F("/path/to/files"))
void setup_webserver(const String& root) 
{
    SPIFFS.begin();
    server80.on ( "/api/getVcc", webserver_api_getVcc );
    server80.serveStatic("", SPIFFS, root.c_str(), "");
    server80.begin();
}

void loop_webserver() 
{
    server80.handleClient();
}

bool webserver_api_getVcc() 
{
    server80.send(200, "text/plain", read_vcc());
}

