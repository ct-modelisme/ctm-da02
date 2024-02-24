/**
 * CT-Modelisme (ct-modelisme.fr) - Decodeur Audio 02 - Black-smith (AccessoryDecoder.h)
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

#ifndef ACCESSORY_DECODER_H
#define ACCESSORY_DECODER_H

// Include libraries
#include "Arduino.h"                                // Arduino library
#include "SoftwareSerial.h"                         // Serial used by DFPlayer
#include "DFRobotDFPlayerMini.h"                    // DFPlayerMini audio driver
#include "DCCInterface.h"                           // DCC interface


// General purpose constants
#define FIRMWARE "1.0"                              // The decoder firmware release


// Arduino pins definition
#define MODE_BTN_IN 3                               // The mode button
#define A_BTN_IN 4                                  // The pos A button
#define B_BTN_IN 5                                  // The pos B button

#define FIREPLACE_OUT 6                             // The A0 relay pin
#define ENVIL_OUT 7                                 // The A1 relay pin

#define PLAYER_RX_PIN  10                           // The pin connected to serial rx of DFPlayer
#define PLAYER_TX_PIN  11                           // The pin connected to serial tx of DFPlayer

#define STATE_A_LED_OUT A0                          // The A LED pin
#define STATE_B_LED_OUT A1                          // The B LED pin
#define MODE_LED_OUT LED_BUILTIN                    // The mode LED pin


// Factory CVs definition (CVs 47-64 & CVs 112-256 reserved for manufacturers)
#define CV_ACCESSORY_DECODER_ADDRESS_LSB 47         // DCC address 2 CVs
#define CV_ACCESSORY_DECODER_ADDRESS_MSB 48         // The audio player serial



/**
 * Accessory decoder class
 */
class AccessoryDecoder
{
  private:
  
    // Private attributes
    DCCInterface *dcc;                              // The dcc object
    DFRobotDFPlayerMini player;                     // The audio player instance
    
    unsigned long timerStartTime;                   // Used for timers
    unsigned long timerEndTime;                     // Used for timers
    byte timerRunning;                              // Used for timers
    
    byte animationState;                            // Is animation started?

    // Private methods
    AccessoryDecoder();                             // Singleton pattern
    AccessoryDecoder(AccessoryDecoder const&);      // Disable object copy
    void operator = (AccessoryDecoder const&);      // Disable assigment
    
    void blinkLed(int led, int nb=1, int freq=200); // Blink a led
    
    void defineMode();                              // Define the decoder mode
    void informationMode();                         // The information mode (Serial)
    void addressProgrammingMode();                  // The address programming mode
    void parsePlayerMessages();                     // Parse audio player messages


  public:
  
    // Public types
    enum MODES
    {
      RUNNING,
      INFO,
      ADDR_PROG,
    };
    
    // Public attributes
    int dccAddress;                                 // The first DCC address
    enum MODES mode;                                // The current decoder mode
    byte waitForSecondAddress;                      // Used in addr programming mode
    
    // Public methods
    static AccessoryDecoder* getInstance()
    {                                               // Singleton getter
        static AccessoryDecoder decoder;
        return &decoder;
    };

    void setup();                                   // Setup decoder
    void loop();                                    // Run decoder operations
    void runAnimation();                            // Run main animation
    void stopAnimation();                            // Stop main animation
};


void notifyDccAccTurnoutOutput(uint16_t addr, 
  uint8_t direction, uint8_t outputPower);          // The DCC accessory packet handler

#endif
