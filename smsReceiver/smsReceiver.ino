/*
 This project allows you to control a relay using SMS messaging.
 Required hardware is Arduino UNO/Leonardo, Arduino GSM shield, and a relay shield by Evil Mad Science.
 Relay shield is driven through digital pin 4, which is not used by the GSM shield.
 
 
 Based on GSM examples created
 by Javier Zorzano / TD and Tom Igoe
  
 http://arduino.cc/en/Tutorial/GSMExamplesReceiveSMS
 http://arduino.cc/en/Tutorial/GSMExamplesSendSMS
 
*/

// include the GSM library
#include <GSM.h>

//Commands
#define STATUS 0
#define TURN_ON 1
#define TURN_OFF 2
#define UNKNOWN_COMMAND -1;

const String TURN_ON_COMMAND = "TURN ON";
const String TURN_OFF_COMMAND = "TURN OFF";
const String STATUS_COMMAND = "STATUS";

// PIN Number for the SIM
#define PINNUMBER ""

// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;

int relayPin=4;
int delayValue = 3000;

void setup() 
{
  //set up relay pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin,LOW);
  
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  } 
  Serial.println("SMS Messages Receiver");
    
  // connection state
  boolean notConnected = true;
  
  // Start GSM connection
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  
  Serial.println("GSM initialized");
  Serial.println("Waiting for messages");
}

void loop() 
{
  char c;
  
  // If there are any SMSs available()  
  if (sms.available())
  {
    Serial.println("Message received from:");  
    
    char senderNumber[20];
    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);
    
    // Read message bytes and print them
    String s = "";
    while(c=sms.read()){
      s = s + c;
      Serial.print(c);
    }
    Serial.println("\nEND OF MESSAGE");
    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
    
    parseSMS(s,senderNumber);  
    
    s = "";
    
  }

  delay(1000);

}

void parseSMS(String sms, char senderNumber[])
{
  int command = getCommand(sms);
  execute(command, senderNumber);
}

int getCommand(String sms)
{
    int returnCommandCode = UNKNOWN_COMMAND; //Unknown command code
    if(sms.startsWith(TURN_ON_COMMAND))
    {
      returnCommandCode = TURN_ON;  
    }else if(sms.startsWith(TURN_OFF_COMMAND))
    {
      returnCommandCode = TURN_OFF;
    }else if(sms.startsWith(STATUS_COMMAND))
    {
      returnCommandCode = STATUS;
    }
    return returnCommandCode;
}

void execute(int command, char senderNumber[])
{
   switch(command)
   {
     case TURN_ON:
     {
       int state = switchOn();
       char message[20];
       String stateString = getStateAsString(state);
       stateString.toCharArray(message,20);
       sendReply(senderNumber,message);
       break;
     }
     case TURN_OFF:
     {
       int state = switchOff();
       char message[20];
       String stateString = getStateAsString(state);
       stateString.toCharArray(message,20);
       sendReply(senderNumber,message);
       break;
     }
     case STATUS:
     {
       char message[20];
       int state = digitalRead(relayPin);
       String stateString = getStateAsString(state);
       stateString.toCharArray(message,20);
       sendReply(senderNumber,message);
       break;
     }
   }
}

void sendReply(char remoteNum[], char textMessage[])
{
  Serial.println(remoteNum);    
  // sms text
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(textMessage);
  
  // send the message
  sms.beginSMS(remoteNum);
  sms.print(textMessage);
  sms.endSMS();
  sms.flush(); 
  Serial.println("\nCOMPLETE!\n");
}

int switchOn()
{
 digitalWrite(relayPin, HIGH);   
 int state = digitalRead(relayPin);
 logState(state);
 delay(1000);
 return state;
}

int switchOff()
{
  digitalWrite(relayPin, LOW);    // turn the LED off by making the voltage LOW    
  int state = digitalRead(relayPin);
  logState(state);          
  delay(1000);
  return state;  
}

void logState(int state)
{
 Serial.println(state);
 if (state == HIGH)
 {
   Serial.println("Heat On");
 }
 else
 {
   Serial.println("Heat Off");
 }
}


String getStateAsString(int state)
{
  String returnArray = String("");
  if (state == HIGH)
  {
    returnArray = "Relay state:ON";
  }  
  else
  {
    returnArray = "Relay state:OFF";
  }
  return returnArray; 
}



