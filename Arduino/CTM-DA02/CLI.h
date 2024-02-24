/**
 * CT-Modelisme (ct-modelisme.fr) - Decodeur Audio 02 - Black-smith (CLI.h)
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

#ifndef CLI_H
#define CLI_H

#include "Arduino.h"                                // Arduino library
#include "DCCInterface.h"                           // DCC interface


/**
 * Command Line Interface class
 */
class CLI
{
  private:
  
    // Private methods
    CLI();                                          // Singleton pattern
    CLI(CLI const&);                                // Disable object copy
    void operator = (CLI const&);                   // Disable assigment


  public:
  
    // Public methods
    static CLI* getInstance()
    {                                               // Singleton getter
        static CLI cli;
        return &cli;
    };
    
    void setup();                                   // Setup CLI
    void loop();                                    // Run CLI operations
};

#endif
