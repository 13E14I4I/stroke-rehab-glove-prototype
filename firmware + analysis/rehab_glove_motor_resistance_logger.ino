/*
  Rehab Glove Motor Resistance Logger

  Measures voltage across a conductive rubber/flex sensor element in a voltage
  divider circuit, calculates resistance, and logs time, voltage, and resistance
  data through the Arduino Serial Monitor.

  The script also controls a continuous-rotation servo motor through a predefined
  motion sequence to simulate glove-assisted movement trials.

  Serial output format:
  time_s,voltage_v,resistance_ohms
*/

#include <Servo.h>

Servo servo1;

#define STOP_TIME 30  // Total trial duration in seconds

const int pinNumber = 5;  // Servo motor control signal pin

// Motor direction and speed sequence.
// Valid directions: "cw", "ccw", "stop"
// Valid speeds: "slow", "fast", "na"
String mtr_motions[9][2] = {
  {"cw", "slow"},
  {"ccw", "fast"},
  {"cw", "fast"},
  {"ccw", "slow"},
  {"stop", "na"},
  {"cw", "slow"},
  {"ccw", "slow"},
  {"cw", "fast"},
  {"ccw", "fast"}
};

// End time for each motor motion segment, in seconds.
// The final value should match STOP_TIME.
int mtr_stop_times[9] = {3, 7, 12, 15, 17, 20, 22, 25, 30};

const int NUM_MOTIONS = 9;
int j = 0;  // Current motor motion index

int RubberPin = A0;                // Analog input pin for voltage across the rubber/flex element
const int Resistor = 1000;         // Fixed resistor value in the voltage divider circuit
const int sample_interval = 1000;  // Time between resistance measurements, in milliseconds

float R_voltage;     // Measured voltage across the rubber/flex element
float R_resistance;  // Calculated resistance of the rubber/flex element
float sample_time;   // Sample time in seconds

int i = 0;              // Counter used to calculate sample time
bool written = false;   // True once all data has been output to the serial monitor
bool motor_done = false; // True once the motor sequence has finished


void setup() {
  Serial.begin(9600);

  servo1.attach(pinNumber);

  Serial.print('\n');
  Serial.println("time_s,voltage_v,resistance_ohms");
}


// Reads voltage across the rubber/flex element, calculates resistance,
// outputs CSV-formatted data, and updates the servo motion sequence.
void loop() {

  // Update servo motion sequence while the trial is still running.
  if (!motor_done && millis() <= STOP_TIME * 1000UL) {

    if (j < NUM_MOTIONS) {
      servo1.attach(pinNumber);
      move_mtr(mtr_motions[j][0], mtr_motions[j][1]);

      if (millis() > mtr_stop_times[j] * 1000UL) {
        j++;
        servo1.detach();
      }
    } else {
      servo1.detach();
      motor_done = true;
    }
  }

  // Stop the motor once the trial duration has ended.
  if (millis() > STOP_TIME * 1000UL) {
    servo1.detach();
    motor_done = true;
  }

  // Process sensor values.
  R_voltage = analogRead(RubberPin) * (5.0 / 1023.0);
  R_resistance = Resistor * R_voltage / (5 - R_voltage);
  sample_time = sample_interval * i / 1000.0;

  // Print calculated values to the serial monitor in CSV format.
  if (!written) {
    Serial.print(sample_time, 2);
    Serial.print(',');
    Serial.print(R_voltage, 2);
    Serial.print(',');
    Serial.println(R_resistance, 2);

    if (sample_time >= STOP_TIME) {
      written = true;
    }
  }

  i++;
  delay(sample_interval);
}


// Rotates the motor in different directions and speeds based on the
// predefined values from mtr_motions and mtr_stop_times.
void move_mtr(String direction, String speed) {

  if (direction == "cw") {
    if (speed == "slow") {
      servo1.write(93);
    } else if (speed == "fast") {
      servo1.write(30);
    } else {
      Serial.println("ERROR: incorrect speed input");
    }

  } else if (direction == "ccw") {
    if (speed == "slow") {
      servo1.write(96);
    } else if (speed == "fast") {
      servo1.write(160);
    } else {
      Serial.println("ERROR: incorrect speed input");
    }

  } else if (direction == "stop") {
    servo1.detach();

  } else {
    Serial.println("ERROR: incorrect direction input");
  }
}