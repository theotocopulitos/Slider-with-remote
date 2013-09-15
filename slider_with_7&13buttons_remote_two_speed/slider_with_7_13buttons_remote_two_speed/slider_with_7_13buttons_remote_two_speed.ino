/*

 Stepper Motor Single Channel Manual Controller
 language: Wiring/Arduino

 This program drives a single bi-polar stepper motor using an EasyDriver
 stepper motor controller from SparkFun.
 It takes input from a set of switches and potentiometers.

 The goal is to create smooth motion of the steppper motor without having a 
 PC attached to the Arduino.
*/

/* apm - on/off button is used to toggle between two different speeds */

#include <AccelStepper.h>
#include <IRLib.h>


// Define some steppers and the pins the will use

AccelStepper stepper1(1, 12, 11); 

#define stepsPerRev      1600
#define stepPin          12
#define dirPin           11
#define ledPin           13

#define remotePin        7

#define rotateLeft       0x100D
#define rotateRight      0x100B
#define savePositionA    0x1020
#define savePositionB    0x1021
#define gotoPositionA    0x1011
#define gotoPositionB    0x1010
#define speedToggle      0x100C

#define rotateLeft_13       0x40BFB24D
#define rotateRight_13      0x40BFF20D
#define savePositionA_13    0x40BF708F
#define savePositionB_13    0x40BFB04F
#define gotoPositionA_13    0x40BF50AF
#define gotoPositionB_13    0x40BFA857
#define speedUp_13          0x40BFAA55		
#define speedReset_13       0x40BF2AD5
#define speedDown_13        0x40BFCA35
#define stop_13             0x40BF00FF

#define printDebug       1
 

//int buttonState = 0;
//int stepNumber = 0;
//int curSpeed = 7000;
int dir = 0;
//int maxSpeed = 10000;
int accel = 1000;

long savedPosA = 0;
long savedPosB = 0;

IRrecv My_Receiver(remotePin);//Receive on pin remotePin
IRdecode My_Decoder; 
 

//float fMaxSpeed = 10000.0;
float speedSlow = 300.0;
float speedFast = 400.0;
float speedMax = 1000.0;
float speedCurr = speedFast;
float fStepsPerSecond = speedCurr;

void setup() 
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  ///// ARTURO: Start the remote

  My_Receiver.enableIRIn(); // Start the receiver
  
  if (printDebug)
  {
    // Initialize the Serial port
    Serial.begin(9600);
  }

  // blink the LED:
  blink(2);
  
  stepper1.setMaxSpeed(10000.0);
  stepper1.setAcceleration(accel);
}


void loop() 

