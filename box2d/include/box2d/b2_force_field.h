//
// Created by Daniel on 28.08.2024.
//

#ifndef BOX2D_B2_FORCE_FIELD_H
#define BOX2D_B2_FORCE_FIELD_H


#include "b2_polygon_shape.h"
#include "b2_time_step.h"

struct b2ParticleBodyContact; //forward decl
class b2ParticleSystem;

class b2ForceField : public b2PolygonShape {
public:
    b2Vec2 m_force;
    float m_totalDuration;
    float m_remainingDuration;
    bool m_isTimed;
    b2ParticleSystem* m_system;

    b2ForceField() {
        m_isObserver = true;
        isForceField = true;
    }

    void Configure(b2Vec2& force, bool isTimed, float duration, b2ParticleSystem* system) {
        m_force = force;
        m_isTimed = isTimed;
        m_totalDuration = duration;
        m_remainingDuration = duration;
        m_system = system;
    }

    inline void ResetTimer() {
        m_remainingDuration = m_totalDuration;
    }

    void Solve(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts);

    b2ForceField* AsForceField() override {return (b2ForceField*) this;};

    b2ForceField* Clone(b2BlockAllocator* allocator) const override;
};


#endif //BOX2D_B2_FORCE_FIELD_H
