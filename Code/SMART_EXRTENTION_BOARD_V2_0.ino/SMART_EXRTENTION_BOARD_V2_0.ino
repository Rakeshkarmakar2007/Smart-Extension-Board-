#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <IRrecv.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <IRutils.h>
#include <WebServer.h>
#include <ESP32Ping.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
int attempt = 0; 

WebServer server(80);
DNSServer dnsServer;
RTC_DS1307 rtc;

const char *ssid = "SMART EXTENTION BOARD V2.0";

const int ssidAddr = 1;
const int passAddr = 50;
const int ipaddr = 100;
const int wifiStatus = 134;
const int l1 = 135, l2 = 136, l3 = 137, l4 = 138, sliderAdd = 139;
const int ir1add = 140, ir2add = 150, ir3add = 160, ir4add = 170, ir5add = 180, ir6add = 190, ir7add = 200, ir8add = 210;
const int sw1add = 220, sw2add = 221, sw3add = 222, sw4add = 223, coadd = 224, Reg = 225;

const int day1onadd = 227, day2onadd = 228, day3onadd = 229, day4onadd = 230;
const int day1offadd = 231, day2offadd = 232, day3offadd = 233, day4offadd = 234;

const int toggle1onadd = 235, toggle2onadd = 236, toggle3onadd = 237, toggle4onadd = 238;
const int toggle1offadd = 239, toggle2offadd = 240, toggle3offadd = 241, toggle4offadd = 242;

const int time1onadd = 245, time2onadd = 265, time3onadd = 285, time4onadd = 305;
const int time1offadd = 325, time2offadd = 345, time3offadd = 365, time4offadd = 385, flagadd = 405;


String SSID;
String PASS;
String IP;
String value;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "in.pool.ntp.org", 19800);
const IPAddress remote_ip(8, 8, 8, 8);

bool preSw1State, preSw2State, preSw3State, preSw4State, flag = false, flagon1=true, flagon2=true, flagon3=true, flagon4=true, flagoff1=true, flagoff2=true, flagoff3=true, flagoff4=true;
int preRegVal;
bool pin1status, pin2status, pin3status, pin4status, condition, var;

#define pin1 4
#define pin2 16
#define pin3 17
#define pin4 18
#define IR_PIN 19
#define pwm 33
#define Net 13
#define sw1 23
#define sw2 25
#define sw3 26
#define sw4 27
#define config 32

int sliderValue;
int wifi = 0, k = 0;
bool AP = true;
uint64_t ir1, ir2, ir3, ir4, ir5, ir6, ir7, ir8;
int add = 0;

int socket4DayOn, socket4DayOff;
String socket4TimeOn, socket4TimeOff;
bool socket4ToggleOn, socket4ToggleOff;

int socket3DayOn, socket3DayOff;
String socket3TimeOn, socket3TimeOff;
bool socket3ToggleOn, socket3ToggleOff;

int socket2DayOn, socket2DayOff;
String socket2TimeOn, socket2TimeOff;
bool socket2ToggleOn, socket2ToggleOff;

int socket1DayOn, socket1DayOff;
String socket1TimeOn, socket1TimeOff;
bool socket1ToggleOn, socket1ToggleOff;

String day[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Everyday" };

bool connected_to_blynk =  false;
int c=0;

IRrecv irrecv(IR_PIN);
decode_results results;

void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(Net, OUTPUT);
  pinMode(pwm, OUTPUT);

  pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);
  pinMode(sw3, INPUT_PULLUP);
  pinMode(sw4, INPUT_PULLUP);
  pinMode(config, INPUT_PULLUP);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  Serial.begin(112500);
  irrecv.enableIRIn();
  digitalWrite(Net, LOW);
  readDetails();
  analogWrite(pwm, sliderValue);
  sw();
  value = String((sliderValue / 255.0) * 100);
  if ((wifi == 1) && (condition == 1)) {
    AP = false;
    Serial.println("Connecting to WiFi...");
    Serial.println(SSID);
    Serial.print("Password: ");
    Serial.println(PASS);
    WiFi.begin(SSID.c_str(), PASS.c_str());
    while (WiFi.status() != WL_CONNECTED) {

      if ((digitalRead(sw1) != preSw1State) || (digitalRead(sw2) != preSw2State) || (digitalRead(sw3) != preSw3State) || (digitalRead(sw4) != preSw4State) || (int(((analogRead(A0) / 100) / 40.0) * 255) != preRegVal)) {
        sw();
      }
      if(flag){
        digitalWrite(Net, HIGH);
        delay(200);
        digitalWrite(Net, LOW);
        delay(200);
      }

      if (digitalRead(config) == 0) {
        digitalWrite(Net, LOW);
        delay(200);
        digitalWrite(Net, HIGH);
        delay(200);
        digitalWrite(Net, LOW);
        AP = true;
        putcondition(false, false);
        ESP.restart();
      }

      if (irrecv.decode(&results)) {
        remote();
        irrecv.resume();
      }
      if (!pin1status) {
        digitalWrite(pin1, LOW);
      } else {
        digitalWrite(pin1, HIGH);
      }
      if (!pin2status) {
        digitalWrite(pin2, LOW);
      } else {
        digitalWrite(pin2, HIGH);
      }
      if (!pin3status) {
        digitalWrite(pin3, LOW);
      } else {
        digitalWrite(pin3, HIGH);
      }
      if (!pin4status) {
        digitalWrite(pin4, LOW);
      } else {
        digitalWrite(pin4, HIGH);
      }
      if (socket1ToggleOn || socket1ToggleOff || socket2ToggleOn || socket2ToggleOff || socket3ToggleOn || socket3ToggleOff || socket4ToggleOn || socket4ToggleOff) {
        if (k == 1000) {
          timer();
          k = 0;
        }
        k++;
      }
      DateTime now = rtc.now();
      if((now.hour() == 0 && now.minute() == 1) || (now.hour() == 6 && now.minute() == 1) || (now.hour() == 12 && now.minute() == 1) || (now.hour() == 18 && now.minute() == 1) ){
        ESP.restart();
      }
      delay(10);
    }
    


    IPAddress ip = WiFi.localIP();
    IP = ip.toString();
    Serial.println("\nConnected to WiFi");
    writeIP(IP);
    Serial.print("IP Address: ");
    Serial.println(IP);
    
    digitalWrite(Net, HIGH);
    if(flag){
      flag = false;
      putcondition(false, false);
      AP = true;
      ESP.restart();
    }
  }
  if (AP) {
    WiFi.softAP(ssid);
    WiFi.scanNetworks(true, true);
  }
  server.begin();
  dnsServer.start(53, "*", WiFi.softAPIP());


  server.on("/", handleRoot);
  server.on("/credentials", handledetails);
  server.on("/l1on", hand1on);
  server.on("/l1off", hand1off);
  server.on("/l2on", hand2on);
  server.on("/l2off", hand2off);
  server.on("/l3on", hand3on);
  server.on("/l3off", hand3off);
  server.on("/l4on", hand4on);
  server.on("/l4off", hand4off);
  server.on("/IP", handleIP);
  server.on("/factory", handlereset);
  server.on("/IR", handleremote);
  server.on("/send-value", handleSendValue);
  server.on("/Timer", handleTimer);
  server.on("/set-alarm", HTTP_POST, handleData);
  server.on("/WiFi", handleWiFi);
  server.on("/RTC", handleRTC);
  server.on("/close", Close);
  server.onNotFound(handleNotFound);


  server.begin();
  timeClient.begin();
}

void Close(){
  putcondition(true, false);
  ESP.restart();
}

void ledState() {
  EEPROM.begin(512);
  EEPROM.write(l1, pin1status);
  EEPROM.write(l2, pin2status);
  EEPROM.write(l3, pin3status);
  EEPROM.write(l4, pin4status);
  EEPROM.write(Reg, preRegVal);
  EEPROM.write(sw1add, preSw1State);
  EEPROM.write(sw2add, preSw2State);
  EEPROM.write(sw3add, preSw3State);
  EEPROM.write(sw4add, preSw4State);
  EEPROM.write(sliderAdd, sliderValue);
  EEPROM.commit();
  EEPROM.end();
if(connected_to_blynk){
  if(pin1status){
    Blynk.virtualWrite(1, HIGH);
  }else{
    Blynk.virtualWrite(1, LOW);
  }

  if(pin2status){
    Blynk.virtualWrite(2, HIGH);
  }else{
    Blynk.virtualWrite(2, LOW);
  }

  if(pin3status){
    Blynk.virtualWrite(3, HIGH);
  }else{
    Blynk.virtualWrite(3, LOW);
  }

  if(pin4status){
    Blynk.virtualWrite(4, HIGH);
  }else{
    Blynk.virtualWrite(4, LOW);
  }
}
  
}

