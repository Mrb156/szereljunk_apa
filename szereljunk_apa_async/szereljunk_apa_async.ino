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

int direction = 10;
int forward_speed = 140;
int backward_speed = 140;
int turn_degree = 90;
int prev_turn_degree = 90;

void la_cucaracha() {
  tone(BUZZER_PIN, Do);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Do);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Do);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Fa);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t1);
  tone(BUZZER_PIN, La);
  delay(t1);
  noTone(BUZZER_PIN);
  delay(t1);

  tone(BUZZER_PIN, Do);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Do);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Do);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Fa);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t1);
  tone(BUZZER_PIN, La);
  delay(t1);
  noTone(BUZZER_PIN);
  delay(t1);

  tone(BUZZER_PIN, Fa);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Fa);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Mi);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Mi);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Re);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Re);
  delay(t2);
  noTone(BUZZER_PIN);
  delay(t3);
  tone(BUZZER_PIN, Do);
  delay(t1);
  noTone(BUZZER_PIN);
  delay(t1);
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

void horn() {
  horn_sound();
}

unsigned long previousTime = 0;
bool stop_it = false;

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


  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/start.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/start.html", "text/html");
  });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/settings.html", "text/html");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  server.on("/approbi.gif", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/approbi.gif", "image/gif");
  });

  server.on("/updateForward", HTTP_GET, [](AsyncWebServerRequest *request) {
    int get_speed = request->getParam("speed")->value().toInt();
    forward_speed = get_speed;
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateBackward", HTTP_GET, [](AsyncWebServerRequest *request) {
    int get_speed = request->getParam("speed")->value().toInt();
    backward_speed = get_speed;
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateServo", HTTP_GET, [](AsyncWebServerRequest *request) {
    int get_degree = request->getParam("degree")->value().toInt();
    turn_degree = get_degree;
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateDirection", HTTP_GET, [](AsyncWebServerRequest *request) {
    String dir = request->getParam("direction")->value();
    if (dir == "forward") {
      direction = 1;
      tone(BUZZER_PIN, Mi, 100);
      stop_it = false;
    } else if (dir == "backward") {
      direction = 0;
      tone(BUZZER_PIN, Mi, 100);
      stop_it = false;
    } else if (dir == "stop") {
      stop_motor();
      direction = 2;
      tone(BUZZER_PIN, Mi, 100);
      stop_it = true;
    } else if (dir == "horn") {
      horn();
    } else if (dir == "p1") {
      tone(BUZZER_PIN, La, 100);
      stop_it = false;
      direction = 3;
    } else if (dir == "p2") {
      tone(BUZZER_PIN, La, 100);
      stop_it = false;
      direction = 4;
    } else if (dir == "p3") {
      tone(BUZZER_PIN, La, 100);
      stop_it = false;
      direction = 5;
    } else if (dir == "p4") {
      tone(BUZZER_PIN, La, 100);
      stop_it = false;
      direction = 6;
    }
    request->send(200, "text/plain", "OK");
  });

  server.on("/check_connection", HTTP_GET, handleCheckConnection);

  server.begin();
  previousTime = 0;
}
void handleCheckConnection(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "OK");
}
void loop() {
  unsigned long currentTime = millis();

  if (direction == 1) {
    turn();

    go_forward();
    previousTime = currentTime;

  } else if (direction == 0) {
    turn();

    go_backward();
    previousTime = currentTime;

  } else if (direction == 3 and stop_it == false) {

    if (currentTime - previousTime <= 2000) {
      
      servo.write(90);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 4000) {
      
      servo.write(90);
      digitalWrite(DC_PIN1, 255);
      analogWrite(DC_PIN2, LOW);
    } else if (currentTime - previousTime <= 5000) {
      
      servo.write(180);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 7000) {
      
      servo.write(1);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else {
      
      stop_it = true;
      previousTime = currentTime;
    }
  } else if (direction == 4 and stop_it == false) {

    if (currentTime - previousTime <= 2000) {
      
      servo.write(180);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 4000) {
      
      servo.write(1);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 6000) {
      
      servo.write(180);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 8000) {
      
      servo.write(1);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    }

    else if (currentTime - previousTime <= 10000) {
      
      servo.write(1);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 12000) {
      
      servo.write(180);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 14000) {
      
      servo.write(1);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    }

    else {
      
      stop_it = true;
      previousTime = currentTime;
    }
  } else if (direction == 5 and stop_it == false) {

    if (currentTime - previousTime <= 4000) {
      
      servo.write(180);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 5000) {
      
      servo.write(90);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 9000) {
      
      servo.write(1);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 13000) {
      
      servo.write(90);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else {
      
      stop_it = true;
      previousTime = currentTime;
    }
  } else if (direction == 6 and stop_it == false) {

    if (currentTime - previousTime <= 2000) {
      
      servo.write(90);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 3000) {
      
      servo.write(90);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, LOW);
    } else if (currentTime - previousTime <= 5000) {
      
      servo.write(180);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 6000) {
      
      servo.write(90);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, LOW);
    } else if (currentTime - previousTime <= 8000) {
      
      servo.write(1);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else if (currentTime - previousTime <= 10000) {
      
      servo.write(90);
      digitalWrite(DC_PIN1, LOW);
      analogWrite(DC_PIN2, 255);
    } else {
      
      stop_it = true;
      previousTime = currentTime;
    }
  }

  else {
    turn();

    stop_motor();
    previousTime = currentTime;

    stop_it = true;
  }
}
