//
// Created by Daniel on 25.05.2024.
//

#pragma once

#include "b2_polygon_shape.h"

class b2Pump : public b2PolygonShape {
public:
    explicit b2Pump(const b2Vec2& force){
        m_hasCollision = false;
        m_isPump= true;
        pumpForce = force;
    };

};
