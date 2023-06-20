#include "chassis.hpp"
#include "enums.h"

// check equal instead of white/black?

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
