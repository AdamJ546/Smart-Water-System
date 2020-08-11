 /***************************************************************************
 * File contains all functions used by the LCD screen and buttons on the 
 * faucet display.
 ***************************************************************************/

/***************************************************************************
* void updateDisplay(){
* Author: Adam Jennings
* Description: This function is used to refreh the display.  
* 
* Parameters:
* button             I/P    Used to store how many button presses and determines what options highlighted on the menu.  
**************************************************************************/

//Function that determines which menu to show.
void updateDisplay(){

    if(button == 0)
       mainMenu0();

    if(button == 1)
       mainMenu1();

    if(button == 2)
      mainMenu2();

    //removes old interrupts   
    detachInterrupt(digitalPinToInterrupt(buttonScrollNext)); 
    detachInterrupt(digitalPinToInterrupt(buttonScrollBack));
    detachInterrupt(digitalPinToInterrupt(buttonSelect));

    // reinstates startup interrupts
    attachInterrupt(digitalPinToInterrupt(buttonScrollNext), buttonPressNext,FALLING );
    attachInterrupt(digitalPinToInterrupt(buttonScrollBack), buttonPressPrev,FALLING );
    attachInterrupt(digitalPinToInterrupt(buttonSelect), buttonPressSelect,FALLING );
  }


/***************************************************************************
* void dispSetTemp()
* Author: Adam Jennings
* Description: When the user selects the option to set the desired temp this 
* function will display the set temp menu.   
* 
* Parameters:
* selectTemp             I/P    Shows the temporary temperature the user is considering but has not comitted yet.   
**************************************************************************/
void dispSetTemp()
{
    tft.setCursor(0, 0);
    tft.fillScreen(ILI9341_BLUE);
    tft.println("Set desired temperature");
    tft.println("(40F to 115F) \n\n");
    tft.print("          ");
    tft.print(selectTemp);
    tft.print("F");
}

/***************************************************************************
* void dispSetFlow()
* Author: Adam Jennings
* Description: When the user selects the option to set the desired flow rate 
* this function will display the set flow rate menu.   
* 
* Parameters:
* selectFlowRate   I/P    Shows the temporary flow rate the user is considering but has not comitted yet.   
**************************************************************************/
void dispSetFlow()
{
    tft.setCursor(0, 0);
    tft.fillScreen(ILI9341_BLUE);
    tft.println("Set desired flow rate");
    tft.println("(0 to 100%) \n\n");
    tft.print("          ");
    tft.print(selectFlowRate);
    tft.print("%");
}


/***************************************************************************
* void buttonPressNext()
* Author: Adam Jennings
* Description: Used to count button presses to traverse the main menu.   
* When end of menu is reached cycles back to the top. 
* 
* Parameters:
* button   I/P    stores button position   
**************************************************************************/
void buttonPressNext() {
  button = button+1; //increase button position

  //hit end of menu cycle to top. 
  if (button == 3)
      button = 0;

  //shows updated menu
    updateDisplay(); 
} 

/***************************************************************************
* void buttonPressPrev()
* Author: Adam Jennings
* Description: Used to count button presses to traverse the main menu backwards.   
* When top of menu is reached cycles back to the bottom. 
* 
* Parameters:
* button   I/P    stores button position   
**************************************************************************/
void buttonPressPrev() {
  button = button-1; //decreases button position

  //if button is at top option cycle to last option
  if (button < 0)
      button = 2;

  //show updated menu
   updateDisplay();
} 

