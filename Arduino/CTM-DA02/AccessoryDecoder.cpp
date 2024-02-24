/**
 * CT-Modelisme (ct-modelisme.fr) - Decodeur Audio 02 - Black-smith (AccessoryDecoder.cpp)
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

#include "AccessoryDecoder.h"


// The factory CV values for this decoder
CV factoryCVs [] =
{
  {CV_ACCESSORY_DECODER_ADDRESS_LSB, 140},      // Less signifiant bits of long address 1
  {CV_ACCESSORY_DECODER_ADDRESS_MSB, 0},      // Most signifiant bits of long address 1
};


// Globals
SoftwareSerial playerSerial(
  PLAYER_RX_PIN, 
  PLAYER_TX_PIN
);


/**
 * Generic constructor for AccessoryDecoder
 */
AccessoryDecoder::AccessoryDecoder() 
{
  this->dcc = DCCInterface::getInstance();
  this->dcc->registerFactoryCVs(*factoryCVs, sizeof(factoryCVs) / sizeof(CV));
  
  this->mode = AccessoryDecoder::RUNNING;
  this->timerRunning = 0;
  
  // Read settings from decoder CVs
  this->dccAddress = this->dcc->readIntFromCVs(CV_ACCESSORY_DECODER_ADDRESS_LSB, CV_ACCESSORY_DECODER_ADDRESS_MSB);
}


/**
 * The deecoder setup function
 */
void 
AccessoryDecoder::setup()
{
  // Set pin modes
  pinMode(MODE_LED_OUT, OUTPUT);
  pinMode(STATE_A_LED_OUT, OUTPUT);
  pinMode(STATE_B_LED_OUT, OUTPUT);

  pinMode(FIREPLACE_OUT, OUTPUT);
  pinMode(ENVIL_OUT, OUTPUT);

  pinMode(MODE_BTN_IN, INPUT);
  pinMode(A_BTN_IN, INPUT);
  pinMode(B_BTN_IN, INPUT);

  Serial.println(" - Inputs/Outputs initialized");
  Serial.println("\nInitializing audio driver...");

  // Initialize DFPlayer
  playerSerial.begin(9600);
  // Initialize player with software serial, no ACK, and no reset (bug with reset)!!!
  if(!this->player.begin(playerSerial, false, false))
  {
    Serial.println(" - Error: unable to initialize driver. Is SD card inserted?\n");
    while(true);
  }

  Serial.println(" - MP3-TF-16P driver loaded");

  // Danger! There is a bug with reset function...
  //this->player.reset();
  this->player.setTimeOut(500);
  this->player.enableDAC();
  this->player.EQ(DFPLAYER_EQ_NORMAL);
  this->player.volume(15);

  Serial.println(" - MP3-TF-16P driver configured");

  Serial.println("Initializing completed!");
  Serial.print("CTM-DA02 - ");
  Serial.print("Firmware version: v");
  Serial.println(FIRMWARE);
  Serial.println("Please visit https://ct-modelisme.fr/\n");

  Serial.println("-- Running mode --");
}


/**
 * Parse audio player messages
 */
