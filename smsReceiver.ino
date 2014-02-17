/*
 This project allows you to control a relay using SMS messaging.
 Required hardware is Arduino UNO/Leonardo, Arduino GSM shield, and a relay.
 
 
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

// Array to hold the number a SMS is retreived from
//char senderNumber[20];
int relayPin=13;
int delayValue = 3000;



void setup() 
{
  //set up relay pin
  pinMode(relayPin, OUTPUT);
  
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

    // An example of message disposal    
    // Any messages starting with # should be discarded
    if(sms.peek()=='#')
    {
      Serial.println("Discarded SMS");
      sms.flush();
    }
    
    
    // Read message bytes and print them
    String s = "";
    while(c=sms.read()){
      s = s + c;
      Serial.print(c);
    }
    
    char senderNumber[20];
    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);
    
    parseSMS(s,senderNumber);  
    Serial.println("\nEND OF MESSAGE");
    
    // Delete message from modem memory
    sms.flush();
    s = "";
    Serial.println("MESSAGE DELETED");
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
       switchOn();
       sendReply(senderNumber,"");
       break;
     }
     case TURN_OFF:
     {
       switchOff();
       sendReply(senderNumber,"");
       break;
     }
     case STATUS:
     {
       
       sendReply(senderNumber,"");
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
  Serial.println("\nCOMPLETE!\n");
}




void switchOn()
{
  digitalWrite(relayPin,HIGH);
  delay(delayValue);
  digitalWrite(relayPin,LOW);
  delay(delayValue);
}

void switchOff()
{
  digitalWrite(relayPin,LOW);
  delay(delayValue);
  digitalWrite(relayPin,LOW);
  delay(delayValue);
}

