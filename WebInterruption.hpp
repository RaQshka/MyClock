#include "HardwareSerial.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include "index.h"
#include "EEPROMManipulation.hpp"


AsyncWebServer server(80);
IPAddress address;

String serverString = "";


void handleWebPages(){
  MainNotes.InitNotes();
  Serial.println("Handling!!!");
    address = WiFi.localIP();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      Serial.println("Web Server: home page");
      String html = HTML_CONTENT_HOME;  // Use the HTML content from the index.h file
      html.replace("%STRING_STATE%", alarmString);  // update the LED state
      //html.replace("%REPLACE-FROM-EEPROM%", MainNotes.GetHtmlNotes());  
      request->send(200, "text/html", html);
  });
  Serial.println("first handler installed");
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Serial.print("Web Server: String page");
        // Check for the 'state' parameter in the query string
        if (request->hasArg("hour")&&request->hasArg("min")&&request->hasArg("sec")) {
            alarmString = String(request->arg("hour")) + ":"+ String(request->arg("min"))+":"+String(request->arg("sec"));
        }       
        if (request->hasArg("rollingString")) {
            rollingString = String(request->arg("rollingString"));
            if(rollingString !="")
              MainNotes.WriteNoteToMemory(rollingString);
        }
        if (request->hasArg("scrollDelay")) {
            ScrollSpeed = request->arg("scrollDelay").toInt();
        }        
        if (request->hasArg("scrollAll")) {
            String state = request->arg("scrollAll");
            Serial.println(state);
            if(state == "true"){
              IsNotesQueue = true;
            }else if(state == "false"){
              IsNotesQueue = false;
            }
        }

        Serial.println();
        Serial.println(alarmString);

        String html = HTML_CONTENT_HOME;                   
        html.replace("%STRING_STATE%", alarmString);  
        //html.replace("%REPLACE-FROM-EEPROM%", MainNotes.GetHtmlNotes());  
        request->send(200, "text/html", html);
    });

  Serial.println("second handler installed");
  
  server.on("/getNotes", HTTP_GET, [](AsyncWebServerRequest *request) {
      Serial.println("Web Server: api");
      String html = MainNotes.GetHtmlNotes();  // Use the HTML content from the index.h file
      //html.replace("%REPLACE-FROM-EEPROM%", MainNotes.GetHtmlNotes());  
      request->send(200, "text/html", html);
  }); 
     
     
            server.begin();
            Serial.println("HTTP server started");
}

void WifiInit(){
   // Initialize Serial Monitor
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
