#include <LiquidCrystal.h>            //the liquid crystal library contains commands for printing to the display
LiquidCrystal lcd(10, 9, 8, 7, 6, 5);     // tell the RedBoard what pins are connected to the display

int potPosition;                      //this variable will hold a value based on the position of the potentiometer
float distance = 0;                   //stores the distance measured by the distance sensor
int blueButton = 2;                   //pin that the button is connected to
int redButton = 13;                    //pin that the button is connected to
const int trigPin = 11;               //connects to the trigger pin on the distance sensor
const int echoPin = 12;               //connects to the echo pin on the distance sensor
int buzzerPin = 3;                    //pin for driving the buzzer
long timeLimit = 15000;               //time limit to input the passcode
int buttonPressTime = 0;              //variable to show how much time left to input the passcode

long startTime = 0;                   //used to measure time that has passed    // might be able to get the time when alarm is set,
                                      // then maybe even a costant for timeLimit
const int passCodeSize = 4;
bool passcode[passCodeSize] = {-1, -1, -1, -1};    // passcode to stop alarm
bool armed = false;
bool validCode = false;
int userPasscode[passCodeSize] = { -1, -1, -1, -1 };

// set last buttons pressed
bool blueLastButtonState = false;
bool redLastButtonState = false;

void setup() {
  lcd.begin(16, 2);                       //tell the LCD library the size of the screen

  pinMode(A0, OUTPUT);                    //set the LED blue to output
  pinMode(A1, OUTPUT);                    //set the LED red to output
  pinMode(A3, INPUT);                     //Potentiometer pin as input
  pinMode(buzzerPin, OUTPUT);             //set the buzzer pin to output
  pinMode(trigPin, OUTPUT);               //the trigger pin will output pulses of electricity
  pinMode(echoPin, INPUT);                //the echo pin will measure the duration of pulses coming back from the distance sensor
  
  pinMode(blueButton, INPUT_PULLUP);      //the trigger pin will output pulses of electricity
  pinMode(redButton, INPUT_PULLUP);       //set the button pins as an input


  lcd.clear();
  lcd.print("Setup: Welcome");               // Welcome to setup!
  lcd.setCursor(0, 1);
  lcd.print("this is Setup!");
  delay(5000);
  lcd.clear();
  lcd.print("Setup: The knob");
  lcd.setCursor(0, 1);
  lcd.print("is for distance");
  delay(5000);
  lcd.clear();
  lcd.print("Setup: The R/B");
  lcd.setCursor(0, 1);
  lcd.print("buttons are N/Y");
  delay(5000);
  lcd.clear();
  lcd.print("Setup: Press Y");
  lcd.setCursor(0, 1);
  lcd.print("to continue!");
  while(true){
    bool blue = isButtonPressed(blueButton);
    if (blue) {
      analogWrite(A0, 150);
      delay(500);
      break;
    }
  }
  analogWrite(A0, 0);
  lcd.clear();
  lcd.print("Setup: Use knob");               // set distance
  lcd.setCursor(0, 1);
  lcd.print("to set distance");
  delay(3000);
  lcd.setCursor(0, 1);
  lcd.print("detect at:      ");
  while(true){                                        // get button press
    if ( millis() % 1000 > 100){            // check distance less often
      potPosition = analogRead(A2);         //set potentiometer to a number between 0 and 1023
      distance = getDistance();             //variable to store the distance measured by the sensor
      if (distance < (potPosition / 5)){
        analogWrite(A0, 150);
        analogWrite(A1, 0);
      }else{
        analogWrite(A1, 150);
        analogWrite(A0, 0);
      }
      if ((potPosition / 5) < 100){
        lcd.setCursor(12, 1);
        lcd.print(potPosition / 5);
        lcd.setCursor(14, 1);
        lcd.print(" ");
      }else{
        lcd.setCursor(12, 1);
        lcd.print(potPosition / 5);
      }
    }
    bool blue = isButtonPressed(blueButton);
    if (blue) {
      analogWrite(A0, 150);
      analogWrite(A1, 0);
      delay(500);
      break;
    }
  }
  analogWrite(A1, 0);
  analogWrite(A0, 0);
  lcd.clear();
  lcd.print("Setup: Create a");               // set passcode
  lcd.setCursor(0, 1);
  lcd.print("code: ");
  bool verifyPrompt = true;         
  int buttonCount = 0;
  while(!validCode){                
    while(buttonCount < 4){                    // input passcode
      bool anyButton = false;
      bool blue = isButtonPressed(blueButton);
      bool red = isButtonPressed(redButton);
      if (blue && !blueLastButtonState) {
        analogWrite(A0, 150);
        analogWrite(A1, 0);
        anyButton = true;
        lcd.print(blue);
      }
      if (red && !redLastButtonState) {
        analogWrite(A0, 0);
        analogWrite(A1, 150);
        anyButton = true;
        lcd.print(blue);
      }
      if (anyButton) {
        passcode[passCodeSize - 1] = blue;
        buttonCount++;
        // shift array to left
        int temp = passcode[0];                 //remember first element
        for (int i = 0; i < passCodeSize - 1; i++)
        {
          passcode[i] = passcode[i + 1];        //move all element to the left and remove the first one
        }
        passcode[passCodeSize - 1] = temp;      //assign remembered value to last element
      }
      blueLastButtonState = blue;
      redLastButtonState = red;
    }
    if (verifyPrompt && buttonCount >= 4){
      delay(500);
      lcd.clear();
      lcd.print("Re-input code to");
      lcd.setCursor(0, 1);
      lcd.print("verify: ");
      verifyPrompt = false;
    }
    while(buttonCount >= 4 && buttonCount < 8 && !verifyPrompt){      // verify new passcode
      validCode = true;
      bool anyButton = false;
      bool blue = isButtonPressed(blueButton);
      bool red = isButtonPressed(redButton);
      if (blue && !blueLastButtonState) {
        analogWrite(A0, 150);
        analogWrite(A1, 0);
        anyButton = true;
        lcd.print(blue);
      }
      if (red && !redLastButtonState) {
        analogWrite(A0, 0);
        analogWrite(A1, 150);
        anyButton = true;
        lcd.print(blue);
      }
      if (anyButton) {
        userPasscode[passCodeSize - 1] = blue;
        buttonCount++;
        // shift array first then verify        
        int temp = userPasscode[0];                 //remember first element
        for (int i = 0; i < passCodeSize - 1; i++)
        {
          userPasscode[i] = userPasscode[i + 1];        //move all element to the left and remove the first one
        }
        userPasscode[passCodeSize - 1] = temp;      //assign remembered value to last element
        
        for (int j = 0; j < 4; j++)
        {
          if (passcode[j] != userPasscode[j])
          {
            validCode = false;
            break;
          }
        }
      }
      blueLastButtonState = blue;
      redLastButtonState = red;
    }
    if (buttonCount >= 8 && !validCode){
      delay(500);
      lcd.clear();
      lcd.print("Try again");
      delay(2000);
      lcd.clear();
      verifyPrompt = true;
      buttonCount = 0;
      analogWrite(A0, 0);
      analogWrite(A1, 0);
      for (size_t i = 0; i < passCodeSize; i++)
      {
        userPasscode[i] = -1;
        passcode[i] = -1;
      }
      lcd.print("Setup: Create a");
      lcd.setCursor(0, 1);
      lcd.print("code: ");
    }else if (validCode){
      delay(500);
      lcd.clear();
      lcd.print("Valid: Arm? N/Y");
      lcd.setCursor(0, 1);
      bool anyButton = false;
      while(!anyButton){                                        // Arm alarm?
        bool blue = isButtonPressed(blueButton);
        bool red = isButtonPressed(redButton);
        if (blue) {
          analogWrite(A0, 150);
          analogWrite(A1, 0);
          lcd.print(blue);
          anyButton = true;
          armed = true;
        }
        if (red) {
          analogWrite(A0, 0);
          analogWrite(A1, 150);
          lcd.print(blue);
          anyButton = true;
          armed = false;
        }
      }
    }
  }
  if(armed){                                      // Arm alarm!
    delay(500);
    ArmAlarm(true);
  }else{
    delay(500);
    ArmAlarm(false);
  }
}

