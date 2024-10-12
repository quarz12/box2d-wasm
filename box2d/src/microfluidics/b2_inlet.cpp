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

void b2Inlet::Solve(std::list<b2ParticleBodyContact>& contacts) const {
    if (IsActive()) {
        //apply force
        for (b2ParticleBodyContact contact : contacts) {
            b2Vec2 awayFromCenter=m_system->GetPositionBuffer()[contact.index]-m_p;
            awayFromCenter.Normalize();
            // print((force*awayFromCenter).ToString());
            // m_system->GetForceBuffer()[contact.index]= force*awayFromCenter;
            m_system->ParticleApplyForce(contact.index, force*awayFromCenter);//input with fixed pressure
            // m_system->GetVelocityBuffer()[contact.index]= force;// input with fixed velocity
        }
        //summon new particles
        float summoningRadius= m_radius - m_system->GetDef()->radius*1.5;
        float circumference= 2 * b2_pi * summoningRadius;
        int32 numParticles = floor(circumference/m_system->m_particleDiameter);
        float angle = 2 * b2_pi / numParticles;
        std::list<b2Vec2> points;
        for (int i = 0; i < numParticles; ++i) {
            b2Vec2 position;
            position.x=m_p.x+summoningRadius*cos(i*angle);
            position.y=m_p.y+summoningRadius*sin(i*angle);
            points.push_back(position);
        }
        for (b2Vec2 point : points) {
            if (TestParticlePoint(point,contacts)) {
                b2ParticleDef def = *m_particleDef;
                def.position = point;
                int32 index=m_system->CreateParticleUnlocked(def);
                b2Vec2 awayFromCenter=m_system->GetPositionBuffer()[index]-m_p;
                awayFromCenter.Normalize();
                // m_system->ParticleApplyForce(index,awayFromCenter*force);
            }
        }
    }
}
bool b2Inlet::TestParticlePoint(b2Vec2& point, std::list<b2ParticleBodyContact>& contacts) const {
    for (auto contact : contacts) {
        // Calculate the squared distance between the point and the contact
        b2Vec2 posA=m_system->GetPositionBuffer()[contact.index];
        float distanceSquared = (point - posA ).LengthSquared();

        // Check if the distance is less than or equal to 4 * radius^2 (since both circles have the same radius)
        if (distanceSquared <= 4 * m_system->GetDef()->radius * m_system->GetDef()->radius) {
            return false;  // Overlap detected
        }
    }

    return true;  // No overlap
    // float minDistance = m_system->GetDef().radius;
    // for (b2ParticleBodyContact contact : contacts) {
    //     b2Vec2 posA= m_system->GetPositionBuffer()[contact.index];
    //     if ((point-posA).Length() < minDistance)//
    //         return false;
    // }
    // return true;
}

float b2Inlet::weightToDistance(float weight) const {
    return -weight*m_system->m_particleDiameter+m_system->m_particleDiameter;
}