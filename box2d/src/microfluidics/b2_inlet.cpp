//
// Created by Daniel on 02.09.2024.
//

#include "box2d/b2_inlet.h"
#include "box2d/b2_block_allocator.h"
#include "box2d/b2_particle_system.h"
#include "box2d/b2_world.h"

b2Inlet* b2Inlet::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2Inlet));
    b2Inlet* clone = new(mem) b2Inlet;
    *clone = *this;
    return clone;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void b2Inlet::Solve(std::list<b2ParticleBodyContact>& contacts) {
    if (IsActive()) {
        //apply force
        for (b2ParticleBodyContact contact : contacts) {
            m_system->ParticleApplyForce(contact.index, force);
            // m_system->GetVelocityBuffer()[contact.index]= force;// msystem->particleapplyforce?
        }
        //summon new particles
        b2Vec2 normal = summoningLine2 - summoningLine1;
        normal.Normalize();
        float lineLength = (summoningLine1 - summoningLine2).Length();
        int32 particleCount = ceil(lineLength / (m_system->GetDef().radius * 2));
        float distanceBetweenParticles = lineLength / particleCount;
        std::list<b2Vec2> points;
        for (int i = 0; i < particleCount; ++i) {
            points.push_back(summoningLine1 + normal * (m_system->GetDef().radius + i * distanceBetweenParticles));
        }
        for (b2Vec2 point : points) {
            if (TestParticlePoint(point,contacts)) {
                b2ParticleDef def = *m_particleDef;
                def.position = point;
                m_system->CreateParticleUnlocked(def);
            }
        }
    }
}
bool b2Inlet::TestParticlePoint(b2Vec2& point, std::list<b2ParticleBodyContact>& contacts) {
    float minDistance = m_system->GetDef().radius;
    for (b2ParticleBodyContact contact : contacts) {
        b2Vec2 posA= m_system->GetPositionBuffer()[contact.index];
        if ((point-posA).Length() < minDistance)//todo if potential force is greater than m_force.Length
            return false;
    }
    return true;
}