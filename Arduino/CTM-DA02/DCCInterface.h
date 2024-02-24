/**
 * CT-Modelisme (ct-modelisme.fr) - Decodeur Audio 02 - Black-smith (DCCInterface.h)
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

#ifndef DCC_INTERFACE_H
#define DCC_INTERFACE_H

#include "Arduino.h"                                // Arduino library
#include <NmraDcc.h>                                // DCC library


// Structures
struct CV
{
  uint16_t addr;
  uint8_t value;
};


// Constants
#define DECODER_VERSION 1                           // The decoder version used by NmraDcc lib
#define DCC_SIGNAL_PIN 2                            // The pin connected to opto-isolator


/**
 * Digital Command Control class
 */
class DCCInterface
{
  private:
  
    // Private methods
    DCCInterface();                                 // Singleton pattern
    DCCInterface(DCCInterface const&);              // Disable object copy
    void operator = (DCCInterface const&);          // Disable assigment

    void (*resetArduino) (void) = 0;                // Arduino software reset


  public:
  
    // Public attributes
    NmraDcc nmraDCC;                                // The NmraDcc object
    
    CV *factoryCVs;                                 // A pointer to the factory CVs
    uint8_t factoryCVsLength = 0;                   // The factory CVs length
    uint8_t factoryCVIndex = 0;                     // The factory CV current index
    
    // Public methods
    static DCCInterface* getInstance()
    {                                               // Singleton getter
        static DCCInterface dcc;
        return &dcc;
    };
    
    void setup();                                   // Setup CLI
    void loop();                                    // Run CLI operations
    void registerFactoryCVs(CV &factoryCVs, 
      uint8_t factoryCVsLength);                    // Register the factory CVs from pointer
    void writeIntToCVs(uint8_t lsbCV, 
      uint8_t msbCV, int value);                    // Write int to two CVs
    int readIntFromCVs(uint8_t lsbCV, 
      uint8_t msbCV);                               // Read int from two CVs
};

void notifyCVResetFactoryDefault();                 // Request a reset of factory CVs

#endif
