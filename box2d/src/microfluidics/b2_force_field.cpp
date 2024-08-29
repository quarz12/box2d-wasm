//
// Created by Daniel on 28.08.2024.
//

#include "box2d/b2_force_field.h"

void b2ForceField::Solve(b2TimeStep &step, std::list<b2ParticleBodyContact> &contacts) {
    if (m_isTimed && m_remainingDuration < 0)
        return;
    


    m_remainingDuration-=step.dt;
}