void saveAlarm() {
  EEPROM.begin(512);
  EEPROM.write(day1onadd, socket1DayOn);
  EEPROM.write(day2onadd, socket2DayOn);
  EEPROM.write(day3onadd, socket3DayOn);
  EEPROM.write(day4onadd, socket4DayOn);

  for (int i = 0; i < socket1TimeOn.length(); ++i) {
    EEPROM.write(time1onadd + i, socket1TimeOn[i]);
  }
  EEPROM.write(time1onadd + socket1TimeOn.length(), '\0');
  for (int i = 0; i < socket2TimeOn.length(); ++i) {
    EEPROM.write(time2onadd + i, socket2TimeOn[i]);
  }
  EEPROM.write(time2onadd + socket2TimeOn.length(), '\0');
  for (int i = 0; i < socket3TimeOn.length(); ++i) {
    EEPROM.write(time3onadd + i, socket3TimeOn[i]);
  }
  EEPROM.write(time3onadd + socket3TimeOn.length(), '\0');
  for (int i = 0; i < socket4TimeOn.length(); ++i) {
    EEPROM.write(time4onadd + i, socket4TimeOn[i]);
  }
  EEPROM.write(time4onadd + socket4TimeOn.length(), '\0');


  EEPROM.write(toggle1onadd, socket1ToggleOn);
  EEPROM.write(toggle2onadd, socket2ToggleOn);
  EEPROM.write(toggle3onadd, socket3ToggleOn);
  EEPROM.write(toggle4onadd, socket4ToggleOn);

  EEPROM.write(day1offadd, socket1DayOff);
  EEPROM.write(day2offadd, socket2DayOff);
  EEPROM.write(day3offadd, socket3DayOff);
  EEPROM.write(day4offadd, socket4DayOff);

  for (int i = 0; i < socket1TimeOff.length(); ++i) {
    EEPROM.write(time1offadd + i, socket1TimeOff[i]);
  }
  EEPROM.write(time1offadd + socket1TimeOff.length(), '\0');
  for (int i = 0; i < socket2TimeOff.length(); ++i) {
    EEPROM.write(time2offadd + i, socket2TimeOff[i]);
  }
  EEPROM.write(time2offadd + socket2TimeOff.length(), '\0');
  for (int i = 0; i < socket3TimeOff.length(); ++i) {
    EEPROM.write(time3offadd + i, socket3TimeOff[i]);
  }
  EEPROM.write(time3offadd + socket3TimeOff.length(), '\0');
  for (int i = 0; i < socket4TimeOff.length(); ++i) {
    EEPROM.write(time4offadd + i, socket4TimeOff[i]);
  }
  EEPROM.write(time4offadd + socket4TimeOff.length(), '\0');

  EEPROM.write(toggle1offadd, socket1ToggleOff);
  EEPROM.write(toggle2offadd, socket2ToggleOff);
  EEPROM.write(toggle3offadd, socket3ToggleOff);
  EEPROM.write(toggle4offadd, socket4ToggleOff);

  EEPROM.commit();
  EEPROM.end();
}

void saveDetails(String ssid, String password) {
  EEPROM.begin(512);
  for (int i = 0; i < ssid.length(); ++i) {
    EEPROM.write(ssidAddr + i, ssid[i]);
  }
  EEPROM.write(ssidAddr + ssid.length(), '\0');

  for (int i = 0; i < password.length(); ++i) {
    EEPROM.write(passAddr + i, password[i]);
  }
  EEPROM.write(passAddr + password.length(), '\0');
  EEPROM.write(wifiStatus, 1);
  EEPROM.commit();
  EEPROM.end();
}

void putcondition(bool co, bool flag_val) {
  EEPROM.begin(512);
  EEPROM.write(coadd, co);
  EEPROM.write(flagadd, flag_val);
  EEPROM.commit();
  EEPROM.end();
}

void readDetails() {
  EEPROM.begin(512);
  SSID = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(ssidAddr + i);
    if (c == '\0')
      break;
    SSID += c;
  }

  PASS = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(passAddr + i);
    if (c == '\0')
      break;
    PASS += c;
  }

  IP = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(ipaddr + i);
    if (c == '\0')
      break;
    IP += c;
  }
  condition = EEPROM.read(coadd);
  flag = EEPROM.read(flagadd);
  pin1status = EEPROM.read(l1);
  pin2status = EEPROM.read(l2);
  pin3status = EEPROM.read(l3);
  pin4status = EEPROM.read(l4);
  wifi = EEPROM.read(wifiStatus);
  sliderValue = 0;


  for (int i = 0; i < sizeof(ir1); ++i) {
    ir1 |= (uint64_t)EEPROM.read(ir1add + i) << (i * 8);
  }
  for (int i = 0; i < sizeof(ir2); ++i) {
    ir2 |= (uint64_t)EEPROM.read(ir2add + i) << (i * 8);
  }
  for (int i = 0; i < sizeof(ir3); ++i) {
    ir3 |= (uint64_t)EEPROM.read(ir3add + i) << (i * 8);
  }
  for (int i = 0; i < sizeof(ir4); ++i) {
    ir4 |= (uint64_t)EEPROM.read(ir4add + i) << (i * 8);
  }
  for (int i = 0; i < sizeof(ir5); ++i) {
    ir5 |= (uint64_t)EEPROM.read(ir5add + i) << (i * 8);
  }
  for (int i = 0; i < sizeof(ir6); ++i) {
    ir6 |= (uint64_t)EEPROM.read(ir6add + i) << (i * 8);
  }
  for (int i = 0; i < sizeof(ir7); ++i) {
    ir7 |= (uint64_t)EEPROM.read(ir7add + i) << (i * 8);
  }
  for (int i = 0; i < sizeof(ir8); ++i) {
    ir8 |= (uint64_t)EEPROM.read(ir8add + i) << (i * 8);
  }


  preRegVal = EEPROM.read(Reg);
  preSw1State = EEPROM.read(sw1add);
  preSw2State = EEPROM.read(sw2add);
  preSw3State = EEPROM.read(sw3add);
  preSw4State = EEPROM.read(sw4add);
  sliderValue = EEPROM.read(sliderAdd);

  socket1DayOn = EEPROM.read(day1onadd);
  socket2DayOn = EEPROM.read(day2onadd);
  socket3DayOn = EEPROM.read(day3onadd);
  socket4DayOn = EEPROM.read(day4onadd);

  socket1TimeOn = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(time1onadd + i);
    if (c == '\0')
      break;
    socket1TimeOn += c;
  }
  socket2TimeOn = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(time2onadd + i);
    if (c == '\0')
      break;
    socket2TimeOn += c;
  }
  socket3TimeOn = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(time3onadd + i);
    if (c == '\0')
      break;
    socket3TimeOn += c;
  }
  socket4TimeOn = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(time4onadd + i);
    if (c == '\0')
      break;
    socket4TimeOn += c;
  }

  socket1ToggleOn = EEPROM.read(toggle1onadd);
  socket2ToggleOn = EEPROM.read(toggle2onadd);
  socket3ToggleOn = EEPROM.read(toggle3onadd);
  socket4ToggleOn = EEPROM.read(toggle4onadd);


  socket1DayOff = EEPROM.read(day1offadd);
  socket2DayOff = EEPROM.read(day2offadd);
  socket3DayOff = EEPROM.read(day3offadd);
  socket4DayOff = EEPROM.read(day4offadd);

  socket1TimeOff = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(time1offadd + i);
    if (c == '\0')
      break;
    socket1TimeOff += c;
  }
  socket2TimeOff = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(time2offadd + i);
    if (c == '\0')
      break;
    socket2TimeOff += c;
  }
  socket3TimeOff = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(time3offadd + i);
    if (c == '\0')
      break;
    socket3TimeOff += c;
  }
  socket4TimeOff = "";
  for (int i = 0;; ++i) {
    char c = EEPROM.read(time4offadd + i);
    if (c == '\0')
      break;
    socket4TimeOff += c;
  }

  socket1ToggleOff = EEPROM.read(toggle1offadd);
  socket2ToggleOff = EEPROM.read(toggle2offadd);
  socket3ToggleOff = EEPROM.read(toggle3offadd);
  socket4ToggleOff = EEPROM.read(toggle4offadd);

  EEPROM.end();
}

void writeIR(int iradd, uint64_t irval) {
  EEPROM.begin(512);
  for (int i = 0; i < sizeof(irval); ++i) {
    EEPROM.write(iradd + i, (irval >> (i * 8)) & 0xFF);
  }
  EEPROM.commit();
  EEPROM.end();
}

void writeIP(String ip) {
  EEPROM.begin(512);
  for (int i = 0; i < ip.length(); ++i) {
    EEPROM.write(ipaddr + i, ip[i]);
  }
  EEPROM.write(ipaddr + ip.length(), '\0');
  EEPROM.commit();
  EEPROM.end();
}

void clearEEPROM() {
  EEPROM.begin(512);
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
}

void handleRoot() {
  if (WiFi.status() == WL_CONNECTED) {
    server.send(200, "text/html", CONTROL());
  } else if (condition == true) {
    if (WiFi.scanComplete() != -1) {
      server.send(200, "text/html", LOGIN());
      WiFi.scanNetworks(true, true);
    }
    delay(3500);
    server.send(200, "text/html", LOGIN());
  } else {
    server.send(200, "text/html", ADDRESS());
  }
}

void handleIP() {
  server.send(200, "text/html", Details());
}

void handledetails() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    SSID = server.arg("ssid");
    PASS = server.arg("password");
    saveDetails(SSID, PASS);
    Serial.println("Received SSID: " + SSID);
    Serial.println("Received Password: " + PASS);
    putcondition(true, true);
    
    ESP.restart();

  } else {
    server.send(400, "text/plain", "Something went Wrong");
  }
}

void handleRTC() {
  timeClient.update();
  DateTime ntpDateTime = timeClient.getEpochTime();
  rtc.adjust(
    DateTime(
      ntpDateTime.year(),
      ntpDateTime.month(),
      ntpDateTime.day(),
      ntpDateTime.hour(),
      ntpDateTime.minute(),
      ntpDateTime.second()
    )
  );
  server.send(200, "text/html", SETTIME());
}


