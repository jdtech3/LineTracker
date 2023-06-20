#include "enums.h"
#include "chassis.hpp"

void Chassis::begin(int leftFrontMotorPort, int leftBackMotorPort, int rightFrontMotorPort, int rightBackMotorPort) {
    _motorShield = Adafruit_MotorShield();
    _leftFrontMotor = _motorShield.getMotor(leftFrontMotorPort);
    _leftBackMotor = _motorShield.getMotor(leftBackMotorPort);
    _rightFrontMotor = _motorShield.getMotor(rightFrontMotorPort);
    _rightBackMotor = _motorShield.getMotor(rightBackMotorPort);

    _motorShield.begin();
}

void Chassis::setOne(Adafruit_DCMotor *motor, int speed, int direction) const {
  motor->setSpeed(speed);
  motor->run(direction);
}

void Chassis::setSide(SIDE side, int speed, int direction) {
  if (side == LEFT) {
    setOne(_leftFrontMotor, speed, direction);
    setOne(_leftBackMotor, speed, direction);
  } else if (side == RIGHT) {
    setOne(_rightFrontMotor, speed, direction);
    setOne(_rightBackMotor, speed, direction);
  }
}

void Chassis::setAll(int speed, int direction) {
  setOne(_leftFrontMotor, speed, direction);
  setOne(_leftBackMotor, speed, direction);
  setOne(_rightFrontMotor, speed, direction);
  setOne(_rightBackMotor, speed, direction);
}
