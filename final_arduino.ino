#include <Servo.h> // Include the Servo library

// Create servo objects
Servo baseServo;
Servo arm1Servo;
Servo arm2Servo;
Servo cameraServo;
Servo gripper;

// Define sensor pins
const int sensor1 = 24; // Channel 1 connected to pin 24
const int sensor2 = 2;
const int sensor3 = 3;
const int sensor4 = 4;
const int sensor5 = 5;
const int sensor6 = 6;
const int sensor7 = 7;
const int sensor8 = 8;

// Define motor control pins
const int motor1Dir = 11;
const int motor1PWM = 9;
const int motor2Dir = 12;
const int motor2PWM = 10;

// Define servo pins
const int BASE_PIN = 40;
const int ARM1_PIN = 44;
const int ARM2_PIN = 46;
const int CAMERA_SERVO_PIN = 50;
const int gripperPin = 42;

// Define resting positions
// const int BASE_REST = 70;
// const int ARM1_REST = 160;
// const int ARM2_REST = 180;
const int CAMERA_DOWN = 45;
const int CAMERA_FORWARD = 90;

// Variable to track gripper position
int gripperAngle = 0;


void moveServoSmoothly(Servo &servo, int startAngle, int targetAngle, int delayTime) {
    if (startAngle < targetAngle) {
        for (int angle = startAngle; angle <= targetAngle; angle++) {
            servo.write(angle);
            delay(delayTime);
        }
    } else {
        for (int angle = startAngle; angle >= targetAngle; angle--) {
            servo.write(angle);
            delay(delayTime);
        }
    }
}


void setup() {
    Serial.begin(9600);

    // Attach servos to pins
    baseServo.attach(BASE_PIN);
    arm1Servo.attach(ARM1_PIN);
    arm2Servo.attach(ARM2_PIN);
    
    cameraServo.attach(CAMERA_SERVO_PIN);
    gripper.attach(gripperPin);

    // Move servos to resting positions
   
    baseServo.write(82);
    arm1Servo.write(163);
    arm2Servo.write(180);
    cameraServo.write(CAMERA_DOWN);
    gripper.write(0);

    // restingPosition();

    delay(3000); // Wait for servos to reach position

    // Initialize IR sensor pins
    pinMode(sensor1, INPUT);
    pinMode(sensor2, INPUT);
    pinMode(sensor3, INPUT);
    pinMode(sensor4, INPUT);
    pinMode(sensor5, INPUT);
    pinMode(sensor6, INPUT);
    pinMode(sensor7, INPUT);
    pinMode(sensor8, INPUT);

    // Initialize Motor Driver pins
    pinMode(motor1Dir, OUTPUT);
    pinMode(motor1PWM, OUTPUT);
    pinMode(motor2Dir, OUTPUT);
    pinMode(motor2PWM, OUTPUT);

   
}

void loop() {
    // Check for serial input from Python
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        // Serial.print("Received command: ");
        // Serial.println(command);

        if (command == "STOP") {
            stopMotors();
            Serial.println("Marker Detected: Robot Stopped");
        }
         else if (command == "TURN_SEQUENCE") {
            stopMotors();
            cameraServo.write(CAMERA_FORWARD);
            delay(2000);
            executeTurnSequence();
        }
        else if (command == "OBDETECTED"){
            pickupPosition();
            delay(3000);
            pickPosition();
            delay(6000);
            pickupPosition();
            delay(3000);
            drop();
            delay(5000);
            restingPosition2();
            delay(4000);

            executeTurnbackSequence();
        }
    }

    // Read sensor values
    int s1 = digitalRead(sensor1);
    int s2 = digitalRead(sensor2);
    int s3 = digitalRead(sensor3);
    int s4 = digitalRead(sensor4);
    int s5 = digitalRead(sensor5);
    int s6 = digitalRead(sensor6);
    int s7 = digitalRead(sensor7);
    int s8 = digitalRead(sensor8);

    // Line following logic
    if (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 1 && s6 == 1 && s7 == 1 && s8 == 1) {
        stopMotors();
    } else if (s4 == 1 && s5 == 1) {
        moveForward();
    } else if ((s4 == 1 && s5 == 1) && (s3 == 1 || s6 == 1)) {
        moveForward();
    } else if ((s6 == 0 && s2 == 1) || (s5 == 0 && s1 == 1) || (s4 == 1 && s5 == 0)) {
        turnRight();
    } else if ((s3 == 0 && s7 == 1) || (s4 == 0 && s8 == 1) || (s5 == 1 && s4 == 0)) {
        turnLeft();
    } else {
        stopMotors();
    }
}