void handleSendValue() {
  if (server.hasArg("value")) {
    value = server.arg("value");
    sliderValue = (value.toInt() / 100.0) * 255;
    EEPROM.begin(512);
    EEPROM.write(sliderAdd, sliderValue);
    EEPROM.commit();
    EEPROM.end();
    analogWrite(pwm, sliderValue);
  }
}
void timer() {
  DateTime now = rtc.now();
  if (socket1ToggleOn && flagon1) {

    if ((socket1DayOn == now.dayOfTheWeek() || socket1DayOn == 7)) {

      int Hour = socket1TimeOn.substring(0, socket1TimeOn.indexOf(":")).toInt();
      int Minute = socket1TimeOn.substring(socket1TimeOn.indexOf(":") + 1).toInt();
      
      if (Hour == now.hour() && Minute == now.minute()) {
        pin1status = true;
        flagon1 = false;
        ledState();
      }
    }
  }

  if (socket2ToggleOn && flagon2) {

    if ((socket2DayOn == now.dayOfTheWeek() || socket2DayOn == 7)) {
      int Hour = socket2TimeOn.substring(0, socket2TimeOn.indexOf(":")).toInt();
      int Minute = socket2TimeOn.substring(socket2TimeOn.indexOf(":") + 1).toInt();
      
      if (Hour == now.hour() && Minute == now.minute()) {
        pin2status = true;
        flagon2 = false;
        ledState();
      }
    }
  }

  if (socket3ToggleOn && flagon3) {

    if ((socket3DayOn == now.dayOfTheWeek() || socket3DayOn == 7)) {
      int Hour = socket3TimeOn.substring(0, socket3TimeOn.indexOf(":")).toInt();
      int Minute = socket3TimeOn.substring(socket3TimeOn.indexOf(":") + 1).toInt();
     
      if (Hour == now.hour() && Minute == now.minute()) {
        pin3status = true;
        flagon3 = false;
        ledState();
      }
    }
  }

  if (socket4ToggleOn && flagon4) {

    if ((socket4DayOn == now.dayOfTheWeek() || socket4DayOn == 7)) {
      int Hour = socket4TimeOn.substring(0, socket4TimeOn.indexOf(":")).toInt();
      int Minute = socket4TimeOn.substring(socket4TimeOn.indexOf(":") + 1).toInt();
      
      if (Hour == now.hour() && Minute == now.minute()) {
        pin4status = true;
        flagon4 = false;
        ledState();
      }
    }
  }




  if (socket1ToggleOff && flagoff1) {

    if ((socket1DayOff == now.dayOfTheWeek() || socket1DayOff == 7)) {

      int Hour = socket1TimeOff.substring(0, socket1TimeOff.indexOf(":")).toInt();
      int Minute = socket1TimeOff.substring(socket1TimeOff.indexOf(":") + 1).toInt();
      
      if (Hour == now.hour() && Minute == now.minute()) {
        pin1status = false;
        flagoff1 = false;
        ledState();
      }
    }
  }
  if (socket2ToggleOff && flagoff2) {

    if ((socket2DayOff == now.dayOfTheWeek() || socket2DayOff == 7)) {
      int Hour = socket2TimeOff.substring(0, socket2TimeOff.indexOf(":")).toInt();
      int Minute = socket2TimeOff.substring(socket2TimeOff.indexOf(":") + 1).toInt();
      
      if (Hour == now.hour() && Minute == now.minute()) {
        pin2status = false;
        flagoff2 = false;
        ledState();
      }
    }
  }

  if (socket3ToggleOff && flagoff3) {

    if ((socket3DayOff == now.dayOfTheWeek() || socket3DayOff == 7)) {
      int Hour = socket3TimeOff.substring(0, socket3TimeOff.indexOf(":")).toInt();
      int Minute = socket3TimeOff.substring(socket3TimeOff.indexOf(":") + 1).toInt();
      
      if (Hour == now.hour() && Minute == now.minute()) {
        pin3status = false;
        flagoff3 = false;
        ledState();
      }
    }
  }

  if (socket4ToggleOff && flagoff4) {

    if ((socket4DayOff == now.dayOfTheWeek() || socket4DayOff == 7)) {
      int Hour = socket4TimeOff.substring(0, socket4TimeOff.indexOf(":")).toInt();
      int Minute = socket4TimeOff.substring(socket4TimeOff.indexOf(":") + 1).toInt();
      
      if (Hour == now.hour() && Minute == now.minute()) {
        pin4status = false;
        flagoff4 = false;
        ledState();
      }
    }
  }

Serial.println(flagon1);
}



void handleData() {
  String json = server.arg("plain");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);

  socket1DayOn = doc["socket1"]["dayOn"].as<int>();
  socket1TimeOn = doc["socket1"]["timeOn"].as<String>();
  socket1ToggleOn = doc["socket1"]["channelOn"].as<bool>();

  socket1DayOff = doc["socket1"]["dayOff"].as<int>();
  socket1TimeOff = doc["socket1"]["timeOff"].as<String>();
  socket1ToggleOff = doc["socket1"]["channelOff"].as<bool>();



  socket2DayOn = doc["socket2"]["dayOn"].as<int>();
  socket2TimeOn = doc["socket2"]["timeOn"].as<String>();
  socket2ToggleOn = doc["socket2"]["channelOn"].as<bool>();

  socket2DayOff = doc["socket2"]["dayOff"].as<int>();
  socket2TimeOff = doc["socket2"]["timeOff"].as<String>();
  socket2ToggleOff = doc["socket2"]["channelOff"].as<bool>();



  socket3DayOn = doc["socket3"]["dayOn"].as<int>();
  socket3TimeOn = doc["socket3"]["timeOn"].as<String>();
  socket3ToggleOn = doc["socket3"]["channelOn"].as<bool>();

  socket3DayOff = doc["socket3"]["dayOff"].as<int>();
  socket3TimeOff = doc["socket3"]["timeOff"].as<String>();
  socket3ToggleOff = doc["socket3"]["channelOff"].as<bool>();



  socket4DayOn = doc["socket4"]["dayOn"].as<int>();
  socket4TimeOn = doc["socket4"]["timeOn"].as<String>();
  socket4ToggleOn = doc["socket4"]["channelOn"].as<bool>();

  socket4DayOff = doc["socket4"]["dayOff"].as<int>();
  socket4TimeOff = doc["socket4"]["timeOff"].as<String>();
  socket4ToggleOff = doc["socket4"]["channelOff"].as<bool>();

  saveAlarm();

  flagon1=true; flagon2=true; flagon3=true; flagon4=true; flagoff1=true; flagoff2=true; flagoff3=true; flagoff4=true;
}

void handleWiFi() {
  if (WiFi.scanComplete() != -1) {
    server.send(200, "text/html", LOGIN());
    WiFi.scanNetworks(true, true);
  }
  delay(3500);
  server.send(200, "text/html", LOGIN());
}

void handleTimer() {
  server.send(200, "text/html", TIMER());
}

void hand1on() {
  pin1status = false;
  server.send(200, "text/html", CONTROL());
  ledState();
}

void hand1off() {
  pin1status = true;
  server.send(200, "text/html", CONTROL());
  ledState();
}

void hand2on() {
  pin2status = false;
  server.send(200, "text/html", CONTROL());
  ledState();
}

void hand2off() {
  pin2status = true;
  server.send(200, "text/html", CONTROL());
  ledState();
}

void hand3on() {
  pin3status = false;
  server.send(200, "text/html", CONTROL());
  ledState();
}

void hand3off() {
  pin3status = true;
  server.send(200, "text/html", CONTROL());
  ledState();
}

void hand4on() {
  pin4status = false;
  server.send(200, "text/html", CONTROL());
  ledState();
}

void hand4off() {
  pin4status = true;
  server.send(200, "text/html", CONTROL());
  ledState();
}

BLYNK_WRITE(V1)
{
  pin1status = static_cast<bool>(param.asInt());
  ledState();
}
BLYNK_WRITE(V2)
{
  pin2status = static_cast<bool>(param.asInt());
  ledState();
}
BLYNK_WRITE(V3)
{
  pin3status = static_cast<bool>(param.asInt());
  ledState();
}
BLYNK_WRITE(V4)
{
  pin4status = static_cast<bool>(param.asInt());
  ledState();
}

void handlereset() {
  clearEEPROM();
  putcondition(true, false);
  ESP.restart();
}

void handleremote() {

  server.send(200, "text/html", Remote());
  

  while (ir1add + add != 226) {

    while (irrecv.decode(&results) == 0) {
      server.handleClient();
      dnsServer.processNextRequest();
      delay(150);
    }

    if (results.value != 18446744073709551615) {
      writeIR(ir1add + add, results.value);
      add = add + 10;
      digitalWrite(Net, HIGH);
      delay(200);
      digitalWrite(Net, LOW);
    }
    irrecv.resume();
  }
}