{  
  // First, we need to see if either rotate button is down. They always take precidence

  if (My_Receiver.GetResults(&My_Decoder)) {
       My_Decoder.decode();
       //My_Decoder.DumpResults();
       My_Receiver.resume();
  }

  if(My_Decoder.value==speedToggle)
  {
    if (fStepsPerSecond==speedSlow){
        fStepsPerSecond = speedFast;
       }
    else {
        fStepsPerSecond = speedSlow;}
        
    if (stepper1.speed() < 0) // to keep correct direction
      {stepper1.setSpeed(-fStepsPerSecond);}
      else {stepper1.setSpeed(fStepsPerSecond);}
    Serial.print("Using speed:");
    Serial.println(fStepsPerSecond);;
    }


  if(My_Decoder.value==speedDown_13)
  {
    if (fStepsPerSecond < 100.0)
      {      
        fStepsPerSecond = fStepsPerSecond - 50.0;
        if (stepper1.speed() < 0) // to keep correct direction
            {stepper1.setSpeed(-fStepsPerSecond);}
        else {stepper1.setSpeed(fStepsPerSecond);}
      }
    Serial.print("Using speed:");
    Serial.println(fStepsPerSecond);
    My_Receiver.resume();
    }
   
    
  if(My_Decoder.value==speedUp_13)
  {
    if (fStepsPerSecond < speedMax)
      {      
        fStepsPerSecond = fStepsPerSecond + 50.0;
        if (stepper1.speed() < 0) // to keep correct direction
            {stepper1.setSpeed(-fStepsPerSecond);}
        else {stepper1.setSpeed(fStepsPerSecond);}
      }
    Serial.print("Using speed:");
    Serial.println(fStepsPerSecond);;
    My_Receiver.resume();
    }



  if(My_Decoder.value==rotateLeft || My_Decoder.value==rotateLeft_13)
  {
    stepper1.setSpeed(-fStepsPerSecond);
    stepper1.runSpeed();
    My_Receiver.resume();
    //Serial.print("Using speed:");
    //Serial.println(fStepsPerSecond);
    }
  

  else if (My_Decoder.value==rotateRight || My_Decoder.value==rotateRight_13)
  {
    stepper1.setSpeed(fStepsPerSecond);
    stepper1.runSpeed();
    My_Receiver.resume();
    //Serial.print("Using speed:");
    //Serial.println(fStepsPerSecond);  
  }
  

  if(My_Decoder.value==savePositionA || My_Decoder.value==savePositionA_13)
  {
    savedPosA = stepper1.currentPosition();
    if (printDebug)
    {
      Serial.print("Saved A at :");
      Serial.println(savedPosA);
    }
  }

  if(My_Decoder.value==savePositionB || My_Decoder.value==savePositionB_13)
  {
    savedPosB = stepper1.currentPosition();
    if (printDebug)
    {
      Serial.print("Saved B at :");
      Serial.println(savedPosB);
    }
  }

  // Check to see if the user wants to go to position A or B
  if(My_Decoder.value==gotoPositionA || My_Decoder.value==gotoPositionA_13)
  {
    if (printDebug)
    {
      // Yup, let's go to position A
      Serial.print("cur pos = ");
      Serial.println(stepper1.currentPosition());
      Serial.print("Going to A = ");
      Serial.println(savedPosA);
      Serial.print("Speed = ");
      Serial.println(stepper1.speed());
      Serial.print("Accel = ");
      Serial.println(accel);
    }

    stepper1.setAcceleration(0);
    stepper1.runToNewPosition(stepper1.currentPosition());
    stepper1.setSpeed(fStepsPerSecond);
    //stepper1.moveTo(savedPosA);
    //stepper1.setMaxSpeed(fStepsPerSecond);
    //stepper1.runSpeedToPosition();
    stepper1.setAcceleration(accel);
    stepper1.runToNewPosition(savedPosA);
    
    if (printDebug)
    {
      Serial.print("new pos = ");
      Serial.println(stepper1.currentPosition());
    }
  }

  else if (My_Decoder.value==gotoPositionB || My_Decoder.value==gotoPositionB_13)
  {
    // Yup, let's go to position B
    if (printDebug)
    {
      Serial.print("cur pos = ");
      Serial.println(stepper1.currentPosition());
      Serial.print("Going to B = ");
      Serial.println(savedPosB);
      Serial.print("Speed = ");
      Serial.println(stepper1.speed());
      Serial.print("Accel = ");
      Serial.println(accel);
    }

    stepper1.setAcceleration(0);
    stepper1.runToNewPosition(stepper1.currentPosition());
    stepper1.setSpeed(fStepsPerSecond);
    stepper1.setAcceleration(accel);
    stepper1.runToNewPosition(savedPosB);
    //stepper1.moveTo(savedPosB);
    //stepper1.setMaxSpeed(fStepsPerSecond);
    //stepper1.runSpeedToPosition();
    

    if (printDebug)
    {
      Serial.print("new pos = ");
      Serial.println(stepper1.currentPosition());
    }
  }
}



// Blink the reset LED:

void blink(int howManyTimes) 
{
  int i;
  for (i=0; i < howManyTimes; i++) 
  {
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin, LOW);
    delay(200);
    delay(200);
  }
}