/***************************************************************************
* void buttonPressSelect()
* Author: Adam Jennings
* Description: Used to turn on/off the faucet, enter set temperature mode, or 
* enter set flow rate mode based on button value. 
* 
* Parameters:
* button        I/P    stores button position   
* faucet        I/P    stores weather faucet is off (0) or faucet is on (1)
* selectTemp    I/P    temporary value that takes on the current temp setting and allows user to adjust and select new temperature before comitting. 
* desiredTemp   I/P    current set temperature
* selectFlowRate  I/P  temporary value that takes on the current flow rate  and allows user to adjust and select new flow rate before comitting.
* desiredFlowRate I/P  current set flow rate
**************************************************************************/
void buttonPressSelect() {

  //if faucet is turned off then faucet will turn on. 
  if (button == 0 && faucet == 0){
    faucet = 1;
    openFaucet();
    updateDisplay();
  }//if faucet is on it will turn off
  else if( button == 0 && faucet == 1){
    faucet = 0;
    closeHotOff();
    closeColdOff();
    updateDisplay();
  }//if set temp is highlighted then display set temp options. 
  else if(button == 1){
    //disable startup interrupt
    detachInterrupt(digitalPinToInterrupt(buttonScrollNext));
    detachInterrupt(digitalPinToInterrupt(buttonScrollBack));
    detachInterrupt(digitalPinToInterrupt(buttonSelect));
    //enable interrupt to control increase or decrease of temp
    attachInterrupt(digitalPinToInterrupt(buttonScrollNext), setIncrease,FALLING );
    attachInterrupt(digitalPinToInterrupt(buttonScrollBack), setDecrease,FALLING );
    attachInterrupt(digitalPinToInterrupt(buttonSelect), setValue,FALLING );
    selectTemp = desiredTemp; // copies current desired temp into variable to temporary variable for manipulation
    dispSetTemp(); //go to set temp screen
    
  }//if set flow rate is highlighted then display set flow rate options. 
  else if(button == 2){
    //disable startup interrupt
    detachInterrupt(digitalPinToInterrupt(buttonScrollNext));
    detachInterrupt(digitalPinToInterrupt(buttonScrollBack));
    detachInterrupt(digitalPinToInterrupt(buttonSelect));
    //enable interrupt to control increase or decrease of flow rate
    attachInterrupt(digitalPinToInterrupt(buttonScrollNext), setIncrease,FALLING );
    attachInterrupt(digitalPinToInterrupt(buttonScrollBack), setDecrease,FALLING );
    attachInterrupt(digitalPinToInterrupt(buttonSelect), setValue,FALLING );
    selectFlowRate = desiredFlowRate; //copies current desired flow rate into temporary variable for manipulation
    dispSetFlow();
  }
} 


/***************************************************************************
* void buttonPressBack()
* Author: Adam Jennings
* Description: takes user back to main menu without saving any changes. 
* 
* Parameters:
**************************************************************************/
void buttonPressBack() {
  updateDisplay();
} 

/***************************************************************************
* void setIncrease()
* Author: Adam Jennings
* Description: Function that acts as interrupt to increase the temperature
* or flow rate value based on button value when the user is in the set 
* temperature or flow rate screens. 
* 
* Parameters:
* button          I/P   stores button position  
* selectTemp      I/P   holds temporary temperature value before comitting. 
* selectFlowRate  I/P   holds temporary flow rate value before comitting. 
**************************************************************************/
void setIncrease()
{
  //if set temp screen
  if (button == 1)
  {
      selectTemp = selectTemp + 5; //increase temproary temp variable by 5

      //if temp variable exceeds 115F then go to minimum setting 40F
      if (selectTemp > 115)
          selectTemp = 40;
      //update displat to show new proposed value
      dispSetTemp();
  }
  // if set flow rate screen
  if (button == 2)
  {
      selectFlowRate = selectFlowRate + 5; //increase flow by 5%

      // if flow is at 100% then reset flow to off. 
      if (selectFlowRate > 100)
          selectFlowRate = 0;

      //update to show new proposed value
      dispSetFlow();
  }
}

/***************************************************************************
* void setDecrease()
* Author: Adam Jennings
* Description: Function that acts as interrupt to decrease the temperature
* or flow rate value based on button value when the user is in the set 
* temperature or flow rate screens. 
* 
* Parameters:
* button          I/P   stores button position  
* selectTemp      I/P   holds temporary temperature value before comitting. 
* selectFlowRate  I/P   holds temporary flow rate value before comitting. 
**************************************************************************/
void setDecrease()
{
  //if on set temp screen
  if (button == 1)
  {
      selectTemp = selectTemp - 5; //decrease temporary value by 5

      //if decreased below 40 reset to 115 or highest value
      if (selectTemp < 40)
          selectTemp = 115;
      //update to show new proposed value
      dispSetTemp();
  }
  //if on set flow rate screen
  if (button == 2)
  {
      selectFlowRate = selectFlowRate - 5; //decrease flow by 5
      //if flow goes below 0 reset to 100
      if (selectFlowRate < 0)
          selectFlowRate = 100;
      //update to show new proposed value
      dispSetFlow();
  }
}