void loop() {
  if(armed){
    if (millis() % 1000 > 100){             // delay(100);           // delay checking for person
      potPosition = analogRead(A2);         //set potentiometer to a number between 0 and 1023
      distance = getDistance();             //variable to store the distance measured by the sensor
      if (distance < (potPosition / 5))
      {
        validCode = false;
        lcd.clear();
        lcd.print("Cops are on the");
        lcd.setCursor(0, 1);
        lcd.print("way!");
        
        startTime = millis();         // Start time that for dissarming alarm
        while (!validCode)
        {
          int roundedTime = round((timeLimit - (millis() - startTime)) / 1000); //calculate the time left, dividing by 1000 converts to seconds
          if (millis() - startTime > timeLimit){
            lcd.clear();
            lcd.print("Your going away");
            lcd.setCursor(0, 1);
            lcd.print("for along time!");
            delay(5000);
            ArmAlarm(true);
            break;
          }
          if (millis() % 500 > 100){
          if (roundedTime < 10){        
            lcd.setCursor(14, 1);                                                //set the cursor in the lower right corner of the screen
            lcd.print(" ");                                                      //get rid of the last number
            lcd.print(roundedTime);
          }else{
            lcd.setCursor(14, 1);                                                //set the cursor in the lower right corner of the screen
            lcd.print(roundedTime);
          }
          }

          if (millis() % 1000 > 500){
            analogWrite(A0, 150);           // red
            analogWrite(A1, 0);
            tone(buzzerPin, 130, 250);      //E6        weeeee
          }
          if (millis() % 1000 < 500){
            analogWrite(A0, 0);
            analogWrite(A1, 150);           // blue
            tone(buzzerPin, 73, 250);       //G6        wooooo
          }
      
          if (CheckForCode())
          {
            delay(500);
            ArmAlarm(false);
          }
        }
      }
    }
    if (CheckForCode())
    {
      delay(500);
      ArmAlarm(false);
    }
  }else{                  // wait for code to arm
    if (CheckForCode())
    {
      delay(500);
      ArmAlarm(true);
    }
  }
}


