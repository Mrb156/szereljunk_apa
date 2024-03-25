#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>  // Include the SPIFFS library
#include <Servo.h>

const char *ssid = "APP-ROBI-01";
const char *password = "";
AsyncWebServer server(80);


int Do = 261;
int Re = 293;
int Mi = 329;
int Fa = 349;
int Sol = 391;
int La = 440;
int Si = 493;
int DoS = 277;
int ReS = 311;

int t1 = 160;
int t2 = 80;
int t3 = 50;

#define BUZZER_PIN D1
#define DC_PIN1 D6
#define DC_PIN2 D5
#define SERVO_PIN D8
Servo servo;

int direction = 2;
int forward_speed = 140;
int backward_speed = 140;
int turn_degree = 90;
int prev_turn_degree = 90;

void la_cucaracha() {
  tone (BUZZER_PIN, Do);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Do);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Do);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Fa);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t1);
  tone (BUZZER_PIN, La);
  delay (t1);
  noTone (BUZZER_PIN);
  delay (t1);

  tone (BUZZER_PIN, Do);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Do);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Do);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Fa);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t1);
  tone (BUZZER_PIN, La);
  delay (t1);
  noTone (BUZZER_PIN);
  delay (t1);

  tone (BUZZER_PIN, Fa);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Fa);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Mi);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Mi);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Re);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Re);
  delay (t2);
  noTone (BUZZER_PIN);
  delay (t3);
  tone (BUZZER_PIN, Do);
  delay (t1);
  noTone (BUZZER_PIN);
  delay (t1);
}

void horn_sound() {
  tone(BUZZER_PIN, Do, 100);
  delay(50);
  tone(BUZZER_PIN, Re, 100);
  delay(50);
  tone(BUZZER_PIN, Mi, 100);
  delay(50);
  tone(BUZZER_PIN, Do, 100);
  delay(50);
  tone(BUZZER_PIN, Re, 100);
  delay(50);
  tone(BUZZER_PIN, Mi, 100);
  delay(50);
  tone(BUZZER_PIN, Do, 100);
  delay(50);
  tone(BUZZER_PIN, Re, 100);
  delay(50);
  tone(BUZZER_PIN, Mi, 100);
  delay(50);
}

void init_components() {
  la_cucaracha();
  digitalWrite(DC_PIN1, LOW);
  digitalWrite(DC_PIN2, HIGH);
  delay(100);
  digitalWrite(DC_PIN1, LOW);
  digitalWrite(DC_PIN2, LOW);
  servo.write(1);
  delay(100);
  servo.write(180);
  delay(100);
  servo.write(90);
}
void go_forward() {
  digitalWrite(DC_PIN1, LOW);
  analogWrite(DC_PIN2, forward_speed);
}

void go_backward() {
  digitalWrite(DC_PIN2, LOW);
  analogWrite(DC_PIN1, backward_speed);
}

void stop_motor() {
  digitalWrite(DC_PIN1, LOW);
  digitalWrite(DC_PIN2, LOW);
}

void turn() {
  if (turn_degree != prev_turn_degree) {
    servo.write(turn_degree);
    prev_turn_degree = turn_degree;
  }
}

void forward() {
  Serial.println("Forward");
  direction = 1;
  tone(BUZZER_PIN, Mi, 100);
}

void backward() {
  Serial.println("Backward");
  direction = 0;
  tone(BUZZER_PIN, Do, 100);
}

void stop() {
  Serial.println("stop");
  direction = 2;
  tone(BUZZER_PIN, La, 100);
}
void horn() {
  Serial.println("horn");
  horn_sound();
}


void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(DC_PIN1, OUTPUT);
  pinMode(DC_PIN2, OUTPUT);
  servo.attach(SERVO_PIN);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password, 1, 0, 1);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  init_components();

  //Initialize File System
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS Initialize....ok");
  } else {
    Serial.println("SPIFFS Initialization...failed");
  }

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/start.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/start.html", "text/html");
  });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/settings.html", "text/html");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  server.on("/approbi.gif", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/approbi.gif", "image/gif");
  });

  server.on("/updateForward", HTTP_GET, [](AsyncWebServerRequest * request) {
    int get_speed = request->getParam("speed")->value().toInt();
    forward_speed = get_speed;
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateBackward", HTTP_GET, [](AsyncWebServerRequest * request) {
    int get_speed = request->getParam("speed")->value().toInt();
    backward_speed = get_speed;
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateServo", HTTP_GET, [](AsyncWebServerRequest * request) {
    int get_degree = request->getParam("degree")->value().toInt();
    turn_degree = get_degree;
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateDirection", HTTP_GET, [](AsyncWebServerRequest * request) {
    String dir = request->getParam("direction")->value();
    if (dir == "forward") {
      forward();
    } else if (dir == "backward") {
      backward();
    } else if (dir == "stop") {
      stop();
    } else if (dir == "horn") {
      horn();
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/check_connection", HTTP_GET, handleCheckConnection);

  server.begin();
}
void handleCheckConnection(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "OK");
}
void loop() {
  turn();

  if (direction == 1) {
    go_forward();
  } else if (direction == 0) {
    go_backward();
  } else {
    stop_motor();
  }
}
