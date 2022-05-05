#include <OneWire.h>
// Variables
  float temp = 0; //temperature measured by sensor
  long timer = 0; //current time
  long teaTime = 0; //time that the tea needs to be steeped
  int tempMax = 0; //max temperature to steep tea
  int tempMin = 0; //minimum temperature to steep tea
  int sensorRead;
  boolean heatOn; //determines whether the heater is on
  boolean LEDon; //determines whether the LED is on
  boolean notTea = true; //determined by the user input. 
                         //If the user inputs a string that is not expected, the loop continues.
  String tea; //User input name of the tea
  String userInput;

  //pins
  const int tempSensInput = 4; //digital
  const int ledOutput = 8; //digital
  const int relayOutput = 1; //digital

  OneWire ds(tempSensInput);
  
void setup() {
  Serial.begin(9600);
  
  pinMode(relayOutput, OUTPUT);
  digitalWrite(relayOutput, HIGH);

  pinMode(tempSensInput, INPUT);
  
  pinMode(ledOutput, OUTPUT);
  digitalWrite(ledOutput, HIGH);
}
void loop() {
  // put your main code here, to run repeatedly:
  while(notTea){
      Serial.println("Please fill receptacle with the proper amount of water and place your loose-leaf tea in the steeper and input the type of tea: ");
      Serial.println("black, green, white, oolong, manual");
      delay(3000);
      tea = Serial.readString(); //read user input for tea type, accounting for capitalization
      tea.toLowerCase();
      tea.trim();
      Serial.println(tea);
      if (tea == "black") {
        teaTime = 300000; //milliseconds
        tempMax = 212; //farenheiht
        tempMin = 210;
        Serial.print("Time: ");
        Serial.print(teaTime); 
        Serial.print(", Temp range: ");
        Serial.print(tempMin);
        Serial.print("-"); 
        Serial.println(tempMax);
        notTea = false;
      }
      else if (tea == "green") {
        teaTime = 180000;
        tempMax = 180;
        tempMin = 175;
        Serial.print("Time: ");
        Serial.print(teaTime); 
        Serial.print(", Temp range: ");
        Serial.print(tempMin);
        Serial.print("-"); 
        Serial.println(tempMax);
        notTea = false;
      }
      else if (tea == "white"){
        teaTime = 60000;
        tempMax = 185;
        tempMin = 175;
        Serial.print("Time: ");
        Serial.print(teaTime); 
        Serial.print(", Temp range: ");
        Serial.print(tempMin);
        Serial.print("-"); 
        Serial.println(tempMax);
        notTea = false;
      }
      else if (tea == "oolong"){
        teaTime = 60000;
        tempMax = 200;
        tempMin = 185;
        Serial.print("Time: ");
        Serial.print(teaTime); 
        Serial.print(", Temp range: ");
        Serial.print(tempMin);
        Serial.print("-"); 
        Serial.println(tempMax);
        notTea = false;
      }
      else if (tea == "manual") {
        Serial.println("input tea steep temperature in degrees Farenheit.");
        delay(3000);
        userInput = Serial.readString(); //read user input for temperature
        tempMax = userInput.toInt();
        tempMin = tempMax-10;
        Serial.println("input tea steep time in minutes.");
        delay(3000);
        userInput = Serial.readString(); //read user input for time.
        teaTime = userInput.toInt();
        Serial.print("Time: ");
        Serial.print(teaTime); 
        Serial.print(" minutes, Temp range: ");
        Serial.print(tempMin);
        Serial.print("-"); 
        Serial.println(tempMax);
        Serial.print(" degrees Farenheit.");
        teaTime = teaTime*60000; //converts to milliseconds
        notTea = false;
      }
      else {
        Serial.println("Input not valid."); //demand another input
      }
}
      //Steeping starts
      long current = millis();
      timer = millis()-current;
      Serial.println(timer);
      
        digitalWrite(relayOutput,LOW);
    while (timer < teaTime) {
      temp = (getTemp()*9/5)+32;
      Serial.println(temp);
      if (temp < tempMin) {
        digitalWrite(relayOutput,LOW);
        Serial.println("heat on");
        //turn on heater
      }
      else if (temp >= tempMax-1) {
        digitalWrite(relayOutput,HIGH);
        Serial.println("heat off");
        //turn off heater
      }
      timer = millis()-current;
      delay(500);
    }
    digitalWrite(relayOutput,HIGH);//turn off heater
    digitalWrite(ledOutput, LOW);//turn on LED
    Serial.println("It's done! Turn off the system before removing the heater, friend!"); //display finished message

}
float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}
