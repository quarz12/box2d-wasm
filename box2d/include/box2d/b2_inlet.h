//
// Created by Daniel on 02.09.2024.
//

#ifndef BOX2D_B2_INLET_H
#define BOX2D_B2_INLET_H


#include "b2_polygon_shape.h"
#include <list>

struct b2ParticleBodyContact;
class b2ParticleSystem;
struct b2ParticleDef;

class b2Inlet : public b2PolygonShape {
public:
    b2Vec2 force, summoningLine1, summoningLine2;
    b2ParticleSystem* m_system;
    b2ParticleDef* m_particleDef;

    b2Inlet() {
        m_isObserver = true;
        isInlet = true;
    }

    inline void Configure(b2ParticleSystem* system, b2ParticleDef* partDef, b2Vec2& directedPressure, b2Vec2& point1,
                          b2Vec2& point2) { //todo channge dircectedPressure to norm + pressure(float)
        m_system = system;
        m_particleDef = partDef;
        force = directedPressure;
        summoningLine1 = point1;
        summoningLine2 = point2;
    }

    b2Inlet* Clone(b2BlockAllocator* allocator) const override;

    /// true if particle can be summoned at point, false if too much overlap
    bool TestParticlePoint(b2Vec2& point, std::list<b2ParticleBodyContact>& contacts);

    float weightToDistance(float weight) const;

    void Solve(std::list<b2ParticleBodyContact>& contacts);

    inline void Activate() { active = true; }
    inline void Deactivate() { active = false; }
    inline bool IsActive() const { return active; }
    inline void SetPressure(b2Vec2& p) { force = p; }

    inline b2Inlet* AsInlet() override { return this; }

private:
    bool active = false;
};

#endif //BOX2D_B2_INLET_H