void handleNotFound() {
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void sw() {
  if (digitalRead(sw1) != preSw1State) {
    pin1status = preSw1State;
    preSw1State = digitalRead(sw1);
  }
  if (digitalRead(sw2) != preSw2State) {
    pin2status = preSw2State;
    preSw2State = digitalRead(sw2);
  }
  if (digitalRead(sw3) != preSw3State) {
    pin3status = preSw3State;
    preSw3State = digitalRead(sw3);
  }
  if (digitalRead(sw4) != preSw4State) {
    pin4status = preSw4State;
    preSw4State = digitalRead(sw4);
  }
  if (int(((analogRead(A0) / 100) / 40.0) * 255) != preRegVal) {
    preRegVal = sliderValue = int(((analogRead(A0) / 100) / 40.0) * 255);
    value = String(((analogRead(A0) / 100) / 40.0) * 100);
    analogWrite(pwm, sliderValue);
  }
  ledState();
}

void remote() {
  if (results.value == 0x1FE50AF || results.value == ir1) {
    if (pin1status == true) {
      pin1status = false;
    } else {
      pin1status = true;
    }
  }
  if (results.value == 0x1FE807F) {
    ESP.restart();
  }

  if (results.value == 0x1FED827 || results.value == ir2) {
    if (pin2status == true) {
      pin2status = false;
    } else {     
      pin2status = true;
    }
  }

  if (results.value == 0x1FEF807 || results.value == ir3) {
    if (pin3status == true) {
      pin3status = false;
    } else {
      pin3status = true;
    }
  }

  if (results.value == 0x1FE30CF || results.value == ir4) {
    if (pin4status == true) {
      pin4status = false;
    } else {
      pin4status = true;
    }
  }

  if (results.value == 0x1FE48B7 || results.value == ir6) {
    bool status = (pin1status == false && pin2status == false && pin3status == false && pin4status == false) || (pin1status == true && pin2status == true && pin3status == true && pin4status == true);
    if (status == true) {
      pin1status = !pin1status;
      pin2status = !pin2status;
      pin3status = !pin3status;
      pin4status = !pin4status;
    } else {
      pin1status = false;
      pin2status = false;
      pin3status = false;
      pin4status = false;
    }
  }

  if (results.value == 33431775 || results.value == ir5) {
    digitalWrite(Net, LOW);
    delay(200);
    digitalWrite(Net, HIGH);
    delay(200);
    digitalWrite(Net, LOW);
    if (condition != false) {
      AP = true;
      putcondition(false, false);
      ESP.restart();
    } else {
      putcondition(true, false);
      ESP.restart();
    }
  }
  if (results.value == 0x1FEA05F || results.value == ir7) {
    sliderValue -= 25;
    if (sliderValue < 0) { sliderValue = 0; }
    value = String((sliderValue / 255.0) * 100);
    analogWrite(pwm, sliderValue);
  }

  if (results.value == 0x1FE609F || results.value == ir8) {
    sliderValue += 25;
    if (sliderValue > 250) { sliderValue = 255; }
    value = String((sliderValue / 255.0) * 100);
    analogWrite(pwm, sliderValue);
  }
  ledState();
}

void loop() {
  server.handleClient();
  dnsServer.processNextRequest();
  if ((digitalRead(sw1) != preSw1State) || (digitalRead(sw2) != preSw2State) || (digitalRead(sw3) != preSw3State) || (digitalRead(sw4) != preSw4State) || (int(((analogRead(A0) / 100) / 40.0) * 255) != preRegVal)) {
    sw();
  }

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(Net, LOW);
    if (!AP) {
      ESP.restart();
    }
  } else {
    if(connected_to_blynk){
      Blynk.run();
    }else{
      c++;
      if(c == 10000){
        if(Ping.ping(remote_ip, 1) && attempt < 5) { 
          Blynk.begin(auth, SSID.c_str(), PASS.c_str());
          connected_to_blynk = true;
        }
        attempt++;
        c=0;
      }
    }
  }

  if (!pin1status) {
    digitalWrite(pin1, LOW);
  } else {
    digitalWrite(pin1, HIGH);
  }
  if (!pin2status) {
    digitalWrite(pin2, LOW);
  } else {
    digitalWrite(pin2, HIGH);
  }
  if (!pin3status) {
    digitalWrite(pin3, LOW);
  } else {
    digitalWrite(pin3, HIGH);
  }
  if (!pin4status) {
    digitalWrite(pin4, LOW);
  } else {
    digitalWrite(pin4, HIGH);
  }

  if (digitalRead(config) == 0) {
    digitalWrite(Net, LOW);
    delay(200);
    digitalWrite(Net, HIGH);
    delay(200);
    digitalWrite(Net, LOW);
    if (condition != false) {
      AP = true;
      putcondition(false,false);
      ESP.restart();
    } else {
      putcondition(true, false);
      ESP.restart();
    }
  }
  
  
    if (irrecv.decode(&results)) {
    remote();
    Serial.println(results.value);
    irrecv.resume();
  }
  if (socket1ToggleOn|| socket1ToggleOff || socket2ToggleOn || socket2ToggleOff|| socket3ToggleOn|| socket3ToggleOff || socket4ToggleOn || socket4ToggleOff ) {
    if (k == 1000) {
      timer();
      k = 0;
    }
    k++;
  }
  DateTime now = rtc.now();
  if((now.hour() == 00 && now.minute() == 1) || (now.hour() == 6 && now.minute() == 1) || (now.hour() == 12 && now.minute() == 1) || (now.hour() == 18 && now.minute() == 1) ){
    ESP.restart();
  }
  delay(10);
}

String SETTIME(){
  String htmlString = "<!DOCTYPE html>\n";
    htmlString += "<html lang=\"en\">\n";
    htmlString += "<head>\n";
    htmlString += "    <meta charset=\"UTF-8\">\n";
    htmlString += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    htmlString += "    <title>Time Adjust</title>\n";
    htmlString += "    <style>\n";
    htmlString += "        body {\n";
    htmlString += "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n";
    htmlString += "            margin: 0;\n";
    htmlString += "            padding: 0;\n";
    htmlString += "            background-color: #f4f4f4;\n";
    htmlString += "            color: #333;\n";
    htmlString += "            text-align: center;\n";
    htmlString += "        }\n";
    htmlString += "        header {\n";
    htmlString += "            background-color: #004578;\n";
    htmlString += "            color: white;\n";
    htmlString += "            padding: 10px 0;\n";
    htmlString += "        }\n";
    htmlString += "        main {\n";
    htmlString += "            margin: 20px;\n";
    htmlString += "            padding: 20px;\n";
    htmlString += "            background-color: white;\n";
    htmlString += "            box-shadow: 0 4px 8px rgba(0,0,0,0.1);\n";
    htmlString += "            display: inline-block;\n";
    htmlString += "            width: auto;\n";
    htmlString += "        }\n";
    htmlString += "        .time {\n";
    htmlString += "            color: #004578;\n";
    htmlString += "            font-size: 2em;\n";
    htmlString += "            margin: 10px 0;\n";
    htmlString += "        }\n";
    htmlString += "        .date {\n";
    htmlString += "            color: #666;\n";
    htmlString += "            font-size: 1.5em;\n";
    htmlString += "            margin-bottom: 20px;\n";
    htmlString += "        }\n";
    htmlString += "        .note {\n";
    htmlString += "            color: #147029; \n";
    htmlString += "            font-size: 14px; \n";
    htmlString += "            margin-top: 30px;\n";
    htmlString += "            background-color: #d4edda; \n";
    htmlString += "            padding: 15px; \n";
    htmlString += "            border-left: 6px solid #c3e6cb; \n";
    htmlString += "            border-radius: 5px; \n";
    htmlString += "        }\n";
    htmlString += "    </style>\n";
    htmlString += "</head>\n";
    htmlString += "<body>\n";
    htmlString += "    <header>\n";
    htmlString += "        <h1>Adjust Time</h1>\n";
    htmlString += "    </header>\n";
    htmlString += "    <main>\n";
    DateTime now = rtc.now();
    int hour = now.hour();
    String ampm = "AM";
    if (hour >= 12) {
      ampm = "PM";
      if (hour > 12) {
        hour -= 12;
      }
    }
    if (hour == 0) {
      hour = 12;
    }

      String hourString = (hour < 10 ? "0" : "") + String(hour);
      String minuteString = (now.minute() < 10 ? "0" : "") + String(now.minute());
      String timeString = hourString + ":" + minuteString + " " + ampm;
      htmlString += "        <div class=\"time\">Time: " + timeString + "</div>\n";
      htmlString += "        <div class=\"date\">Date: "+day[now.dayOfTheWeek()]+"</div>\n";
      htmlString += "    </main>\n";
      
    if(connected_to_blynk){
      htmlString += "    <div class=\"note\">\n";
      htmlString += "        <p>Note: This page will automatically synchronize the date and time of your extension board from the NTP server. Refresh this page until the current time is displayed.</p>\n";
      htmlString += "    </div>\n";
    }else{
      htmlString += "    <div class=\"note\">\n";
      htmlString += "        <p>Note:  you are not connected to NTP server!!🔌 Please Check your Internet Conection and try again.</p>\n";
      htmlString += "    </div>\n";
    }   

    htmlString += "</body>\n";
    htmlString += "</html>\n";
 return htmlString;
}

String Details() {
  String L = "<!DOCTYPE html>\n";
  L += "<html lang=\"en\">\n";
  L += "<head>\n";
  L += "    <meta charset=\"UTF-8\">\n";
  L += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  L += "    <title>String Display Page</title>\n";
  L += "    <style>\n";
  L += "        body {\n";
  L += "            font-family: Arial, sans-serif;\n";
  L += "            margin: 0;\n";
  L += "            padding: 0;\n";
  L += "            background-color: #f2f2f2;\n";
  L += "        }\n";
  L += "\n";
  L += "        header {\n";
  L += "            background-color: #333;\n";
  L += "            color: white;\n";
  L += "            text-align: center;\n";
  L += "            padding: 1em;\n";
  L += "        }\n";
  L += "\n";
  L += "        main {\n";
  L += "            max-width: 600px;\n";
  L += "            margin: 20px auto;\n";
  L += "            background-color: white;\n";
  L += "            padding: 20px;\n";
  L += "            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n";
  L += "        }\n";
  L += "    </style>\n";
  L += "</head>\n";
  L += "<body>\n";
  L += "\n";
  L += "    <header>\n";
  L += "        <h1>WiFi Details</h1>\n";
  L += "    </header>\n";
  L += "\n";
  L += "    <main>\n";
  L += "        <p>SSID&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;:&nbsp;&nbsp;&nbsp;&nbsp; " + SSID + " </p>\n";
  L += "        <p>Password &nbsp;&nbsp;    : &nbsp;&nbsp; &nbsp;গোপনীয়</p>\n";
  L += "        <p>IP Address &nbsp;  : &nbsp;&nbsp;&nbsp;&nbsp; " + IP + " </p>\n";
  L += "    </main>\n";
  L += "\n";
  L += "</body>\n";
  L += "\n";
  L += "</html>";
  return L;
}
String ADDRESS() {
  String L = "<!DOCTYPE html>\n";
  L += "<html lang=\"en\">\n";
  L += "<head>\n";
  L += "    <meta charset=\"UTF-8\">\n";
  L += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  L += "    <title>Configuration Page</title>\n";
  L += "    <style>\n";
  L += "        body {\n";
  L += "            font-family: Arial, sans-serif;\n";
  L += "            margin: 0;\n";
  L += "            padding: 0;\n";
  L += "            background-color: #f2f2f2;\n";
  L += "        }\n";
  L += "\n";
  L += "        header {\n";
  L += "            background-color: #333;\n";
  L += "            color: white;\n";
  L += "            text-align: center;\n";
  L += "            padding: 1em;\n";
  L += "        }\n";
  L += ".close{\n";
  L +="      display: inline-block;\n";
  L +="      margin-top: 100px;\n";
  L +="      color: #000000;\n";
  L +="      background-color: #ffffff;\n";
  L +="      padding: 5px 10px;\n";
  L +="      font-size: 10px;\n";
  L +="      border: none;\n";
  L +="      border-radius: 1px;\n";
  L +="      text-align: center;\n";
  L +="      text-decoration: none;\n";
  L +="      margin: 7 560px 1px auto; \n";
  L +="    }\n";
  L += "\n";
  L += "        main {\n";
  L += "            max-width: 600px;\n";
  L += "            margin: 20px auto;\n";
  L += "            background-color: white;\n";
  L += "            padding: 20px;\n";
  L += "            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n";
  L += "        }\n";
  L += "\n";
  L += "        button {\n";
  L += "            background-color: #333;\n";
  L += "            color: white;\n";
  L += "            padding: 15px;\n";
  L += "            border: none;\n";
  L += "            margin: 10px 0;\n";
  L += "            cursor: pointer;\n";
  L += "            width: 100%;\n";
  L += "            box-sizing: border-box;\n";
  L += "        }\n";
  L += "\n";
  L += "        button:hover {\n";
  L += "            background-color: #555;\n";
  L += "        }\n";
  L += "    </style>\n";
  L += "</head>\n";
  L += "<body>\n";
  L += "\n";
  L += "    <header>\n";
  L += "        <h1>Configuration Page</h1>\n";
  L += "    </header>\n";
  L += "\n";
  L += "    <main>\n";
  L += "        <a href = \"/IP\" > <button>Show WiFi details</button></a>\n";
  L += "        <a href = \"/WiFi\" > <button>Change WiFi Network</button></a>\n";
  L += "        <a href = \"/IR\" > <button>Add IR Remote</button></a>\n";
  L += "        <a href = \"/factory\" > <button>Factory Reset</button></a>\n";
  L += "    </main>\n";
  L += "    <a class=\"close\" href=\"/close\">close</a>\n";
  L += "\n";
  L += "</body>\n";
  L += "\n";
  L += "</html>";

  return L;
}

