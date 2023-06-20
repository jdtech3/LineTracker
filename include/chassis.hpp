#include <Adafruit_MotorShield.h>

#include "enums.h"

class Chassis {
    public:
        void setOne(Adafruit_DCMotor *motor, int speed, int direction) const;
        void setSide(SIDE side, int speed, int direction);
        void setAll(int speed, int direction);
        void begin(int leftFrontMotorPin, int leftBackMotorPin, int rightFrontMotorPin, int rightBackMotorPin);
    private:
        Adafruit_MotorShield _motorShield;
        Adafruit_DCMotor *_leftFrontMotor;
        Adafruit_DCMotor *_leftBackMotor;
        Adafruit_DCMotor *_rightFrontMotor;
        Adafruit_DCMotor *_rightBackMotor;
};
