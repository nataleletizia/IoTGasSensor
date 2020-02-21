#include <SoftwareSerial.h>
#define rxPin 2
#define txPin 3
#define redLedPin 9
#define greenLedPin 8
#define buzzerPin 10
#define smokeSensorPin A0
#define DEBUG true

SoftwareSerial wifiModule(rxPin,txPin); // Connect TX pin of esp to the pin 2 of Arduino and RX pin of esp to the pin 3 of Arduino


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

  esp8266Command("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point

  esp8266Command("AT+CIFSR\r\n",1000,DEBUG); // get ip address

  esp8266Command("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections

  esp8266Command("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}

void loop()

{

  int smokeSensor = analogRead(smokeSensorPin);

  if (smokeSensor > 400)

  {

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


  if(wifiModule.available()) 

  {
    if(wifiModule.find("+IPD,"))

    {

     delay(1000);

     int connectionId = wifiModule.read()-48;     
     
     String webPage = "<html><head><meta http-equiv='refresh' content='5' /></head><body>";
      webPage += "<h1>IOT Smoke Detection System</h1>";

      webPage +="<p>Smoke Value is ";

      webPage += "<div id=\"smokeLevel\">"+String(smokeSensor)+"</div>";

      webPage +="</p>";

      if (smokeSensor > 400)

  {

    webPage +="<h5>DANGER! Move Somewhere Else</h5>";

  }

  else

  {

    webPage +="<h4>Everything Normal</h4>";

  }
     webPage+="</body></html>";
     
     String cipSend = "AT+CIPSEND=";

     cipSend += connectionId;

     cipSend += ",";

     cipSend +=webPage.length();

     cipSend +="\r\n";

     

     esp8266Command(cipSend,1000,DEBUG);

     esp8266Command(webPage,1000,DEBUG);

     

     String closeCommand = "AT+CIPCLOSE="; 

     closeCommand+=connectionId; // append connection id

     closeCommand+="\r\n";

     

     esp8266Command(closeCommand,3000,DEBUG);

    }

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
