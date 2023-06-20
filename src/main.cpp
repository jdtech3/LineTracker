#include <stdlib.h>

#include <Arduino.h>
#include <Wire.h>
#include <hp_BH1750.h>
#include <SPI.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>

#include "constants.h"
#include "enums.h"
#include "chassis.hpp"
#include "controller.hpp"

hp_BH1750 lightSensor;
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Chassis chassis;

// Modes

MODE mode = STOP;
BLE_INPUT_MODE bleInputMode = COMMAND;
char lastCmd[65];

// Tuning params

int hardSpeed = 510;
int softSpeed = 0;

// -- High level

void forward() {
  move(chassis, 0);
}
void stop() {
  chassis.setAll(0, FORWARD);
}

void hardLeft() {
  move(chassis, -hardSpeed);
}
void hardRight() {
  move(chassis, hardSpeed);
}

void softLeft() {
  move(chassis, -softSpeed);
}
void softRight() {
  move(chassis, softSpeed);
}

// -- Main --

void setup() {
  chassis.begin(LEFT_FRONT_MOTOR_PORT, LEFT_BACK_MOTOR_PORT, RIGHT_FRONT_MOTOR_PORT, RIGHT_BACK_MOTOR_PORT);
  stop();

  lightSensor.begin(BH1750_TO_GROUND);

  #ifdef DEBUG
    // while (!Serial);
    delay(2500);

    Serial.begin(9600);
    Serial.println("JDLineFollow - DEBUG enabled!");
  #endif

  #ifdef BLUETOOTH
    if (!ble.begin()) {
      #ifdef DEBUG
        Serial.println("BLE init failed!");
      #endif
    }
    #ifdef DEBUG
      Serial.println("BLE init OK!");
      ble.info();
    #endif

    #ifdef DEBUG
      Serial.println("Waiting for BLE connect...");
    #endif
    ble.println("AT+GAPDEVNAME=JDBot");
    ble.println("ATZ");
    while (!ble.isConnected());
    #ifdef DEBUG
      Serial.println("BLE connected!");
    #endif
  #endif

  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void loop() {
  #ifdef BLUETOOTH
    ble.println("AT+BLEUARTRX");
    ble.readline();

    if (strcmp(ble.buffer, "OK") != 0) {
      #ifdef DEBUG
        Serial.print("[BLE Recv] ");
        Serial.println(ble.buffer);
      #endif

      if (bleInputMode == COMMAND) {
        if (strcmp(ble.buffer, "G") == 0) {
          mode = GO;
          ble.print("AT+BLEUARTTX=");
          ble.println("ACK: GO");
        } else if (strcmp(ble.buffer, "S") == 0) {
          mode = STOP;
          ble.print("AT+BLEUARTTX=");
          ble.println("ACK: STOP");
        } else if (strcmp(ble.buffer, "CH") == 0) {
          bleInputMode = DATA;
          strcpy(lastCmd, ble.buffer);
          ble.print("AT+BLEUARTTX=");
          ble.print("ACK: CONF HARD, CUR: ");
          ble.println(hardSpeed);
        } else if (strcmp(ble.buffer, "CS") == 0) {
          bleInputMode = DATA;
          strcpy(lastCmd, ble.buffer);
          ble.print("AT+BLEUARTTX=");
          ble.print("ACK: CONF SOFT, CUR: ");
          ble.println(softSpeed);
        }
      }
      else {
        if (strcmp(lastCmd, "CH") == 0) {
          hardSpeed = atoi(ble.buffer);
          ble.print("AT+BLEUARTTX=");
          ble.print("ACK: CH ");
          ble.println(hardSpeed);

          #ifdef DEBUG
            Serial.print("New hardSpeed: ");
            Serial.println(hardSpeed);
          #endif
        } else if (strcmp(lastCmd, "CS") == 0) {
          softSpeed = atoi(ble.buffer);
          ble.print("AT+BLEUARTTX=");
          ble.print("ACK: CS ");
          ble.println(softSpeed);

          #ifdef DEBUG
            Serial.print("New softSpeed: ");
            Serial.println(softSpeed);
          #endif
        }
        bleInputMode = COMMAND;
      }
    }
  #endif

  if (mode == GO) {

    bool llIR = analogRead(LL_IR_PIN) > IR_THRESHOLD;     // true: black detected
    bool lIR = analogRead(L_IR_PIN) > IR_THRESHOLD;
    bool rIR = analogRead(R_IR_PIN) > IR_THRESHOLD;
    bool rrIR = analogRead(RR_IR_PIN) > IR_THRESHOLD;

    /* Possible combos: (1 is black)

    1000: hard left
    1100: left
    0110: forward
    0011: right
    0001: hard right

    0000: stop
    1111: forward (trap?)

    0100: forward (idk)
    0010: forward (idk)

    1110: hard left
    0111: hard right

    */

    if (llIR && !lIR && !rIR && !rrIR) {
      hardLeft();
    } else if (llIR && lIR && !rIR && !rrIR) {
      softLeft();
    } else if (!llIR && lIR && rIR && !rrIR) {
      forward();
    } else if (!llIR && !lIR && rIR && rrIR) {
      softRight();
    } else if (!llIR && !lIR && !rIR && rrIR) {
      hardRight();
    }

    else if (!llIR && !lIR && !rIR && !rrIR) {
      // stop();
    } else if (llIR && lIR && rIR && rrIR) {
      forward();
    }

    else if (!llIR && lIR && !rIR && !rrIR) {
      forward();
    } else if (!llIR && !lIR && rIR && !rrIR) {
      forward();
    }

    else if (llIR && lIR && rIR && !rrIR) {
      hardLeft();
    } else if (!llIR && lIR && rIR && rrIR) {
      hardRight();
    }

    else {
      stop();
    }

  } else {
    stop();

    lightSensor.start();
    int lux = lightSensor.getLux();

    if (lux > LUX_THRESHOLD) mode = GO;
  }
}
