 /***************************************************************************
 * File contains all functions used by the stepper motors to control ball valves
 * to adjust the flow of water. 
 ***************************************************************************/

/***************************************************************************
* void openFaucet()
* Author: Adam Jennings
* Description: Opens faucet to the set flow rate at 50/50 mixure. After 60
* seconds the system will calculate the desired temperature and flow rate and
* excute a function to adjust the flow rate to achieve desired flow rate and
* temperature. 
* 
* Parameters:
* hotRate          I/P    1/2 the desired flow rate plus the modulo
* coldRate         I/P    1/2 the desired flow rate
* desiredFlowRate  I/P    stores the users desired flow rate
* startTime        I/P    stores the system time water started to use in 60 second timer. 
**************************************************************************/
void openFaucet()
{
  //takes desired flow rate and  divides by 2. Fully open valve is 50 steps and desired can be set between 0 and 100% open. Then add the modulus of the flow rate back to account for odd numbers.
  int hotRate = desiredFlowRate/2 + desiredFlowRate%2;
  int coldRate = desiredFlowRate/2;

  // call functions to open water 
  openHot(hotRate);
  openCold(coldRate);

  //create 60 second delay to allow time for hot water to reach faucet
  int startTime = millis();
  while (millis()-startTime < 60000000)
  {
    //dummy loop that acts as delay. Using millis() does not pevent alterante process from running while using the delay()function stops all process during delay.
  }

  //adjust temp to match desired temp
  adjustTemp();
}

/***************************************************************************
* void adjustTemp()
* Author: Adam Jennings
* Description: Takes the input current temperature of hot and cold water and 
* calculates the ratio of each water line to determine the ratio to create
* the desired temperature. 
* 
* Parameters:
* hot                I/P      stores hot ratio
* hotTemp            I/P      stores current temp of hot water
* desiredTemp        I/P      stores the current desired temperature
* cold               I/P      stores cold ratio
* coldTemp           I/P      stores current temp of cold water 
* hotFlow            I/P      determines the ratio to open hot water valve
* coldFlow           I/P      determines the ratio to open cold water valve
* desiredFlowRate    I/P      stores desired flow rate of water
* maxStep            I/P      stores the max valve open limit. 
**************************************************************************/
void adjustTemp()
{
    //Start by finding the ratio of hot temp - desired temp : desired temp - cold temp
    int hot = hotTemp - desiredTemp;
    int cold = desiredTemp-coldTemp;

    //use current ratio to calcualte flow rate
    int hotFlow = desiredFlowRate * (hot/cold);
    int coldFlow = desiredFlowRate * (cold/hot);

    if(hotFlow/2 <= maxStep)
    {
      coldFlow = desiredFlowRate/2;
      openCold(coldFlow);
      openHot(hotFlow);
    }

    if (hotFlow/2 > maxStep)
    {
      hotFlow = desiredFlowRate/2 + desiredFlowRate%2;
      openCold(coldFlow);
      openHot(hotFlow);
    }
}


/***************************************************************************
* void openHot(int openAmount)
* Author: Adam Jennings
* Description: Used to open stepper to the desired flow rate for  hot water. 
* 
* Parameters:
* openAmount         I/P      stores requested amount to adjust the stepper. 
* currentHotStep     I/P      stores what step the stepper motor is on to dteremine how open or closed the valve is. 
* maxStep            I/P      stores the max valve open limit. 
**************************************************************************/
void openHot(int openAmount)
{
  digitalWrite(dirPinHot,HIGH); // set direction to open.
  for(int x=0; x<openAmount && x+currentHotStep<maxStep; x++)
  {
    digitalWrite(stepPinHot,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPinHot,LOW);
    delayMicroseconds(500);
  }
  if (currentHotStep + openAmount < maxStep)
      currentHotStep = currentHotStep + openAmount;//set curent step to the amount valve is open. 
  else if (currentHotStep + openAmount >= maxStep)
           currentHotStep = 0;
}

