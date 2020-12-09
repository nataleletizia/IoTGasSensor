#include <SoftwareSerial.h>
#define rxPin 2
#define txPin 3
#define redLedPin 9
#define greenLedPin 8
#define buzzerPin 10
#define smokeSensorPin A0
#define smokeLevel 400
#define DEBUG true

String ssid = "";
String apPassword = "";
String apikeyWrite = "";

SoftwareSerial wifiModule(rxPin,txPin); // Connect TX pin of esp to the pin 2 of Arduino and RX pin of esp to the pin 3 of Arduino


unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long interval = 26000;

String host = "api.thingspeak.com"; // cloud API host name
String port = "80";


void setup()

{

  Serial.begin(9600);
  while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

  wifiModule.begin(9600); // Set the baudrate according to your esp8266

  pinMode(redLedPin, OUTPUT);

  pinMode(greenLedPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);

  pinMode(smokeSensorPin, INPUT);

  esp8266Command("AT+RST\r\n",2000,DEBUG); // reset module

  esp8266Command("AT+CWMODE=1\r\n",1000,DEBUG); // configure as station

  esp8266Command("AT+CWJAP=\""+ssid+"\",\""+apPassword+"\"\r\n",1000,DEBUG);
  
  delay(4000);

  startMillis = millis();
  
}
  

void loop()

{

  int smokeSensor = analogRead(smokeSensorPin);

    if (smokeSensor > smokeLevel)

  {

    Serial.println("Smoke level above the threshold! "+smokeSensor);
    
    digitalWrite(redLedPin, HIGH);

    digitalWrite(greenLedPin, LOW);

    tone(buzzerPin, 1000, 200);

  }

  else

  {

    digitalWrite(redLedPin, LOW);

    digitalWrite(greenLedPin, HIGH);

    noTone(buzzerPin);

  }

currentMillis = millis(); 

if (currentMillis - startMillis >= interval)  //test whether the period has elapsed
  {
  String url = "GET /update?api_key="+ apikeyWrite+"&field1="+String(smokeSensor);

  Serial.print("requesting URL: ");
  Serial.println(url);

  String requestLen = String(url.length() + 4);

  Serial.println("Open TCP connection");

  esp8266Command("AT+CIPMUX=1\r\n",10000,DEBUG); // configure for multiple connections

  esp8266Command("AT+CIPSTART=0,\"TCP\",\"" + host +"\"," + port+"\r\n", 20000, DEBUG);

  esp8266Command("AT+CIPSEND=0,"+requestLen+"\r\n",10000,DEBUG);

  wifiModule.println(url);

  esp8266Command("AT+CIPCLOSE=0\r\n",5000,DEBUG);
  startMillis = currentMillis;
  }

}

String esp8266Command(String command, const int timeout, boolean debug)

{

    String response = "";

    wifiModule.print(command); 

    long int time = millis();

    while( (time+timeout) > millis())

    {

      while(wifiModule.available())

      {

        

        char c = wifiModule.read(); 

        response+=c;

      }  

    }

    

    if(debug)

    {

      Serial.print(response);

    }

    return response;

}
