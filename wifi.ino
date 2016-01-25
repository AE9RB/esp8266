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
#include <DNSServer.h>

MDNSResponder mdns;
DNSServer dnsServer;

// WiFi configuration manager. To use:
// Call loop_wifi() from your main loop() function.

// Comfiguration is loaded from the default dictionary.
// mode : sta = station, ap = access point, anything else is auto-detect
//        which reverts to an access point after failing to connect as a station.

// ssid : access point to connect to in station mode
// passphrase : for access point ssid (optional)
// hostname : mDNS name, default esp8266

// ap_ssid : for becoming an access point, default esp8266
// ap_passphrase : for securing this access point (optional)


static const unsigned int TIMEOUT = 200; // ms
static const unsigned int TIMEOUT_COUNT = 10000 / TIMEOUT; // 10 secs

// Returns: (can be used for status LED)
//  0 while connecting
//  1 when connected
//  2 as access point
int loop_wifi() {
    static unsigned int loop_wifi_state = 0;
    static unsigned int timeout_count = 0;
    static unsigned int timeout;

    if (loop_wifi_state == 0) {
        String mode;
        Embedis::get("mode", mode);
        mode.toLowerCase();
        if (mode == "ap") loop_wifi_state = 1;
        else if (mode == "sta") loop_wifi_state = 3;
        else loop_wifi_state = 6;
        return 0;
    }
    if (loop_wifi_state == 1) {
        String ap_ssid(F("esp8266"));
        String ap_passphrase;
        Embedis::get("ap_ssid", ap_ssid);
        Embedis::get("ap_passphrase", ap_passphrase);

        LOG( String() + F("Started access point: ") + ap_ssid);
        LOG(""); // End setup logging
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ap_ssid.c_str(), ap_passphrase.c_str());
        dnsServer.start(53, "*", WiFi.softAPIP());
        ++loop_wifi_state;
        return 0;
    }
    if (loop_wifi_state == 2) {
        dnsServer.processNextRequest();
        return 2;
    }
    if (loop_wifi_state == 3 || loop_wifi_state == 6) {
        String ssid;
        String passphrase;
        Embedis::get("ssid", ssid);
        Embedis::get("passphrase", passphrase);
        if (!ssid.length() && loop_wifi_state == 6) {
            loop_wifi_state = 1; 
            return 0;
        }
        if (timeout_count == 0) {
            LOG(String() + F("Connecting to: ") + ssid);
        }
        WiFi.mode(WIFI_STA);
        WiFi.begin ( ssid.c_str(), passphrase.c_str() );
        timeout = millis() + TIMEOUT;
        ++loop_wifi_state;
        return 0;
    }
    if (loop_wifi_state == 7) {
        if (WiFi.status() == WL_CONNECTED) loop_wifi_state = 4;
        else if (millis() > timeout) {
            timeout_count++;
            LOG(F("Unable to connect."));
            if (timeout_count > TIMEOUT_COUNT) {
                loop_wifi_state = 1;
            } else {
                loop_wifi_state = 6;
            }
        }
        return 0;
    }
    if (loop_wifi_state == 4) {
        if (WiFi.status() == WL_CONNECTED) {
            LOG( String() + F("Connected. IP: ") + WiFi.localIP().toString() );
            String hostname(F("esp8266"));
            Embedis::get("hostname", hostname);
            if (hostname.length()) {
                if(mdns.begin ( hostname.c_str(), WiFi.localIP() ) ) {
                    LOG( String() + F("MDNS: ") +  hostname + F(".local"));
                }
            }
            LOG(""); // End setup logging
            ++loop_wifi_state;
            return 1;
        } else if (millis() > timeout) {
            timeout_count = 1;
            loop_wifi_state = 3;
        }
        return 0;
    }
    if (loop_wifi_state == 5) {
        if (WiFi.status() != WL_CONNECTED) return 0;
        mdns.update();
        return 1;
    }
    // should never get here
    return 0;
}



