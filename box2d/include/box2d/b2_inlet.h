//
// Created by Daniel on 02.09.2024.
//

#ifndef BOX2D_B2_INLET_H
#define BOX2D_B2_INLET_H


#include <list>

#include "b2_circle_shape.h"

struct b2ParticleBodyContact;
class b2ParticleSystem;
struct b2ParticleDef;

class b2Inlet : public b2CircleShape {
public:
    float force;
    b2ParticleSystem* m_system;
    b2ParticleDef* m_particleDef;

    b2Inlet() {
        m_isObserver = true;
        isInlet = true;
    }

    inline void Configure(b2ParticleSystem* system, b2ParticleDef* partDef, const float pressure) {
        m_system = system;
        m_particleDef = partDef;
        force = pressure;
    }

    b2Inlet* Clone(b2BlockAllocator* allocator) const override;

    /// true if particle can be summoned at point, false if too much overlap
    bool TestParticlePoint(b2Vec2& point, std::list<b2ParticleBodyContact>& contacts) const;

    float weightToDistance(float weight) const;

    void Solve(std::list<b2ParticleBodyContact>& contacts) const;

    inline void Activate() { active = true; }
    inline void Deactivate() { active = false; }
    inline bool IsActive() const { return active; }
    inline void SetPressure(const float p) { force = p; }

    inline b2Inlet* AsInlet() override { return this; }

private:
    bool active = false;
};

#endif //BOX2D_B2_INLET_H