/***************************************************************************
* void setValue()
* Author: Adam Jennings
* Description: Function that acts as interrupt to store the temporary values
* as the desired values when user is in the set temp or flow rate screens.
* 
* Parameters:
* button           I/P   stores button position  
* desiredTemp      I/P   stores the current temp
* selectTemp       I/P   holds temporary flow rate value before comitting. 
* desiredFlowRate  I/P   stores the current flow rate
* selectFlowRate   I/P   holds temporary flow rate value before comitting. 
**************************************************************************/
void setValue()
{
  //if select is pushed set the temporary temp value as new desired temp value
  if (button == 1)
  {
    desiredTemp =selectTemp;
    adjustTemp();
  }
  //if select is pushed set the temporary flow value as new desired flow value
  if (button == 2)
  {
    desiredFlowRate = selectFlowRate;
    adjustTemp();
  }
    
  //update display to show menu and reset interrupts back to startup interrupts 
  updateDisplay();
}


/***************************************************************************
* unsigned long mainMenu
* Author: Adam Jennings
* Description: All mainMenu functions show the main menu options on the LCD 
* with each function displaying a different option as highlights. The function
* to display is selected int he updatedDisplay() function and is based on the 
* value stored in button. 
* 
* Menu 0 allows facute to turn on and off
 * Menu 1 set temperature
 * Menu 2 set flow rate
**************************************************************************/
unsigned long mainMenu0() {

  tft.fillScreen(ILI9341_BLUE);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_RED); 
  if(faucet == 0)
    tft.println("Turn Faucet On");
  else if (faucet == 1)
      tft.println("Turn Faucet off");
  tft.setTextColor(ILI9341_WHITE);  
  tft.println("Set Temperature");
  tft.setTextColor(ILI9341_WHITE);  
  tft.println("Set Flow Rate");
  tft.setCursor(0, 120);
  tft.print("Desired Temp: ");
  tft.println(desiredTemp);
  tft.print("Current Temp: ");
  tft.println(currentTemperature);
  tft.print("Desired Flow Rate: ");
  tft.println(desiredFlowRate);
  tft.print("Water Used: ");
  tft.print(totalUsed);
  tft.print("gpm");
  return micros() - start;
}

unsigned long mainMenu1() {

  tft.fillScreen(ILI9341_BLUE);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  
  if(faucet == 0)
    tft.println("Turn Faucet On");
  else if (faucet == 1)
      tft.println("Turn Faucet off");
  tft.setTextColor(ILI9341_RED);  
  tft.println("Set Temperature");
  tft.setTextColor(ILI9341_WHITE);  
  tft.println("Set Flow Rate");
  tft.setCursor(0, 120);
  tft.print("Desired Temp: ");
  tft.println(desiredTemp);
  tft.print("Current Temp: ");
  tft.println(currentTemperature);
  tft.print("Desired Flow Rate: ");
  tft.println(desiredFlowRate);
  tft.print("Water Used: ");
  tft.print(totalUsed);
  tft.print("gpm");
  return micros() - start;
}

unsigned long mainMenu2() {

  tft.fillScreen(ILI9341_BLUE);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  
  if(faucet == 0)
    tft.println("Turn Faucet On");
  else if (faucet == 1)
      tft.println("Turn Faucet off");
  tft.setTextColor(ILI9341_WHITE);  
  tft.println("Set Temperature");
  tft.setTextColor(ILI9341_RED);  
  tft.println("Set Flow Rate");
  tft.setCursor(0, 120);
  tft.setTextColor(ILI9341_WHITE); 
  tft.print("Desired Temp: ");
  tft.println(desiredTemp);
  tft.print("Current Temp: ");
  tft.println(currentTemperature);
  tft.print("Desired Flow Rate: ");
  tft.println(desiredFlowRate);
  tft.print("Water Used: ");
  tft.print(totalUsed);
  tft.print("gpm");
  return micros() - start;
}
