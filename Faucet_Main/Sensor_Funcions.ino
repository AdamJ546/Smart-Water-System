 /********************************************************
 * File contains all functions used by the flow meter
 * and thermistor to determinet the temperature and flow 
 * rate of the water. 
 ********************************************************/
 

/***************************************************************************
* void coldFlow
* Author: Adam Jennings
* Description: Function that is used to calculate the temperature and flow
* of water through the cold water line. 
* Parameters:
* countCold     I/P    counts the pulse from the hall effect sensor in the flow meter. 
* coldTemp      I/P    holds the current temperature reading for the cold water line. 
**************************************************************************/

void coldFlow()
{
   countCold++; 
   coldTemp = calcColdTemp(); 
}

/***************************************************************************
* void hotFlow
* Author: Adam Jennings
* Description: Function that is used to calculate the temperature and flow
* of water through the hot water line. 
* Parameters:
* countHot    I/P    counts the pulse from the hall effect sensor in the flow meter. 
* hotTemp      I/P    holds the current temperature reading for the hot water line. 
**************************************************************************/
void hotFlow()
{
   countHot++; //Every time this function is called increment count
   hotTemp = calcHotTemp(); //calculate the temperature of the hot water in C.
}

//calcTemp takes the average over all temperature readings to get an average temperature of the water. 

/****************************************************************************************************
* double calcHotTemp()
* Author: Adam Jennings
* Description: This function will take a predetermined number of sample temperature readings from the 
* hot water line and average them together to get the temperature of the hot water. 
* 
* Parameters:
* NUMSAMPLES      I/P     Set in main and controls the number of samples for temperature average.  
* samples         I/P     Array that stores each samples reading.  
* THERMISTORPINHO I/P     Reads annalog value in from thermistor 
* average         I/P     Stores average of samples. 
* SERIESRESISTOR  I/P     Set in main and stores the resistor used in series with thermistor
* steinhart       O/P     Used to connvert the average resistance from sample into temperature in K. 
* THERMISTORNOMINAL I/P   Set in main and signifies temperature when thermistor reads 10k ohm resistance. 
* BCOEFFICIENT    I/P     Beta coeffiecent provided in the spec sheet from thermistor manufacturer. 
****************************************************************************************************/
double calcHotTemp()
{
      float average;
     
      // take N samples in a row, with a slight delay
      for (int i=0; i< NUMSAMPLES; i++) {
       samples[i] = analogRead(THERMISTORPINHOT);
       delay(10);
      }
      
      // average all the samples out
      average = 0;
      for (int i=0; i< NUMSAMPLES; i++) {
         average += samples[i];
      }
      average /= NUMSAMPLES;
      
      // convert the value to resistance
      average = 1023 / average - 1;
      average = SERIESRESISTOR / average;

      //apply steinhart equation to calculate temp in C. Equation pulled from spec sheet of part and is used to convert the resistance of thermistor into temp in K. 
      float steinhart;
      steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
      steinhart = log(steinhart);                  // ln(R/Ro)
      steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
      steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
      steinhart = 1.0 / steinhart;                 // Invert
      steinhart -= 273.15;                         // convert to C
      steinhart = (steinhart*1.8) +32;             // convert to F
      
      return steinhart; //return result. 
}

/****************************************************************************************************
* double calcColdTemp()
* Author: Adam Jennings
* Description: This function will take a predetermined number of sample temperature readings from the 
* cold water line and average them together to get the temperature of the cold water. 
* 
* Parameters:
* NUMSAMPLES      I/P     Set in main and controls the number of samples for temperature average.  
* samples         I/P     Array that stores each samples reading.  
* THERMISTORPINHO I/P     Reads annalog value in from thermistor 
* average         I/P     Stores average of samples. 
* SERIESRESISTOR  I/P     Set in main and stores the resistor used in series with thermistor
* steinhart       O/P     Used to connvert the average resistance from sample into temperature in K. 
* THERMISTORNOMINAL  I/P  Set in main and signifies the base resistance when room temperature. (10k ohm)
* TEMPERATURENOMINAL I/P  Set in main and signifies the value for room temperature. (25C)
* BCOEFFICIENT       I/P  Beta coeffiecent provided in the spec sheet from thermistor manufacturer. 
****************************************************************************************************/
double calcColdTemp()
{
      float average;
     
      // take N samples in a row, with a slight delay
      for (int i=0; i< NUMSAMPLES; i++) {
       samples[i] = analogRead(THERMISTORPINCOLD);
       delay(10);
      }
      
      // average all the samples out
      average = 0;
      for (int i=0; i< NUMSAMPLES; i++) {
         average += samples[i];
      }
      average /= NUMSAMPLES;
     
      
      // convert the value to resistance
      average = 1023 / average - 1;
      average = SERIESRESISTOR / average;
     
      //apply steinhart equation to calculate temp in C. Equation pulled from spec sheet of part and is used to convert the resistance of thermistor into temp in K. 
      float steinhart;
      steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
      steinhart = log(steinhart);                  // ln(R/Ro)
      steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
      steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
      steinhart = 1.0 / steinhart;                 // Invert
      steinhart -= 273.15;                         // convert to C
      steinhart = (steinhart*1.8) +32;              // convert to F
      
      return steinhart; // return temp in C. 
}