void 
AccessoryDecoder::parsePlayerMessages()
{
  if(this->player.available())
  {
    switch(this->player.readType())
    {
      case TimeOut:
        Serial.println("Time out!");
        break;
      case WrongStack:
        Serial.println("Stack wrong!");
        break;
      case DFPlayerCardInserted:
        Serial.println("Card inserted!");
        break;
      case DFPlayerCardRemoved:
        Serial.println("Card removed!");
        break;
      case DFPlayerCardOnline:
        Serial.println("Card online!");
        break;
      case DFPlayerError:
        Serial.print("DFPlayer error: ");
        switch (this->player.read()) {
          case Busy:
            Serial.println(F("card not found"));
            break;
          case FileMismatch:
            Serial.println(F("cannot find file"));
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
}

/**
 * The deecoder loop
 */
void 
AccessoryDecoder::loop()
{
  // Running mode
  if(this->mode == RUNNING)
  {
    digitalWrite(MODE_LED_OUT, LOW);
    this->defineMode();

    if(digitalRead(A_BTN_IN))
    {
      Serial.println("First button pressed.");
      this->runAnimation();
      
      // Software anti-rebound
      do {
        delay(200);
      } while(digitalRead(A_BTN_IN));
    }
    
    if(digitalRead(B_BTN_IN))
    {
      Serial.println("Second button pressed (not implemented yet).");
      
      // Software anti-rebound
      do {
        delay(200);
      } while(digitalRead(B_BTN_IN));
    }
  }

  // Address prog mode
  else if(this->mode == ADDR_PROG)
    this->addressProgrammingMode();

  // Info mode
  else if(this->mode == INFO)
    this->informationMode();

  // Parse audio player messages
  this->parsePlayerMessages();
}



/**
 * Run main animation
 */
void
AccessoryDecoder::runAnimation()
{
  int id = 1;
  
  Serial.println("Running main animation.");

  digitalWrite(STATE_A_LED_OUT, HIGH);
  digitalWrite(FIREPLACE_OUT, HIGH);
  digitalWrite(ENVIL_OUT, HIGH);

  player.disableLoop();
  player.stop();
  // Bug si volume > à 20-25 (trop de courant consommé par l'ampli audio ???)
  player.volume(20);
  player.playFolder(1, id);
  
  this->animationState = 1;
  
  delay(55000);
  this->stopAnimation();
}



/**
 * Stop main animation
 */
void
AccessoryDecoder::stopAnimation()
{
  Serial.println("Stop main animation.\n");

  digitalWrite(STATE_A_LED_OUT, LOW);
  digitalWrite(STATE_B_LED_OUT, LOW);
  digitalWrite(FIREPLACE_OUT, LOW);
  digitalWrite(ENVIL_OUT, LOW);
  
  this->animationState = 0;
}



/**
 * Analyse buttons pressed or not and define the current mode
 */
void 
AccessoryDecoder::defineMode()
{
  if(this->mode == RUNNING)
  {
    int modeBtnValue = digitalRead(MODE_BTN_IN);
  
    // Start pressing mode button
    if(!this->timerRunning && modeBtnValue)
    {
      this->timerStartTime = millis();
      this->timerRunning = 1;
    }
    
    // Stop pressing mode button
    else if(this->timerRunning && !modeBtnValue)
    {
      this->timerEndTime = millis();
      this->timerRunning = 0;
      
      unsigned long duration = timerEndTime - timerStartTime;
  
      if(duration < 1000)
      {
        Serial.println("-- Information mode --");
        this->mode = INFO;
      }
      else
      {
        Serial.println("-- DCC Address programming mode --");
        this->mode = ADDR_PROG;
      }
    }
  }
}



/**
 * Information mode
 * Display settings on serial console.
 */
void 
AccessoryDecoder::informationMode()
{
  int modeBtnValue;
  
  Serial.print("DCC address: ");
  Serial.println(this->dccAddress);

  delay(200);
  do {
    modeBtnValue = digitalRead(MODE_BTN_IN);
  } while(modeBtnValue);
  
  // Software anti-rebound
  do {
    delay(200);
  } while(!digitalRead(MODE_BTN_IN));

  Serial.print("CTM-DA02 - ");
  Serial.print("Firmware version: v");
  Serial.println(FIRMWARE);

  delay(200);
  do {
    modeBtnValue = digitalRead(MODE_BTN_IN);
  } while(modeBtnValue);

  Serial.println("Back to running mode.\n");
  Serial.println("-- Running mode --");
  this->mode = RUNNING;
  
  // Software anti-rebound
  do {
    delay(200);
  } while(digitalRead(MODE_BTN_IN));
}



/**
 * DCC address programmation mode
 * Learn the first address received on DCC frames as it first address
 */
void 
AccessoryDecoder::addressProgrammingMode()
{
  int modeBtnValue;
  digitalWrite(MODE_LED_OUT, HIGH);

  // Learning done in DCC handler...
  
  if(digitalRead(MODE_BTN_IN))
  {
    Serial.println("Aborted! Leaving programming mode.\n");
    Serial.println("-- Running mode --");
    this->mode = RUNNING;

    // Software anti-rebound
    do {
      delay(200);
    } while(digitalRead(MODE_BTN_IN));
  }
}



/**
 * Blink a given LED
 * 
 * @param led: The LED pin
 * @param nb: The number of flashes (default to 1)
 * @param freq: The time between flashes in ms (default to 200)
 */
void 
AccessoryDecoder::blinkLed(int led, int nb, int freq)
{
  for(int i=0; i<nb; i++)
  {
    delay(freq);
    digitalWrite(led, HIGH);
    delay(freq);
    digitalWrite(led, LOW);
  }
}



/** 
 * DCC turnout packet handler
 * 
 * @param addr: The DCC address
 * @param direction: The requested state
 * @param outputPower: The output state
 */
void notifyDccAccTurnoutOutput(uint16_t addr, uint8_t direction, uint8_t outputPower)
{
  DCCInterface *dcc = DCCInterface::getInstance();
  AccessoryDecoder *decoder = AccessoryDecoder::getInstance();
  
  // Learning mode
  if(decoder->mode == AccessoryDecoder::ADDR_PROG)
  {
    decoder->dccAddress = addr;
    dcc->writeIntToCVs(CV_ACCESSORY_DECODER_ADDRESS_LSB, CV_ACCESSORY_DECODER_ADDRESS_MSB, addr);
    Serial.print("New DCC address learned: ");
    Serial.println(addr, DEC);

    delay(350);
  }
  
  // Normal mode
  else if(decoder->mode == AccessoryDecoder::RUNNING)
  {
    if(addr == decoder->dccAddress)
      decoder->runAnimation();
  }
}
