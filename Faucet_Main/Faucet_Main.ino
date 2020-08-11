/***************************************************************************
* FILE:Faucet_Main
* AUTHOR: ADAM JENNINGS
* Description:
* This program is part of team 998 senior design project for smart home faucet.
* The purpose of this program is to control the individual faucet controllers 
* within the smart home system and can be used for kitchen sink, bathroom sink, 
* shower, or bathtub faucet. Due to covid-19 shut down the project was canceled 
* late February and is missing some key features.  
* 
* 
***************************************************************************/


/**************************
 *Variables for LCD screen and button inputs
 ***************************/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// LCD pins used to output to display
#define TFT_DC 10
#define TFT_CS 11

//define value when button activated
#define ACTIVATED HIGH 

//assign variable to button pins
const int buttonScrollNext = 12;
const int buttonScrollBack = 13;
const int buttonSelect = 14;
const int buttonBack = 15;

//set default values 
int button = 0; //track what menu option is highlighted
int faucet = 0; // 0 faucet is off, 1 faucet is on
int selectTemp = 40; //used to allow user to select desired temp. When selected will store but if canceled then will not save.
int selectFlowRate = 0;//used to allow user to select desired flow. When selected will store but if canceled then will not save.
int desiredTemp = 70; //set desired temp at start up
int desiredFlowRate = 0; //set flow rate to off 

// Use hardware attach lcd to lcd pins
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


/******************************
 *Variables for flow meter
 ******************************/
int coldFlowPin = 2;    //This is the input pin on the Arduino
int hotFlowPin = 3;    //This is the input pin on the Arduino
double flowRate;    //This is the value we intend to calculate. 
double flowRateCold;    //This is the value we intend to calculate. 
double flowRateHot;    //This is the value we intend to calculate.

volatile int countHot; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process.  
volatile int countCold; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process.  
unsigned long oldTime; //used to keep track of starting time. 

unsigned int flowGallonsCold; //used to track flow rate 
unsigned int flowGallonsHot; // used to track flow rate
unsigned long totalUsedCold; //tracks total water used
unsigned long totalUsedHot; // tracks total water used
unsigned long totalUsed; //tracks combined hot and cold used. 

/******************************
 *Variables for thermistors
 ******************************/
#define THERMISTORPINHOT A0  
#define THERMISTORPINCOLD A1  
        
#define THERMISTORNOMINAL 10000 //nomial resistance of thermistor
#define TEMPERATURENOMINAL 25  //temperature for nomial resistance of 10k is 25C
#define NUMSAMPLES 5 //samples used to average and get more accurate temperature
#define BCOEFFICIENT 3950 //beta coefficient
#define SERIESRESISTOR 10000  //resistor value 

double hotTemp; //stores current hot water temp
double coldTemp;  //stores current cold water temp
double currentTemperature; //stores current temperature
int samples[NUMSAMPLES]; // used to store temp values to average.

/******************************
 *Variables for stepper motors
 ******************************/
 // define pin used for stepper motors
 const int stepPinHot = 6; //sets pin to control if motor steps.
 const int dirPinHot = 7; //sets pin to set step direction
 const int stepPinCold = 8;//sets pin to control if motor steps.
 const int dirPinCold = 9;//sets pin to set step direction
 const int maxStep = 50; //200 steps is full circle. Valve only opens 90 degrees or 50 steps. 
 int currentHotStep; //stores current position of hot stepper motor
 int currentColdStep;//stores current position of cold stepper motor

 
//********************************************************************************************************************************** 
void setup() {

  /******************************
  *Setup for LCD and Input Buttons
  ******************************/

  tft.begin(); //start spi feed to LCD display

  //declare button pins are input source
  pinMode(buttonScrollNext, INPUT); 
  pinMode(buttonScrollBack, INPUT);
  pinMode(buttonSelect, INPUT);
  pinMode(buttonBack, INPUT);

  //create interrupts to handle button pushes on startup. 
  attachInterrupt(digitalPinToInterrupt(buttonScrollNext), buttonPressNext,FALLING ); //moves to next option in menu
  attachInterrupt(digitalPinToInterrupt(buttonScrollBack), buttonPressPrev,FALLING ); //moves to previous option in menu.
  attachInterrupt(digitalPinToInterrupt(buttonSelect), buttonPressSelect,FALLING ); //Select meu option
  attachInterrupt(digitalPinToInterrupt(buttonBack), buttonPressBack,FALLING ); // moves from option back to main menu
  tft.setRotation(1); //rotates output to put screen in landscape mode
  tft.setTextSize(2); //sets text sizez
  updateDisplay(); //function that updates display to show menu on boot up.
  
  /******************************
  *Setup for Flow Meter
  ******************************/
  pinMode(coldFlowPin, INPUT);           //Sets the pin as an input
  pinMode(hotFlowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(digitalPinToInterrupt(2), coldFlow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino) to run the function "Flow"  
  attachInterrupt(digitalPinToInterrupt(3), hotFlow, RISING);  //Configures interrupt 0 (pin 3 on the Arduino) to run the function "Flow"  

  /******************************
  *Setup for Thermistors
  ******************************/
  //analogReference(EXTERNAL);
  
  /******************************
  *Setup for Stepper Motors
  ******************************/
  pinMode(stepPinHot,OUTPUT);
  pinMode(dirPinHot,OUTPUT);
  pinMode(stepPinCold,OUTPUT);
  pinMode(dirPinCold,OUTPUT);
}

void loop() {
  //reset values
  countCold = 0;      // Reset the counter so we start counting from 0 again
  countHot = 0;      // Reset the counter so we start counting from 0 again
  
   //Calculates usage every second there is no interrupt active
   if((millis() - oldTime) > 1000)
   {
    noInterrupts(); //Disable the interrupts on the Arduino
    
    //calculate the flow rate. 
    flowRateCold = ((1000.0 / (millis() - oldTime)) * countCold) / 4.5; //calculates cold flow in gpm
    flowRateHot = ((1000.0 / (millis() - oldTime)) * countHot) / 4.5; //calculates hot flow in gpm

    oldTime = millis(); //set the stop interuppt time to old time. 

    flowGallonsCold = ((flowRateCold / 60)) /3.785411784;
    flowGallonsHot = ((flowRateHot / 60)) /3.785411784;

    //calculate total gallons used
    totalUsedCold =+ flowGallonsCold;
    totalUsedHot =+ flowGallonsHot;
    totalUsed = totalUsedCold + totalUsedHot;

    //reset counters and enable interruptss
    countCold = 0;
    countHot = 0;
    interrupts();   //Enables interrupts on the Arduino
   }
}
