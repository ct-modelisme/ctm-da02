/**
 * CT-Modelisme (ct-modelisme.fr) - Decodeur Audio 02 - Black-smith (CLI.cpp)
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
 * Thu 22 Jul 10:20:42 CEST 2021
 */

#include "CLI.h"


/**
 * Generic constructor for CLI
 */
CLI::CLI() {}



/**
 * The CLI setup function
 */
void 
CLI::setup()
{
  // Initialize serial communications
  Serial.begin(115200);
  
  Serial.println("\n\n------");
  Serial.println("Initializing microcontroller...");
  Serial.println(" - Serial communication initialized");
}



/**
 * The main CLI loop
 */
void 
CLI::loop()
{
  if(Serial.available() > 0)
  {
    String cmd = Serial.readString();
    cmd.trim();
    
    if(cmd == "reset")
    {
      Serial.println("-- Factory reset --");
      
      // Reset decoder CVs to factory values
      notifyCVResetFactoryDefault();
    }
    else
    {
      Serial.print("Command '");
      Serial.print(cmd);
      Serial.println("' unknown");
      delay(250);
    }
  }
}