/***************************************************************************
* void openCold(int openAmount)
* Author: Adam Jennings
* Description: Used to open stepper to the desired flow rate for cold water. 
* 
* Parameters:
* openAmount         I/P      stores requested amount to adjust the stepper. 
* currentColdStep    I/P      stores what step the stepper motor is on to dteremine how open or closed the valve is. 
* maxStep            I/P      stores the max valve open limit. 
**************************************************************************/
void openCold(int openAmount)
{
  digitalWrite(dirPinCold,HIGH); // set direction to open. 
  for(int x=0; x<openAmount && x+currentColdStep<maxStep; x++)
  {
    digitalWrite(stepPinCold,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPinCold,LOW);
    delayMicroseconds(500);
  }

  if (currentColdStep + openAmount < maxStep)
      currentColdStep = currentColdStep + openAmount; //set curent step to the amount valve is open. 
  else if (currentColdStep + openAmount >= maxStep)
           currentColdStep = 0;
}

/***************************************************************************
* void closeHot(int closeAmount)
* Author: Adam Jennings
* Description: Used to close stepper to the desired flow rate for hot water. 
* 
* Parameters:
* closeAmount        I/P      stores requested amount to adjust the stepper. 
* currentHotStep     I/P      stores what step the stepper motor is on to dteremine how open or closed the valve is.  
**************************************************************************/
void closeHot(int closeAmount)
{
  digitalWrite(dirPinHot,LOW); // set direction to close. 
  if (closeAmount < currentHotStep)
  {
    for(int x=0; x<closeAmount; x++)
    {
      digitalWrite(stepPinHot,HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPinHot,LOW);
      delayMicroseconds(500);
    }
  }else if(closeAmount <= currentHotStep)
  {
    closeHotOff();
  }
  currentHotStep = currentHotStep - closeAmount; //set curent step to the amount valve is open. 
}

/***************************************************************************
* void closeCold(int closeAmount)
* Author: Adam Jennings
* Description: Used to close stepper to the desired flow rate for cold water. 
* 
* Parameters:
* closeAmount        I/P      stores requested amount to adjust the stepper. 
* currentColdStep    I/P      stores what step the stepper motor is on to dteremine how open or closed the valve is.  
**************************************************************************/
void closeCold(int closeAmount)
{
  digitalWrite(dirPinCold,LOW); 
  // set direction to close. 
  if (closeAmount < currentColdStep)
  {
    for(int x=0; x<closeAmount; x++)
    {
      digitalWrite(stepPinCold,HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPinCold,LOW);
      delayMicroseconds(500);
    }
  }else if(closeAmount >= currentColdStep)
    {
      closeColdOff();
    }
  currentColdStep = currentColdStep - closeAmount; //set curent step to the amount valve is open. 
}

/***************************************************************************
* void closeHotOff()
* Author: Adam Jennings
* Description: Used to close stepper all the way 
* 
* Parameters:
* currentHotStep    I/P      stores what step the stepper motor is on to dteremine how open or closed the valve is.  
**************************************************************************/
void closeHotOff()
{
  digitalWrite(dirPinHot,LOW);  //set direction to close.
  for(int x=currentHotStep; x>0; x--)
  {
    digitalWrite(stepPinHot,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPinHot,LOW);
    delayMicroseconds(500);
  }
  currentHotStep = 0; //set curent cold stepper position. 

}

/***************************************************************************
* void closeColdOff()
* Author: Adam Jennings
* Description: Used to close stepper all the way 
* 
* Parameters:
* currentColdStep    I/P      stores what step the stepper motor is on to dteremine how open or closed the valve is.  
**************************************************************************/
void closeColdOff()
{
  digitalWrite(dirPinCold,LOW); //set direction to close.
  for(int x=currentColdStep; x>0; x--)
  {
    digitalWrite(stepPinCold,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPinCold,LOW);
    delayMicroseconds(500);
  }
  currentColdStep = 0;//set curent cold stepper position. 

}
