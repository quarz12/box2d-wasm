//
// Created by Daniel on 28.08.2024.
//

#include "box2d/b2_force_field.h"
#include "box2d/b2_particle_system.h"

void b2ForceField::Solve(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts) {
    if (m_isTimed && m_remainingDuration < 0)
        return;
    for (b2ParticleBodyContact contact : contacts) {
        m_system->ParticleApplyForce(contact.index, m_force);
//        print("applied"+m_force.ToString());
    }
    m_remainingDuration -= step.dt;
}

b2ForceField* b2ForceField::Clone(b2BlockAllocator* allocator) const {

    void* mem = allocator->Allocate(sizeof(b2ForceField));
    b2ForceField* clone = new(mem) b2ForceField;
    *clone = *this;
    return clone;

}
