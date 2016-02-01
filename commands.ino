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

// Adds some useful commands for the esp8266.
// Call setup_commands from your main setup() function.
// e.g. setup_commands();

#include <ESP8266WiFi.h>
#include <StreamString.h>

void setup_commands()
{
    Embedis::hardware( F("wifi"), [](Embedis* e) {
        StreamString s;
        WiFi.printDiag(s);
        e->response(s);
    }, 0);

    Embedis::command( F("RESET"), [](Embedis* e) {
        ESP.reset();
        while(1) {}
    });

    Embedis::command( F("pinMode"), [](Embedis* e) {
        if (e->argc != 3) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        String argv2(e->argv[2]);
        argv2.toUpperCase();
        int mode;
        if (argv2 == "INPUT") mode = INPUT;
        else if (argv2 == "OUTPUT") mode = OUTPUT;
        else if (argv2 == "INPUT_PULLUP") mode = INPUT_PULLUP;
        else return e->response(Embedis::ARGS_ERROR);
        pinMode(pin, mode);
        e->response(Embedis::OK);
    });

    Embedis::command( F("digitalWrite"), [](Embedis* e) {
        if (e->argc != 3) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        String argv2(e->argv[2]);
        argv2.toUpperCase();
        int mode;
        if (argv2 == "HIGH") mode = HIGH;
        else if (argv2 == "LOW") mode = LOW;
        else mode = argv2.toInt();
        digitalWrite(pin, mode);
        e->response(Embedis::OK);
    });

    Embedis::command( F("digitalRead"), [](Embedis* e) {
        if (e->argc != 2) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        if (digitalRead(pin)) {
            e->response(F("HIGH"));
        } else {
            e->response(F("LOW"));
        }
    });

    Embedis::command( F("analogRead"), [](Embedis* e) {
        if (e->argc != 2) return e->response(Embedis::ARGS_ERROR);
        int pin = String(e->argv[1]).toInt();
        e->response(':', analogRead(pin));
    });

}
