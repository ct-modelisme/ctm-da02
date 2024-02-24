/**
 * CT-Modelisme (ct-modelisme.fr) - Decodeur Audio 02 - Black-smith (DCCInterface.cpp)
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

#include "DCCInterface.h"


/**
 * Generic constructor for DCC
 */
DCCInterface::DCCInterface() {}



/**
 * The DCC setup function
 */
void 
DCCInterface::setup()
{
  Serial.println("\nInitializing DCC decoder...");
  
  // Setup pin D2 as input pin (with pull-up)
  this->nmraDCC.pin(0, DCC_SIGNAL_PIN, 1);
  Serial.println(" - DCC interface initialized");

  // Compile DCC flags
  uint8_t flags = CV29_ACCESSORY_DECODER;
  flags |= CV29_OUTPUT_ADDRESS_MODE;
  //flags |= FLAGS_MY_ADDRESS_ONLY;
  Serial.println(" - Accessory decoder options set");
  
  Serial.print(" - DCC decoder address: ");
  Serial.println(this->readIntFromCVs(CV_ACCESSORY_DECODER_ADDRESS_LSB, CV_ACCESSORY_DECODER_ADDRESS_MSB));

  // Initialize decoder
  this->nmraDCC.init(MAN_ID_DIY, DECODER_VERSION, flags, 0);
  Serial.println(" - DCC driver initialized");
}



/**
 * The main DCC loop
 */
void 
DCCInterface::loop()
{
  // Process DCC packets
  this->nmraDCC.process();
  
  // Reset factory CVs
  if(this->factoryCVIndex && this->nmraDCC.isSetCVReady())
  {
    // Decrement first as initially it is the size of the array 
    this->factoryCVIndex--; 
    
    this->nmraDCC.setCV(
      this->factoryCVs[this->factoryCVIndex].addr, 
      this->factoryCVs[this->factoryCVIndex].value
    );

    if(this->factoryCVIndex == 0)
    {
      Serial.println("All CVs reset at factory defaults. Reboot in progress...");
      delay(200);
      
      // Reset microcontroller
      this->resetArduino();
    }
  }
}



/**
 * Register the factory CV default array
 * 
 * @param factoryCVs: the CV array address
 * @param factoryCVsLength: the CV array length
 */
void 
DCCInterface::registerFactoryCVs(CV &factoryCVs, uint8_t factoryCVsLength)
{
  this->factoryCVs = &factoryCVs;
  this->factoryCVsLength = factoryCVsLength;
}


/**
 * Write int into CVs
 * 
 * @param lsbCV: The less signifiant address bits
 * @param msbCV: The most signifiant address bits
 * @param value: The int to write
 */
void 
DCCInterface::writeIntToCVs(uint8_t lsbCV, uint8_t msbCV, int value)
{
  if(this->nmraDCC.isSetCVReady())
  {
    this->nmraDCC.setCV(msbCV, value >> 8);
    this->nmraDCC.setCV(lsbCV, value & 0xFF);
  }
  else
    Serial.println("Unable to write int into given CVs");
}



/**
 * Read int from CVs
 * 
 * @param lsbCV: The less signifiant address bits
 * @param msbCV: The most signifiant address bits
 * @return: The int read from CVs
 */
int 
DCCInterface::readIntFromCVs(uint8_t lsbCV, uint8_t msbCV)
{
  return (this->nmraDCC.getCV(msbCV) << 8) + this->nmraDCC.getCV(lsbCV);
}



/**
 * Reset factory CVs to defaults
 */
void notifyCVResetFactoryDefault()
{
  DCCInterface *dcc = DCCInterface::getInstance();
  
  // Make factoryCVIndex non-zero and equal to num CV's to be reset 
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
  dcc->factoryCVIndex = dcc->factoryCVsLength;
};