//----------------------FUNCTIONS-------------------------------
//RETURNS THE DISTANCE MEASURED BY THE HC-SR04 DISTANCE SENSOR
float getDistance()
{
  float echoTime;                   //variable to store the time it takes for a ping to bounce off an object
  float calculatedDistance;         //variable to store the distance calculated from the echo time

  //send out an ultrasonic pulse that's 10ms long
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  echoTime = pulseIn(echoPin, HIGH);      //use the pulsein command to see how long it takes for the
                                          //pulse to bounce back to the sensor

  calculatedDistance = echoTime / 148.0;  //calculate the distance of the object that reflected the pulse (half the bounce time multiplied by the speed of sound)

  return calculatedDistance;              //send back the distance that was calculated
}

bool isButtonPressed(uint8_t pin){
  for (int i = 0; i < 1000; i++){
    if (digitalRead(pin) == HIGH){
        return false;
    }
  }
  return true;
}

void ArmAlarm(bool _armed){
  analogWrite(A1, 0);
  analogWrite(A0, 0);
  lcd.clear();
  armed = _armed;
  for (size_t i = 0; i < passCodeSize; i++)
  {
    userPasscode[i] = -1;
  }
  if(armed){
    lcd.print("Armed");
    lcd.setCursor(0, 1);
  }else{
    lcd.print("Unarmed");
    lcd.setCursor(0, 1);
    lcd.print("Arm code: ");
  }
}

bool CheckForCode() {
  // get input
  validCode = false;
  bool anyButton = false;
  bool blue = isButtonPressed(blueButton);
  bool red = isButtonPressed(redButton);
  if (blue && !blueLastButtonState) {
    analogWrite(A0, 150);
    analogWrite(A1, 0);
    anyButton = true;
    lcd.print(blue);
  }
  if (red && !redLastButtonState) {
    analogWrite(A0, 0);
    analogWrite(A1, 150);
    anyButton = true;
    lcd.print(blue);
  }
  blueLastButtonState = blue;
  redLastButtonState = red;
  if (anyButton) {
    validCode = true;
    userPasscode[passCodeSize - 1] = blue;
    // shift array to left
    int temp = userPasscode[0];                 //remember first element
    for (int i = 0; i < passCodeSize - 1; i++)
    {
      userPasscode[i] = userPasscode[i + 1];    //move all element to the left and remove the first one
    }
    userPasscode[passCodeSize - 1] = temp;      //assign remembered value to last element
    for (int j = 0; j < 4; j++)
    {
      if (passcode[j] != userPasscode[j])
      {
        validCode = false;
        break;
      }
    }
    if (validCode)
    {
      return validCode;
    }
  }
  return validCode;
}
