/**
 * CT-Modelisme (ct-modelisme.fr) - Decodeur Audio 02 - Black-smith (CTM-DA02.ino)
 * 
 * Requires NmraDcc library from mrrwa (https://github.com/mrrwa/NmraDcc)
 * Requires Servo library from arduino project (https://www.arduino.cc/reference/en/libraries/servo/)
 * 
 * 
 * Copyright (C) 2021 Cyrille TOULET
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 * 
 * 
 * Cyrille TOULET <cyrille.toulet@linux.com>
 * Thu Nov 23 02:43:48 PM CET 2023
 */

#include "DCCInterface.h"
#include "AccessoryDecoder.h"
#include "CLI.h"


// Objects
CLI *cli;
DCCInterface *dcc;
AccessoryDecoder *decoder;


// Setup arduino
void setup()
{
  // Command line interface
  cli = CLI::getInstance();
  cli->setup();

  // DCC
  dcc = DCCInterface::getInstance();
  dcc->setup();

  // Accessory decoder
  decoder = AccessoryDecoder::getInstance();
  decoder->setup();
}


// Run main loop
void loop()
{
  cli->loop();
  decoder->loop();
  dcc->loop();
}