String LOGIN() {
  String L = "<!DOCTYPE html>\n";
  L += "<html lang=\"en\">\n";
  L += "<head>\n";
  L += "    <meta charset=\"UTF-8\">\n";
  L += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  L += "    <title>SMART CONTROL</title>\n";
  L += "    <style>\n";
  L += "        .login-form {\n";
  L += "            margin-top: 20px;\n";
  L += "            display: flex;\n";
  L += "            flex-direction: column;\n";
  L += "        }\n";
  L += "        .login-input {\n";
  L += "            margin-bottom: 10px;\n";
  L += "            padding: 5px;\n";
  L += "            font-size: 16px;\n";
  L += "        }\n";
  L += "        .login-submit {\n";
  L += "            padding: 10px;\n";
  L += "            font-size: 18px;\n";
  L += "            background-color: #4CAF50;\n";
  L += "            color: white;\n";
  L += "            border: none;\n";
  L += "            border-radius: 5px;\n";
  L += "            cursor: pointer;\n";
  L += "        }\n";
  L += "    </style>\n";
  L += "</head>\n";
  L += "<body>\n";
  L += "    <Center><h1>Connect To Your WiFi Network</h1></Center></br> \n";
  L += "    <h3> Available Networks: </h3>\n";
  L += " <ul>\n";
  int numNetworks = WiFi.scanComplete();
  if (numNetworks < 0) {
    L += "<li>No networks found</li>";
  } else {
    for (int i = 0; i < numNetworks; ++i) {
      L += " <li onclick = \"populateSSID('" + WiFi.SSID(i) + "')\">" + WiFi.SSID(i) + "</li>\n";
    }
  }
  L += "</ul>\n";
  L += "    <div class=\"login-form\">\n";
  L += "        <label for=\"ssid\" class=\"login-input\">WiFi SSID:</label>\n";
  L += "        <input type=\"text\" id=\"ssid\" name=\"ssid\" class=\"login-input\" required>\n";
  L += "        <label for=\"password\" class=\"login-input\">WiFi Password:</label>\n";
  L += "        <input type=\"password\" id=\"password\" name=\"password\" class=\"login-input\" required>\n";
  L += "        <button class=\"login-submit\" onclick=\"submitForm()\">Submit</button>\n";
  L += "    </div>\n";
  L += "<script>\n";
  L += "function populateSSID(ssid) {\n";
  L += "    document.getElementById('ssid').value = ssid;\n";
  L += "}\n";
  L += "function submitForm() {\n";
  L += "    var ssid = document.getElementById('ssid').value;\n";
  L += "    var password = document.getElementById('password').value;\n";
  L += "    sendCredentials(ssid, password);\n";
  L += "}\n";
  L += "function sendCredentials(ssid, password) {\n";
  L += "    var xhttp = new XMLHttpRequest();\n";
  L += "    xhttp.onreadystatechange = function() {\n";
  L += "        if (this.readyState == 4 && this.status == 200) {\n";
  L += "            // Handle the response if needed\n";
  L += "            console.log('Credentials sent successfully!');\n";
  L += "        }\n";
  L += "    };\n";
  L += "    xhttp.open('POST', '/credentials', true);\n";
  L += "    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');\n";
  L += "    xhttp.send('ssid=' + ssid + '&password=' + password);\n";
  L += "}\n";
  L += "</script>\n";
  L += "</body>\n";
  L += "</html>";
  return L;
}
String TIMER() {
  String htmlString = "<!DOCTYPE html>\n";
  htmlString += "<html lang=\"en\">\n";
  htmlString += "\n";
  htmlString += "<head>\n";
  htmlString += "    <meta charset=\"UTF-8\">\n";
  htmlString += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  htmlString += "    <title>TIMER</title>\n";
  htmlString += "    <style>\n";
  htmlString += "        body {\n";
  htmlString += "            font-family: Arial, sans-serif;\n";
  htmlString += "            margin: 0;\n";
  htmlString += "            padding: 20px;\n";
  htmlString += "            text-align: center;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        h1 {\n";
  htmlString += "            color: #333;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        .alarm-input {\n";
  htmlString += "            margin: 10px;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        .toggle-switch {\n";
  htmlString += "            position: relative;\n";
  htmlString += "            display: inline-block;\n";
  htmlString += "            margin-top: 0px;\n";
  htmlString += "            width: 60px;\n";
  htmlString += "            height: 33px;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        .toggle-switch input {\n";
  htmlString += "            opacity: 0;\n";
  htmlString += "            width: 0;\n";
  htmlString += "            height: 0;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        .toggle-slider {\n";
  htmlString += "            position: absolute;\n";
  htmlString += "            cursor: pointer;\n";
  htmlString += "            top: 0;\n";
  htmlString += "            left: 0;\n";
  htmlString += "            right: 0;\n";
  htmlString += "            bottom: 0;\n";
  htmlString += "            background-color: #ccc;\n";
  htmlString += "            -webkit-transition: .4s;\n";
  htmlString += "            transition: .4s;\n";
  htmlString += "            border-radius: 30px;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        .toggle-slider:before {\n";
  htmlString += "            position: absolute;\n";
  htmlString += "            content: \"\";\n";
  htmlString += "            height: 26px;\n";
  htmlString += "            width: 26px;\n";
  htmlString += "            left: 4px;\n";
  htmlString += "            bottom: 4px;\n";
  htmlString += "            background-color: white;\n";
  htmlString += "            -webkit-transition: .4s;\n";
  htmlString += "            transition: .4s;\n";
  htmlString += "            border-radius: 60%;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        input:checked + .toggle-slider {\n";
  htmlString += "            background-color: #4CAF50;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        input:checked + .toggle-slider:before {\n";
  htmlString += "            -webkit-transform: translateX(26px);\n";
  htmlString += "            -ms-transform: translateX(26px);\n";
  htmlString += "            transform: translateX(26px);\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        button {\n";
  htmlString += "            background-color: #254e27;\n";
  htmlString += "            color: white;\n";
  htmlString += "            padding: 20px 250px;\n";
  htmlString += "            font-size: 25px;\n";
  htmlString += "            border: none;\n";
  htmlString += "            cursor: pointer;\n";
  htmlString += "        }\n";
  htmlString += "\n";
  htmlString += "        button:hover {\n";
  htmlString += "            background-color: rgb(255, 130, 00);\n";
  htmlString += "        }\n";

  htmlString += "        @media only screen and (max-width: 600px) {\n";
  htmlString += "            button {\n";
  htmlString += "                 color: white;\n";
  htmlString += "                padding: 15px 170px;\n";
  htmlString += "            }\n";

  htmlString += "        }\n";
  htmlString += "    </style>\n";

  htmlString += "    <script>\n";
  htmlString += "        function sendData() {\n";
  htmlString += "            const data = {\n";
  htmlString += "                socket1: {\n";
  htmlString += "                    dayOn: document.getElementById('day1on').value,\n";
  htmlString += "                    timeOn: document.getElementById('time1on').value,\n";
  htmlString += "                    channelOn: document.getElementById('channel1on').checked,\n";
  htmlString += "                    dayOff: document.getElementById('day1off').value,\n";
  htmlString += "                    timeOff: document.getElementById('time1off').value,\n";
  htmlString += "                    channelOff: document.getElementById('channel1off').checked\n";
  htmlString += "                },\n";
  htmlString += "                socket2: {\n";
  htmlString += "                    dayOn: document.getElementById('day2on').value,\n";
  htmlString += "                    timeOn: document.getElementById('time2on').value,\n";
  htmlString += "                    channelOn: document.getElementById('channel2on').checked,\n";
  htmlString += "                    dayOff: document.getElementById('day2off').value,\n";
  htmlString += "                    timeOff: document.getElementById('time2off').value,\n";
  htmlString += "                    channelOff: document.getElementById('channel2off').checked\n";
  htmlString += "                },\n";
  htmlString += "                socket3: {\n";
  htmlString += "                    dayOn: document.getElementById('day3on').value,\n";
  htmlString += "                    timeOn: document.getElementById('time3on').value,\n";
  htmlString += "                    channelOn: document.getElementById('channel3on').checked,\n";
  htmlString += "                    dayOff: document.getElementById('day3off').value,\n";
  htmlString += "                    timeOff: document.getElementById('time3off').value,\n";
  htmlString += "                    channelOff: document.getElementById('channel3off').checked\n";
  htmlString += "                },\n";
  htmlString += "                socket4: {\n";
  htmlString += "                    dayOn: document.getElementById('day4on').value,\n";
  htmlString += "                    timeOn: document.getElementById('time4on').value,\n";
  htmlString += "                    channelOn: document.getElementById('channel4on').checked,\n";
  htmlString += "                    dayOff: document.getElementById('day4off').value,\n";
  htmlString += "                    timeOff: document.getElementById('time4off').value,\n";
  htmlString += "                    channelOff: document.getElementById('channel4off').checked\n";
  htmlString += "                }\n";
  htmlString += "            };\n";
  htmlString += "\n";
  htmlString += "            const xhr = new XMLHttpRequest();\n";
  htmlString += "            xhr.open('POST', '/set-alarm', true);\n";
  htmlString += "            xhr.setRequestHeader('Content-Type', 'application/json');\n";
  htmlString += "            xhr.send(JSON.stringify(data));\n";
  htmlString += "        }\n";

  htmlString += "    </script>\n";

  htmlString += "</head>\n";
  htmlString += "\n";
  htmlString += "<body>\n";
  htmlString += "    <h1>SET ALARM</h1>\n";
  htmlString += "    <hr>\n";
  htmlString += "    <hr>\n";
  htmlString += "\n";

  htmlString += "    <h3>SOCKET 1:</h3>\n";
  htmlString += "    <h4>Turned on:</h4>\n";
  htmlString += "    <label for=\"day1on\">Day:</label>\n";
  htmlString += "    <select id=\"day1on\" class=\"alarm-input\">\n";
  htmlString += "        <option value=\"" + String(socket1DayOn) + "\" selected>" + day[socket1DayOn] + "</option>\n";
  htmlString += "        <option value=\"0\">Sunday</option>\n";
  htmlString += "        <option value=\"1\">Monday</option>\n";
  htmlString += "        <option value=\"2\">Tuesday</option>\n";
  htmlString += "        <option value=\"3\">Wednesday</option>\n";
  htmlString += "        <option value=\"4\">Thursday</option>\n";
  htmlString += "        <option value=\"5\">Friday</option>\n";
  htmlString += "        <option value=\"6\">Saturday</option>\n";
  htmlString += "        <option value=\"7\">Every Day</option>\n";
  htmlString += "    </select>\n";
  htmlString += "\n";

  htmlString += "  <label for=\"time1on\">Time:</label>\n";
  htmlString += "  <input type = \"time\" id=\"time1on\" value = "+socket1TimeOn+" class=\"alarm-input\" step=\"60\">\n";

  htmlString += "    <label>\n";
  htmlString += "        <div class=\"toggle-switch\">\n";
  if (socket1ToggleOn) {
    htmlString += "            <input type=\"checkbox\" id=\"channel1on\" checked>\n";
  } else {
    htmlString += "            <input type=\"checkbox\" id=\"channel1on\">\n";
  }
  htmlString += "            <div class=\"toggle-slider\"></div>\n";
  htmlString += "        </div>\n";
  htmlString += "    </label>\n";
  htmlString += " <br>\n";

  htmlString += "    <br><h4>Turned off:</h4>\n";
  htmlString += "    <label for=\"day1off\">Day:</label>\n";
  htmlString += "    <select id=\"day1off\" class=\"alarm-input\">\n";
  htmlString += "        <option value=\"" + String(socket1DayOff) + "\" selected>" + day[socket1DayOff] + "</option>\n";
  htmlString += "        <option value=\"0\">Sunday</option>\n";
  htmlString += "        <option value=\"1\">Monday</option>\n";
  htmlString += "        <option value=\"2\">Tuesday</option>\n";
  htmlString += "        <option value=\"3\">Wednesday</option>\n";
  htmlString += "        <option value=\"4\">Thursday</option>\n";
  htmlString += "        <option value=\"5\">Friday</option>\n";
  htmlString += "        <option value=\"6\">Saturday</option>\n";
  htmlString += "        <option value=\"7\">Every Day</option>\n";
  htmlString += "    </select>\n";
  htmlString += "\n";

  htmlString += "  <label for=\"time1off\">Time:</label>\n";
  htmlString += "  <input type = \"time\" id=\"time1off\" value =  "+socket1TimeOff+" class=\"alarm-input\" step=\"60\">\n";


  htmlString += "    <label>\n";
  htmlString += "        <div class=\"toggle-switch\">\n";
  if (socket1ToggleOff) {
    htmlString += "            <input type=\"checkbox\" id=\"channel1off\" checked>\n";
  } else {
    htmlString += "            <input type=\"checkbox\" id=\"channel1off\">\n";
  }
  htmlString += "            <div class=\"toggle-slider\"></div>\n";
  htmlString += "        </div>\n";
  htmlString += "    </label>\n";



  htmlString += "    <hr>\n";


  htmlString += "\n";
  htmlString += "    <h3>SOCKET 2:</h3>\n";
  htmlString += "    <h4>Turned on:</h4>\n";
  htmlString += "    <label for=\"day2on\">Day:</label>\n";
  htmlString += "    <select id=\"day2on\" class=\"alarm-input\">\n";
  htmlString += "        <option value=\"" + String(socket2DayOn) + "\" selected>" + day[socket2DayOn] + "</option>\n";
  htmlString += "        <option value=\"0\">Sunday</option>\n";
  htmlString += "        <option value=\"1\">Monday</option>\n";
  htmlString += "        <option value=\"2\">Tuesday</option>\n";
  htmlString += "        <option value=\"3\">Wednesday</option>\n";
  htmlString += "        <option value=\"4\">Thursday</option>\n";
  htmlString += "        <option value=\"5\">Friday</option>\n";
  htmlString += "        <option value=\"6\">Saturday</option>\n";
  htmlString += "        <option value=\"7\">Every Day</option>\n";
  htmlString += "    </select>\n";
  htmlString += "\n";

  htmlString += "  <label for=\"time2on\">Time:</label>\n";
  htmlString += "  <input type = \"time\" id=\"time2on\" value= "+socket2TimeOn+" class=\"alarm-input\" step=\"60\">\n";


  htmlString += "    <label>\n";
  htmlString += "        <div class=\"toggle-switch\">\n";
  if (socket2ToggleOn) {
    htmlString += "            <input type=\"checkbox\" id=\"channel2on\" checked>\n";
  } else {
    htmlString += "            <input type=\"checkbox\" id=\"channel2on\">\n";
  }
  htmlString += "            <div class=\"toggle-slider\"></div>\n";
  htmlString += "        </div>\n";
  htmlString += "    </label>\n";
  htmlString += " <br>\n";

  htmlString += "    <br><h4>Turned off:</h4>\n";
  htmlString += "    <label for=\"day2off\">Day:</label>\n";
  htmlString += "    <select id=\"day2off\" class=\"alarm-input\">\n";
  htmlString += "        <option value=\"" + String(socket2DayOff) + "\" selected>" + day[socket2DayOff] + "</option>\n";
  htmlString += "        <option value=\"0\">Sunday</option>\n";
  htmlString += "        <option value=\"1\">Monday</option>\n";
  htmlString += "        <option value=\"2\">Tuesday</option>\n";
  htmlString += "        <option value=\"3\">Wednesday</option>\n";
  htmlString += "        <option value=\"4\">Thursday</option>\n";
  htmlString += "        <option value=\"5\">Friday</option>\n";
  htmlString += "        <option value=\"6\">Saturday</option>\n";
  htmlString += "        <option value=\"7\">Every Day</option>\n";
  htmlString += "    </select>\n";
  htmlString += "\n";


  htmlString += "  <label for=\"time2off\">Time:</label>\n";
  htmlString += "  <input type = \"time\" id=\"time2off\" value = "+socket2TimeOff+" class=\"alarm-input\" step=\"60\">\n";

  htmlString += "    <label>\n";
  htmlString += "        <div class=\"toggle-switch\">\n";
  if (socket2ToggleOff) {
    htmlString += "            <input type=\"checkbox\" id=\"channel2off\" checked>\n";
  } else {
    htmlString += "            <input type=\"checkbox\" id=\"channel2off\">\n";
  }
  htmlString += "            <div class=\"toggle-slider\"></div>\n";
  htmlString += "        </div>\n";
  htmlString += "    </label>\n";


  htmlString += "    <hr>\n";


  htmlString += "\n";
  htmlString += "    <h3>SOCKET 3:</h3>\n";
  htmlString += "    <h4>Turned on:</h4>\n";
  htmlString += "    <label for=\"day3on\">Day:</label>\n";
  htmlString += "    <select id=\"day3on\" class=\"alarm-input\">\n";
  htmlString += "        <option value=\"" + String(socket3DayOn) + "\" selected>" + day[socket3DayOn] + "</option>\n";
  htmlString += "        <option value=\"0\">Sunday</option>\n";
  htmlString += "        <option value=\"1\">Monday</option>\n";
  htmlString += "        <option value=\"2\">Tuesday</option>\n";
  htmlString += "        <option value=\"3\">Wednesday</option>\n";
  htmlString += "        <option value=\"4\">Thursday</option>\n";
  htmlString += "        <option value=\"5\">Friday</option>\n";
  htmlString += "        <option value=\"6\">Saturday</option>\n";
  htmlString += "        <option value=\"7\">Every Day</option>\n";
  htmlString += "    </select>\n";
  htmlString += "\n";


  htmlString += "  <label for=\"time3on\">Time:</label>\n";
  htmlString += "  <input type = \"time\" id=\"time3on\" value = "+socket3TimeOn+" class=\"alarm-input\" step=\"60\">\n";

  htmlString += "    <label>\n";
  htmlString += "        <div class=\"toggle-switch\">\n";
  if (socket3ToggleOn) {
    htmlString += "            <input type=\"checkbox\" id=\"channel3on\" checked>\n";
  } else {
    htmlString += "            <input type=\"checkbox\" id=\"channel3on\">\n";
  }
  htmlString += "            <div class=\"toggle-slider\"></div>\n";
  htmlString += "        </div>\n";
  htmlString += "    </label>\n";
  htmlString += " <br>\n";

  htmlString += "    <br><h4>Turned off:</h4>\n";
  htmlString += "    <label for=\"day3off\">Day:</label>\n";
  htmlString += "    <select id=\"day3off\" class=\"alarm-input\">\n";
  htmlString += "        <option value=\"" + String(socket3DayOff) + "\" selected>" + day[socket3DayOff] + "</option>\n";
  htmlString += "        <option value=\"0\">Sunday</option>\n";
  htmlString += "        <option value=\"1\">Monday</option>\n";
  htmlString += "        <option value=\"2\">Tuesday</option>\n";
  htmlString += "        <option value=\"3\">Wednesday</option>\n";
  htmlString += "        <option value=\"4\">Thursday</option>\n";
  htmlString += "        <option value=\"5\">Friday</option>\n";
  htmlString += "        <option value=\"6\">Saturday</option>\n";
  htmlString += "        <option value=\"7\">Every Day</option>\n";
  htmlString += "    </select>\n";
  htmlString += "\n";

  htmlString += "  <label for=\"time3off\">Time:</label>\n";
  htmlString += "  <input type = \"time\" id=\"time3off\" value = "+socket3TimeOff+" class=\"alarm-input\" step=\"60\">\n";


  htmlString += "    <label>\n";
  htmlString += "        <div class=\"toggle-switch\">\n";
  if (socket3ToggleOff) {
    htmlString += "            <input type=\"checkbox\" id=\"channel3off\" checked>\n";
  } else {
    htmlString += "            <input type=\"checkbox\" id=\"channel3off\">\n";
  }
  htmlString += "            <div class=\"toggle-slider\"></div>\n";
  htmlString += "        </div>\n";
  htmlString += "    </label>\n";


  htmlString += "    <hr>\n";
  htmlString += "\n";



  htmlString += "   <h3>SOCKET 4:</h3>\n";
  htmlString += "    <h4>Turned on:</h4>\n";
  htmlString += "    <label for=\"day4on\">Day:</label>\n";
  htmlString += "    <select id=\"day4on\" class=\"alarm-input\">\n";
  htmlString += "        <option value=\"" + String(socket4DayOn) + "\" selected>" + day[socket4DayOn] + "</option>\n";
  htmlString += "        <option value=\"0\">Sunday</option>\n";
  htmlString += "        <option value=\"1\">Monday</option>\n";
  htmlString += "        <option value=\"2\">Tuesday</option>\n";
  htmlString += "        <option value=\"3\">Wednesday</option>\n";
  htmlString += "        <option value=\"4\">Thursday</option>\n";
  htmlString += "        <option value=\"5\">Friday</option>\n";
  htmlString += "        <option value=\"6\">Saturday</option>\n";
  htmlString += "        <option value=\"7\">Every Day</option>\n";
  htmlString += "    </select>\n";
  htmlString += "\n";

  htmlString += "  <label for=\"time4on\">Time:</label>\n";
  htmlString += "  <input type = \"time\" id=\"time4on\" value = "+socket4TimeOn+" class=\"alarm-input\" step=\"60\">\n";

  htmlString += "    <label>\n";
  htmlString += "        <div class=\"toggle-switch\">\n";
  if (socket4ToggleOn) {
    htmlString += "            <input type=\"checkbox\" id=\"channel4on\" checked>\n";
  } else {
    htmlString += "            <input type=\"checkbox\" id=\"channel4on\">\n";
  }
  htmlString += "            <div class=\"toggle-slider\"></div>\n";
  htmlString += "        </div>\n";
  htmlString += "    </label>\n";
  htmlString += "\n";
  htmlString += " <br>\n";

  htmlString += "    <br><h4>Turned off:</h4>\n";
  htmlString += "    <label for=\"day4off\">Day:</label>\n";
  htmlString += "    <select id=\"day4off\" class=\"alarm-input\">\n";
  htmlString += "        <option value=\"" + String(socket4DayOff) + "\" selected>" + day[socket4DayOff] + "</option>\n";
  htmlString += "        <option value=\"0\">Sunday</option>\n";
  htmlString += "        <option value=\"1\">Monday</option>\n";
  htmlString += "        <option value=\"2\">Tuesday</option>\n";
  htmlString += "        <option value=\"3\">Wednesday</option>\n";
  htmlString += "        <option value=\"4\">Thursday</option>\n";
  htmlString += "        <option value=\"5\">Friday</option>\n";
  htmlString += "        <option value=\"6\">Saturday</option>\n";
  htmlString += "        <option value=\"7\">Every Day</option>\n";
  htmlString += "    </select>\n";
  htmlString += "\n";

  htmlString += "  <label for=\"time4off\">Time:</label>\n";
  htmlString += "  <input type = \"time\" id=\"time4off\" value = "+socket4TimeOff+" class=\"alarm-input\" step=\"60\">\n";

  htmlString += "    <label>\n";
  htmlString += "        <div class=\"toggle-switch\">\n";
  if (socket4ToggleOff) {
    htmlString += "            <input type=\"checkbox\" id=\"channel4off\" checked>\n";
  } else {
    htmlString += "            <input type=\"checkbox\" id=\"channel4off\">\n";
  }
  htmlString += "            <div class=\"toggle-slider\"></div>\n";
  htmlString += "        </div>\n";
  htmlString += "    </label>\n";
  htmlString += "\n";

  htmlString += "    <hr>\n";
  htmlString += "    <hr>\n";
  htmlString += "<button onclick='sendData()'> SET </button>\n";
  htmlString += "\n";
  htmlString += "</body>\n";
  htmlString += "\n";
  htmlString += "</html>";
  return htmlString;
}

