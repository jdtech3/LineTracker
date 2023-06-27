#include "controller.hpp"

// check equal instead of white/black?

#define TARGET 0

int error = 0;
long i = 0;
long d = 0;

// move forward/turn
// range: -510 (max left turn) to 0 (forward) to 510 (max right turn)
void move(Chassis chassis, int direction) {
    // forward
    if (direction == 0) {
        chassis.setAll(255, FORWARD);
    }

    // right turn
    else if (direction > 0) {
        if (direction > 255) {
            chassis.setSide(RIGHT, direction - 255, BACKWARD);
        } else {
            chassis.setSide(RIGHT, 255 - direction, FORWARD);
        }

        chassis.setSide(LEFT, 255, FORWARD);
    }

    // left turn
    else if (direction < 0) {
        direction = -direction;

        if (direction > 255) {
            chassis.setSide(LEFT, direction - 255, BACKWARD);
        } else {
            chassis.setSide(LEFT, 255 - direction, FORWARD);
        }

        chassis.setSide(RIGHT, 255, FORWARD);
    }
}

// where is the line?
// range: -3 (line is on left) to +3 (line is on right)
int sense() {
    bool llIR = analogRead(LL_IR_PIN) > IR_THRESHOLD;     // true: black detected
    bool lIR = analogRead(L_IR_PIN) > IR_THRESHOLD;
    bool rIR = analogRead(R_IR_PIN) > IR_THRESHOLD;
    bool rrIR = analogRead(RR_IR_PIN) > IR_THRESHOLD;

    /* Possible combos: (1 is black)

    1000: -3
    1100: -2
    0110: 0
    0011: 2
    0001: 3

    0000: 0
    1111: 0

    0100: -1
    0010: 1

    1110: -3
    0111: 3

    otherwise: 0

    */

    if (llIR && !lIR && !rIR && !rrIR) {
      return -3;
    } else if (llIR && lIR && !rIR && !rrIR) {
      return -2;
    } else if (!llIR && lIR && rIR && !rrIR) {
      return 0;
    } else if (!llIR && !lIR && rIR && rrIR) {
      return 2;
    } else if (!llIR && !lIR && !rIR && rrIR) {
      return 3;
    }

    else if (!llIR && !lIR && !rIR && !rrIR) {
      return 0;
    } else if (llIR && lIR && rIR && rrIR) {
      return 0;
    }

    else if (!llIR && lIR && !rIR && !rrIR) {
      return -1;
    } else if (!llIR && !lIR && rIR && !rrIR) {
      return 1;
    }

    else if (llIR && lIR && rIR && !rrIR) {
      return -3;
    } else if (!llIR && lIR && rIR && rrIR) {
      return 3;
    }

    else {
      return 0;
    }
}

// run the control loop
void tick(Chassis chassis, int kp, int ki, int kd) {
    int newError = sense() - TARGET;
    i += newError;
    d = newError - error;
    
    int direction = kp*newError + ki*i + kd*d;

    move(chassis, direction);

    // Serial.print("Error:");
    // Serial.print(newError);
    // Serial.print(",I:");
    // Serial.print(i);
    // Serial.print(",D:");
    // Serial.print(d);
    // Serial.print(",kp:");
    // Serial.print(kp);
    // Serial.print(",ki:");
    // Serial.print(ki);
    // Serial.print(",kd:");
    // Serial.print(kd);
    // Serial.print(",OUTPUT:");
    // Serial.println(direction);
}