void executeTurnSequence() {
    digitalWrite(motor1Dir, HIGH);
    analogWrite(motor1PWM, 50);
    digitalWrite(motor2Dir, LOW);
    analogWrite(motor2PWM, 50);
    delay(3000);

    stopMotors();
    delay(8000);

    Serial.println("READY");
    delay(5000);
}



void executeTurnbackSequence(){
    digitalWrite(motor1Dir, LOW);
    analogWrite(motor1PWM, 50);
    digitalWrite(motor2Dir, HIGH);
    analogWrite(motor2PWM, 50);
    delay(3000);

    stopMotors();
    delay(2000);
    cameraServo.write(CAMERA_DOWN);
    delay(4000);

//     Serial.println("READY");
//     delay(4000);
// 

}

void moveForward() {
    digitalWrite(motor1Dir, LOW);
    analogWrite(motor1PWM, 50);
    digitalWrite(motor2Dir, LOW);
    analogWrite(motor2PWM, 50);
}

void turnRight() {
    digitalWrite(motor1Dir, LOW);
    analogWrite(motor1PWM, 50);
    digitalWrite(motor2Dir, LOW);
    analogWrite(motor2PWM, 0);
}

void turnLeft() {
    digitalWrite(motor1Dir, LOW);
    analogWrite(motor1PWM, 0);
    digitalWrite(motor2Dir, LOW);
    analogWrite(motor2PWM, 50);
}

void stopMotors() {
    digitalWrite(motor1Dir, LOW);
    analogWrite(motor1PWM, 0);
    digitalWrite(motor2Dir, LOW);
    analogWrite(motor2PWM, 0);
}
// Predefined positions

void pickupPosition() {
    Serial.println("Moving to pickup position...");
    moveServoSmoothly(baseServo, baseServo.read(), 130, 30);
    moveServoSmoothly(arm2Servo, arm2Servo.read(), 120, 30);
    moveServoSmoothly(arm1Servo, arm1Servo.read(), 170, 30);
}

void pickPosition() {
    Serial.println("Moving to pick ...");
    // Claw open 
    moveServoSmoothly(baseServo, baseServo.read(), 110, 30);
    moveServoSmoothly(arm2Servo, arm2Servo.read(), 120, 30);
    moveServoSmoothly(arm1Servo, arm1Servo.read(), 170, 30);
    delay(2000);
    moveServoSmoothly(gripper, gripper.read(), 40, 10);
    gripperAngle = 40;
    
    
}


void drop() {
    Serial.println("Moving to drop position...");
    moveServoSmoothly(arm2Servo, arm2Servo.read(), 10, 30);
    moveServoSmoothly(baseServo, baseServo.read(), 60, 30);
    moveServoSmoothly(arm1Servo, arm1Servo.read(), 20, 30);
    moveServoSmoothly(baseServo, baseServo.read(), 45, 30);
    moveServoSmoothly(gripper, gripper.read(), 0, 10);
    gripperAngle = 0;
    
}

void restingPosition2() {
    Serial.println("Moving to resting position...");
    moveServoSmoothly(arm2Servo, arm2Servo.read(),180, 30);
    moveServoSmoothly(baseServo, baseServo.read(), 95, 30);
    moveServoSmoothly(arm1Servo, arm1Servo.read(), 150, 30);
    moveServoSmoothly(baseServo, baseServo.read(), 82, 30);
    moveServoSmoothly(arm1Servo, arm1Servo.read(), 163, 30);
}
