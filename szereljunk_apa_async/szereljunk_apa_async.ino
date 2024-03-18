#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>  // Include the SPIFFS library
#include <Servo.h>

const char *ssid = "NodeServer";
const char *password = "";
AsyncWebServer server(80);
#define BUZZER_PIN D1
#define DC_PIN1 D6
#define DC_PIN2 D5
#define SERVO_PIN D8
Servo servo;

int direction = 2;
int speed = 175;
String steer = "straight";
String cur_steer = "straight";

void init_components() {
  tone(BUZZER_PIN, 1000, 500);
  digitalWrite(DC_PIN1, LOW);
  digitalWrite(DC_PIN2, HIGH);
  delay(100);
  digitalWrite(DC_PIN1, LOW);
  digitalWrite(DC_PIN2, LOW);
}
void go_forward() {
  digitalWrite(DC_PIN1, LOW);
  analogWrite(DC_PIN2, speed);
}

void go_backward() {
  digitalWrite(DC_PIN2, LOW);
  analogWrite(DC_PIN1, speed);
}

void stop_motor() {
  digitalWrite(DC_PIN1, LOW);
  digitalWrite(DC_PIN2, LOW);
}

void turn(int degree) {
  servo.write(degree);
}

void forward() {
  Serial.println("Forward");
  direction = 1;
}

void backward() {
  Serial.println("Backward");
  direction = 0;
}

void left() {
  Serial.println("Left");
  if (steer == "right") {
    steer = "straight";
  } else {
    steer = "left";
  }
}

void right() {
  Serial.println("Right");
  if (steer == "left") {
    steer = "straight";
  } else {
    steer = "right";
  }
}

void stop() {
  Serial.println("stop");
  direction = 2;
}
void horn() {
  Serial.println("horn");
  tone(BUZZER_PIN, 1000, 100);
}

String fhtml;

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(DC_PIN1, OUTPUT);
  pinMode(DC_PIN2, OUTPUT);
  servo.attach(SERVO_PIN);
  servo.write(90);


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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/updateSpeed", HTTP_GET, [](AsyncWebServerRequest * request) {
    int get_speed = request->getParam("speed")->value().toInt();
    speed = get_speed;
  });

  server.on("/updateDirection", HTTP_GET, [](AsyncWebServerRequest * request) {
    String dir = request->getParam("direction")->value();
    if (dir == "forward") {
      forward();
    } else if (dir == "backward") {
      backward();
    } else if (dir == "right") {
      right();
    } else if (dir == "left") {
      left();
    } else if (dir == "stop") {
      stop();
    } else if (dir == "horn") {
      horn();
    }
  });

  server.on("/check_connection", HTTP_GET, handleCheckConnection);

  server.begin();
}
void handleCheckConnection(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "1");
}
void loop() {
  if (steer == "left") {
    turn(180);
  }
  else if (steer == "right") {
    turn(1);
  }
  else {
    turn(90);
  }

  if (direction == 1) {
    go_forward();
  } else if (direction == 0) {
    go_backward();
  } else {
    stop_motor();
  }
}
