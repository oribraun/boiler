/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);

#define BOILER_ON_STATE 1
#define BOILER_OFF_STATE 0

static int BOILER_PIN = 5;

boolean haveUserDetails = false;

const char* ssid     = "";
const char* ssidPassword = "";
//const char* ssid     = "ben-moshe";
//const char* ssidPassword = "ybenmosh";

const char* host = "boiler.braunfire.com";
String url = "/api/ajax_check_user_boiler_state";
String PostData = "email=support@braun.com&password=braun123";
const int httpPort = 80;

String hostEmail = "";
String hostPassword = "";
String errorMessage = "";

//ArduinoJson::JsonObject
JsonObject& getJson(String line) {
  line.trim();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& jsonParsed = jsonBuffer.parseObject(line);
  return jsonParsed;
}

//login page, also called for disconnect
void handleUserDetails(){
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
//  if (server.hasArg("DISCONNECT")){
//    Serial.println("Disconnection");
//    server.sendHeader("Location","/login");
//    server.sendHeader("Cache-Control","no-cache");
//    server.sendHeader("Set-Cookie","ESPSESSIONID=0");
//    server.send(301);
//    return;
//  }
  if (server.hasArg("EMAIL") && server.hasArg("PASSWORD") && server.hasArg("SSID") && server.hasArg("SSID_PASSWORD")){
    hostEmail     = server.arg("EMAIL");
    hostPassword  = server.arg("PASSWORD");
    char * cssid = new char [server.arg("SSID").length()+1];
    strcpy (cssid, server.arg("SSID").c_str());
    ssid          = cssid;
    char * cssidPassword = new char [server.arg("SSID_PASSWORD").length()+1];
    strcpy (cssidPassword, server.arg("SSID_PASSWORD").c_str());
    ssidPassword          = cssidPassword;
    Serial.print("current ssid is ");
    Serial.println(ssid);
    Serial.print("current ssid password is ");
    Serial.println(ssidPassword);
    WiFi.begin(ssid, ssidPassword);
    Serial.println("");
    Serial.print("WiFi Connected to ");
    Serial.println(ssid);  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    haveUserDetails = true;
    errorMessage = "Success";
    PostData = "email=" + hostEmail + "&password=" + hostPassword;
    Serial.println(PostData);
    handleRoot();
//    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ){
//      server.sendHeader("Location","/");
//      server.sendHeader("Cache-Control","no-cache");
//      server.sendHeader("Set-Cookie","ESPSESSIONID=1");
//      server.send(301);
//      Serial.println("Log in Successful");
//      return;
//    }
//  msg = "Wrong username/password! try again.";
//  Serial.println("Log in Failed");
  } else {
    errorMessage = "Username or Password or ssid or ssid password Is Empty";
    handleRoot();
  }
}

void handleRoot() {
  Serial.print("handle root access");
  String content = "<html><body><form action='/user_details' method='POST'>please enter user boiler email and password from <a target='_blank' href='http://boiler.braunfire.com'>Boiler Site</a><br>";
  content += "SSID:<input type='text' name='SSID' placeholder='ssid'><br>";
  content += "SSID Password:<input type='text' name='SSID_PASSWORD' placeholder='ssid password'><br>";
  content += "Email:<input type='text' name='EMAIL' placeholder='email'><br>";
  content += "Password:<input type='text' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + errorMessage + "<br>";
  content += "You also can go <a href='/example'>here</a></body></html>";
  server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, ssidPassword);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi Connected to ");
  Serial.println(ssid);  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/user_details", handleUserDetails);
  server.on("/example", [](){
    server.send(200, "text/plain", "example");
  });
  server.onNotFound(handleNotFound);
  
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  if(haveUserDetails) {
      delay(1000);
      Serial.println("have user details");
    
      Serial.print("connecting to ");
      Serial.println(host);
      
      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      
      // We now create a URI for the request
      Serial.print("Requesting URL: ");
      Serial.println(url);
      
      // This will send the request to the server
//      client.println("POST " + url + " HTTP/1.1");
//      client.println("Host: " + String(host));
//      client.println("Cache-Control: no-cache");
//      client.println("Content-Type: application/x-www-form-urlencoded");
//      client.print("Content-Length: ");
//      client.println(PostData.length());
//      client.println(PostData);
      client.print(String("GET ") + url + "?" + PostData + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
//                   "Content-Type: application/x-www-form-urlencoded");
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
      }
      
      // Read all the lines of the reply from server and print them to Serial
      while(client.available()){
//        String json = "";
        boolean httpBody = false;
        String line = client.readStringUntil('\r');
        if (!httpBody && line.charAt(1) == '{') {
          httpBody = true;
        }
        if(httpBody) {
//          json += line;
          Serial.print("line - ");
//          json.trim();
          Serial.println(line);
//          StaticJsonBuffer<200> jsonBuffer;
//          JsonObject& jsonParser = jsonBuffer.parseObject(line);
          JsonObject& jsonParser = getJson(line);
          if (!jsonParser.success()) {
            Serial.println("Parsing failed");
            return;
          }
          int boilerState = jsonParser["boiler_state"];
          Serial.print("boilerState - ");
          Serial.println(boilerState);
          if(boilerState == BOILER_ON_STATE) {
            Serial.println("HIGH");
            digitalWrite(BOILER_PIN, HIGH); 
          } else {                
            Serial.println("LOW");
            digitalWrite(BOILER_PIN, LOW);  
          }
        }
      }
      
      Serial.println();
      Serial.println("closing connection");
  } else {
    Serial.println("no user details");
    delay(1000);
  }
}

