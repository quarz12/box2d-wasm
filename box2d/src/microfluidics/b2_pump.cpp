//
// Created by Daniel on 25.05.2024.
//
//can move to .h
#include "box2d/b2_pump.h"

b2Pump::b2Pump(const b2Vec2& force) {
    is_pump = true;
    pumpForce = force;
};