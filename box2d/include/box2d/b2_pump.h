//
// Created by Daniel on 25.05.2024.
//

#ifndef BOX2D_B2_PUMP_H
#define BOX2D_B2_PUMP_H

#include "b2_polygon_shape.h"

class b2Pump : public b2PolygonShape {
public:
    explicit b2Pump(const b2Vec2&);

};

#endif //BOX2D_B2_PUMP_H