String Remote() {
  String L = "<!DOCTYPE html>\n";
  L += "<html lang=\"en\">\n";
  L += "<head>\n";
  L += "    <meta charset=\"UTF-8\">\n";
  L += "  <meta http-equiv=\"refresh\" content=\"1\"> \n";
  L += "    <title>IR Remote Switch Selection</title>\n";
  L += "    <style>\n";
  L += "        body {\n";
  L += "            font-family: 'Arial', sans-serif;\n";
  L += "            margin: 0;\n";
  L += "            padding: 0;\n";
  L += "            background-color: #f5f5f5;\n";
  L += "            color: #333;\n";
  L += "        }\n";
  L += "\n";
  L += "        header {\n";
  L += "            text-align: center;\n";
  L += "            padding: 20px;\n";
  L += "            background-color: #4285f4;\n";
  L += "            color: #fff;\n";
  L += "        }\n";
  L += "\n";
  L += "        .container {\n";
  L += "            width: 80%;\n";
  L += "            margin: 30px auto;\n";
  L += "            display: flex;\n";
  L += "            flex-wrap: wrap;\n";
  L += "            justify-content: space-around;\n";
  L += "            align-items: stretch;\n";
  L += "        }\n";
  L += "\n";
  L += "        .border-box {\n";
  L += "            flex: 0 0 30%;\n";
  L += "            padding: 20px;\n";
  L += "            margin-bottom: 20px;\n";
  L += "            background-color: #fff;\n";
  L += "            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);\n";
  L += "        }\n";
  L += "\n";
  L += "        .border-box h2 {\n";
  L += "            margin-top: 0;\n";
  L += "            padding-bottom: 10px;\n";
  L += "            border-bottom: 2px solid #4285f4;\n";
  L += "        }\n";
  if (ir1add + add == 156) {
    L += "        .border-box:nth-child(1) {\n";
    L += "            border: 5px solid #e53935; /* Red color for Socket-1 */\n";
    L += "        }\n";

  }

  else if (ir1add + add == 166) {
    L += "        .border-box:nth-child(2) {\n";
    L += "            border: 5px solid #e53935; /* Red color for Socket-1 */\n";
    L += "        }\n";
  }

  else if (ir1add + add == 176) {
    L += "        .border-box:nth-child(3) {\n";
    L += "            border: 5px solid #e53935; /* Red color for Socket-1 */\n";
    L += "        }\n";
  }

  else if (ir1add + add == 186) {
    L += "        .border-box:nth-child(4) {\n";
    L += "            border: 5px solid #e53935; \n";
    L += "        }\n";
  }

  else if (ir1add + add == 196) {
    L += "        .border-box:nth-child(5) {\n";
    L += "            border: 5px solid #e53935; \n";
    L += "        }\n";
  }

  else if (ir1add + add == 206) {
    L += "        .border-box:nth-child(6) {\n";
    L += "            border: 5px solid #e53935; \n";
    L += "        }\n";
  } else if (ir1add + add == 216) {
    L += "        .border-box:nth-child(7) {\n";
    L += "            border: 5px solid #e53935;\n";
    L += "        }\n";
  } else if (ir1add + add == 226) {
    L += "        .border-box:nth-child(8) {\n";
    L += "            border: 5px solid #e53935; \n";
    L += "        }\n";
  } else {
    ESP.restart();
  }

  L += "\n";
  L += "        h2 {\n";
  L += "            color: #4285f4;\n";
  L += "        }\n";
  L += "\n";
  L += "        h1 {\n";
  L += "            text-decoration: underline;\n";
  L += "            color: #f5f5f5;\n";
  L += "        }\n";
  L += "\n";
  L += "        @media (max-width: 768px) {\n";
  L += "            .container {\n";
  L += "                width: 90%;\n";
  L += "            }\n";
  L += "        }\n";
  L += "    </style>\n";
  L += "</head>\n";
  L += "\n";
  L += "<body>\n";
  L += "    <header>\n";
  L += "        <h1>Select your IR remote switch for:</h1>\n";
  L += "    </header>\n";
  L += "    <div class=\"container\">\n";
  L += "        <div class=\"border-box\">\n";
  L += "            <h2>Socket-1</h2>\n";
  L += "        </div>\n";
  L += "\n";
  L += "        <div class=\"border-box\">\n";
  L += "            <h2>Socket-2</h2>\n";
  L += "        </div>\n";
  L += "\n";
  L += "        <div class=\"border-box\">\n";
  L += "            <h2>Socket-3</h2>\n";
  L += "        </div>\n";
  L += "\n";
  L += "        <div class=\"border-box\">\n";
  L += "            <h2>Socket-4</h2>\n";
  L += "        </div>\n";
  L += "\n";
  L += "        <div class=\"border-box\">\n";
  L += "            <h2>Configuration</h2>\n";
  L += "        </div>\n";
  L += "\n";
  L += "        <div class=\"border-box\">\n";
  L += "            <h2>Power button</h2>\n";
  L += "        </div>\n";
  L += "\n";
  L += "        <div class=\"border-box\">\n";
  L += "            <h2>Speed Decrement</h2>\n";
  L += "        </div>\n";
  L += "\n";
  L += "        <div class=\"border-box\">\n";
  L += "            <h2>Speed Increment</h2>\n";
  L += "        </div>\n";
  L += "\n";
  L += "    </div>\n";
  L += "</body>\n";
  L += "\n";
  L += "</html>";
  return L;
}


