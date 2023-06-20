#ifndef _CONTROLLER_HPP
#define _CONTROLLER_HPP


#include "chassis.hpp"
#include "enums.h"

void move(Chassis chassis, int direction);
int sense();
void tick(Chassis chassis, int kp, int ki, int kd);


#endif  // #ifndef _CONTROLLER_HPP
