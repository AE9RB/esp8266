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


// The esp8266 does not have real EEPROM. This is emulated in flash.
// To configure an EEPROM dictionary, call setup_esp8266_EEPROM from your
// main setup() function. Optionally, supply the database name you want to use.
// e.g. setup_EEPROM();
//      setup_EEPROM( F("EEPROM2") );

#include <EEPROM.h>
#include "spi_flash.h"

void setup_EEPROM() 
{
    setup_EEPROM( F("EEPROM") );
}

void setup_EEPROM(const String& dict) 
{
    EEPROM.begin(SPI_FLASH_SEC_SIZE);
    Embedis::dictionary( dict,
        SPI_FLASH_SEC_SIZE,
        [](size_t pos) -> char { return EEPROM.read(pos); },
        [](size_t pos, char value) { EEPROM.write(pos, value); },
        []() { EEPROM.commit(); }
    );
}