String CONTROL() {
  String L = "<!DOCTYPE html>\n";
  L += "<html lang=\"en\">\n";
  L += "\n";
  L += "<head>\n";
  L += "    <meta charset=\"UTF-8\">\n";
  L += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  L += "    <title>SMART CONTROL</title>\n";
  L += "    <style>\n";
  L += "        html {\n";
  L += "            font-family: Cambria, Cochin, Georgia, Times, 'Times New Roman', serif;\n";
  L += "            display: inline-block;\n";
  L += "            margin: 0px auto;\n";
  L += "            text-align: center;\n";
  L += "            background: linear-gradient(to right, rgb(236, 255, 65), rgb(255, 145, 81));\n";
  L += "        }\n";
  L += "\n";
  L += "        body {\n";
  L += "            margin-top: 5px;\n";
  L += "            justify-content: center;\n";
  L += "            align-items: center;\n";
  L += "            height: 100vh;\n";
  L += "            margin: 0;\n";
  L += "            text-align: center; /* Center text in the body */\n";
  L += "            display: flex;\n";
  L += "            flex-direction: column; /* Set the main axis to vertical */\n";
  L += "        }\n";
  L += "\n";
  L += "        h1 {\n";
  L += "            font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;\n";
  L += "            font-weight: bolder;\n";
  L += "            margin: 2px auto 1px; /* Adjusted margin */\n";
  L += "            font-size: 50px;\n";
  L += "        }\n";
  L += "\n";
  L += "        h3 {\n";
  L += "            color: rgb(38, 11, 46);\n";
  L += "            margin-bottom: 1px;\n";
  L += "            margin-top: 3px;\n";
  L += "        }\n";
  L += "\n";
  L += "        .fb{\n";
  L += "            width: 150px;\n";
  L += "            height: 20px;\n";
  L += "            color: rgb(255, 255, 255);\n";
  L += "            padding: 1px 100px;\n";
  L += "            text-decoration: none;\n";
  L += "            font-size: 15px;\n";
  L += "            margin: 1px auto 5px; /* Adjusted margin */\n";
  L += "            border-radius: 10px;\n";
  L += "            background-color: rgb(7, 4, 255);\n";
  L += "        }\n";
  L += "      .timer {\n";
  L += "          display: inline-block;\n";

  if (socket1ToggleOn == 1 || socket1ToggleOff == 1 || socket2ToggleOn == 1 || socket2ToggleOff == 1 || socket3ToggleOn == 1 || socket3ToggleOff == 1 || socket4ToggleOn == 1 || socket4ToggleOff == 1) {
    L += "          color: #ffffff;\n";
    L += "          background-color: #000000;\n";
  } else {
    L += "          color: #000000;\n";
    L += "          background-color: #ffffff;\n";
  }
  L += "           padding: 10px 15px;\n";
  L += "           font-size: 16px;\n";
  L += "           border: none;\n";
  L += "            width: 300px;\n";
  L += "            height: 20px;\n";
  L += "           border-radius: 5px;\n";
  L += "           text-align: center;\n";
  L += "            text-decoration: none;\n";
  L += "            margin: 30px 10px auto; \n";
  L += "        }\n";
  L += "\n";
  L += "        .button {\n";
  L += "            color: white;\n";
  L += "            border: none;\n";
  L += "            box-shadow: 2px 2px rgb(61, 10, 85);\n";
  L += "            width: 150px;\n";
  L += "            height: 50px;\n";
  L += "            color: rgb(0, 0, 0);\n";
  L += "            padding: 5px 50px;\n";
  L += "            text-decoration: none;\n";
  L += "            font-size: 40px;\n";
  L += "            margin: 1px auto 5px; /* Adjusted margin */\n";
  L += "            border-radius: 10px;\n";
  L += "        }\n";
  L += "\n";
  L += "        .button-on {\n";
  L += "            background-color: rgb(0, 255, 55);\n";
  L += "        }\n";
  L += "\n";
  L += "        .button-on:active {\n";
  L += "            background-color: rgb(113, 255, 120);\n";
  L += "        }\n";
  L += "\n";
  L += "        .button-off {\n";
  L += "            background-color: rgb(255, 51, 51);\n";
  L += "            color: rgb(255, 255, 255);\n";
  L += "        }\n";
  L += "\n";
  L += "        .button-off:active {\n";
  L += "            background-color: rgb(105, 0, 0);\n";
  L += "        }\n";
  L += "        input[type=\"range\"] {\n";
  L += "            width: 60%; /* Adjusted width */\n";
  L += "            margin: 1px auto; /* Adjusted margin */\n";
  L += "        }\n";
  L += "\n";
  L += "        @media only screen and (max-width: 600px) {\n";
  L += "            h1 {\n";
  L += "                font-size: 30px;\n";
  L += "                margin-top: 110px;\n";
  L += "            }\n";
  L += "\n";
  L += "            h3 {\n";
  L += "                font-size: 16px;\n";
  L += "                margin-bottom: 1px;\n";
  L += "                margin-top: 2px;\n";
  L += "            }\n";
  L += "            .fb{\n";
  L += "                width: 160px;\n";
  L += "                height: 15px;\n";
  L += "                padding: 3px 60px;\n";
  L += "            }\n";
  L += "      .timer {\n";
  L += "            margin: 50px 30px  auto; \n";
  L += "            width: 300px;\n";
  L += "            height: 20px;\n";
  L += "        }\n";
  L += "        input[type=\"range\"] {\n";
  L += "            width: 80%; /* Adjusted width */\n";
  L += "            margin: 1px auto; /* Adjusted margin */\n";
  L += "        }\n";
  L += "\n";
  L += "            .button {\n";
  L += "                font-size: 35px;\n";
  L += "                width: 120px;\n";
  L += "                height: 40px;\n";
  L += "                margin: 5px auto 10px; /* Adjusted margin */\n";
  L += "            }\n";
  L += "        }\n";
  L += "    </style>\n";
  L += "</head>\n";
  L += "\n";
  L += "<body>\n";
  L += "    <h1>SMART EXTENSION BOARD</h1>\n";
  L += "    <a class=\"fb\" href=\" https://www.facebook.com/profile.php?id=100076382650422&mibextid=ZbWKwL\">By Rakesh Karmakar</a>\n";
  L += "\n";
  L += "    <h3>SOCKET 1: </h3>\n";
  if (pin1status == true) {
    L += "<a class=\"button button-on\" href=\"/l1on\">ON</a>\n";
  } else {
    L += "<a class=\"button button-off\" href=\"/l1off\">OFF</a>\n";
  }

  L += "    <h3>SOCKET 2: </h3>\n";
  if (pin2status == true) {
    L += "<a class=\"button button-on\" href=\"/l2on\">ON</a>\n";
  } else {
    L += "<a class=\"button button-off\" href=\"/l2off\">OFF</a>\n";
  }
  L += "    <h3>SOCKET 3: </h3>\n";
  if (pin3status == true) {
    L += "<a class=\"button button-on\" href=\"/l3on\">ON</a>\n";
  } else {
    L += "<a class=\"button button-off\" href=\"/l3off\">OFF</a>\n";
  }

  L += "    <h3>SOCKET 4: </h3>\n";
  if (pin4status == true) {
    L += "<a class=\"button button-on\" href=\"/l4on\">ON</a>\n";
  } else {
    L += "<a class=\"button button-off\" href=\"/l4off\">OFF</a>\n";
  }
  L += "<label for=\"slider\"></label>\n";
  L += "<input type=\"range\" id=\"slider\" name=\"slider\" min=\"0\" max=\"100\" value=\"" + value + "\">\n";
  L += "<p>Speed: <span id=\"selectedValue\">" + value + "</span>%</p>\n";
  L += "\n";
  L += "<script>\n";
  L += "  const slider = document.getElementById('slider');\n";
  L += "  const selectedValue = document.getElementById('selectedValue');\n";
  L += "\n";
  L += "  slider.addEventListener('input', () => {\n";
  L += "    selectedValue.textContent = slider.value;\n";
  L += "\n";
  L += "    sendSelectedValue(slider.value);\n";
  L += "  });\n";
  L += "\n";
  L += "  function sendSelectedValue(value) {\n";
  L += "    const xhr = new XMLHttpRequest();\n";
  L += "\n";
  L += "    xhr.open(\"GET\", \"/send-value?value=\" + value, true);\n";
  L += "\n";
  L += "    xhr.send();\n";
  L += "\n";
  L += "    xhr.onload = function() {\n";
  L += "      if (xhr.status === 200) {\n";
  L += "        console.log(\"Server response: \" + xhr.responseText);\n";
  L += "      } else {\n";
  L += "        console.error(\"Error: \" + xhr.status);\n";
  L += "      }\n";
  L += "    };\n";
  L += "  }\n";
  L += "</script>";
  L += " <a class=\"timer\" href=\"/Timer\">Add Timer</a>\n";
  L += "\n";
  L += "</body>\n";
  L += "\n";
  L += "</html>";
  return L;
}
