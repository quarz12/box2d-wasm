//
// Created by Daniel on 08.09.2024.
//

#include "box2d/b2_layerchange.h"
#include <random>
#include "box2d/b2_sensor.h"
#include "box2d/b2_particle_system.h"
std::knuth_b engine;

b2Vec2 RandomPointInCircle(float radius, b2Vec2 center);

void b2LayerChange::Configure(b2ParticleSystem* system, b2CircleSensor* sensor) {
    m_system = system;
    m_sensor=sensor;
}

b2Shape* b2LayerChange::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2LayerChange));
    b2LayerChange* clone = new(mem) b2LayerChange;
    *clone = *this;
    return clone;
}

void b2LayerChange::Solve(const std::list<b2ParticleBodyContact>& contacts) const {
    if (debug)
        return;
    float chance = GetLayerShiftProbability();
    std::bernoulli_distribution randomizer(chance);
    if (debug) {
        print("thispressure = "+str(GetPressure()));
        print("linkedPressure = "+str(m_linked->GetPressure()));
    }
    if (GetPressure() > m_linked->GetPressure()) { //if pressure is higher than linked
        b2AABB aabb;
        b2Transform transform;
        transform.SetIdentity();
        m_linked->ComputeAABB(&aabb,transform,0);
        std::list<int32> particlesInLinkedArea=m_linked->m_system->FindParticlesInAABB(aabb);
        std::list<b2Vec2> particlePositionsInLinkedArea;
        for (auto particleIndex : particlesInLinkedArea) {
            particlePositionsInLinkedArea.push_back(m_linked->m_system->GetPositionBuffer()[particleIndex]);
        }
        for (auto particleContact : contacts) {
            if (m_system->GetLayerChangeDelayBuffer()[particleContact.index] <= 0) {
                bool shouldChangeLayer = randomizer(engine);
                if (shouldChangeLayer) {
                    b2Vec2 position = RandomPointInCircle(m_linked->m_radius-m_system->m_particleDiameter/2*1.5, m_linked->m_p);
                    int32 repeats = 0;
                    float radius=m_system->GetDef()->radius;
                    bool positionIsCleared = m_linked->TestPointForShift(position, radius, particlePositionsInLinkedArea);
                    while (!positionIsCleared && repeats < 5) {
                        repeats++;
                        position = RandomPointInCircle(m_linked->m_radius-m_system->m_particleDiameter/2*1.5, m_linked->m_p);
                        positionIsCleared = m_linked->TestPointForShift(position, radius, particlePositionsInLinkedArea);
                    }
                    if (positionIsCleared) {
                        int32 index=m_system->MoveParticleToSystem(particleContact.index, m_linked->m_system, &position);
                        b2Vec2 force=m_linked->ForceAway(position,GetPressure());
                        m_linked->m_system->ParticleApplyForce(index,force);
                        particlePositionsInLinkedArea.push_back(position);
                    }
                }
            }
        }
    } else {
        // move particles out of area
        for (auto particleContact : contacts) {
            // print("pushing away");
            b2Vec2 force = ForceAway(m_system->GetPositionBuffer()[particleContact.index],m_linked->GetPressure());
            // print("force "+force.ToString());
            m_system->ParticleApplyForce(particleContact.index, force);
        }
    }
}
///force to push particle on position away from the center
b2Vec2 b2LayerChange::ForceAway(const b2Vec2& position, float force) const {
    b2Vec2 directionAwayFromCenter = position-m_p;
    directionAwayFromCenter.Normalize();
    return directionAwayFromCenter*force;
}
///biased towards center
b2Vec2 RandomPointInCircle(float radius, b2Vec2 center) {
    float theta = random() / (RAND_MAX / (2 * M_PI));
    // Generate a random radius with uniform distribution within the circle
    float pRadius = random() / (RAND_MAX / radius);

    // Convert polar coordinates to Cartesian coordinates
    float x = pRadius * cos(theta);
    float y = pRadius * sin(theta);

    // Translate the point to the circle's center
    return b2Vec2(x, y) + center;
}

constexpr float MOVEPARTICLEGUARANTEED = 1;

float b2LayerChange::GetLayerShiftProbability() const {
    float pThis = GetPressure();
    float pLinked = m_linked->GetPressure();
    if (pThis <= pLinked)
        return 0;
    float diff = pThis - pLinked;
    if (diff >= pLinked * 0.1) //force diff must be significant
        return 1 / (MOVEPARTICLEGUARANTEED * diff);
    return 0;
}

bool b2LayerChange::TestPointForShift(b2Vec2& pos, float radius, const std::list<b2Vec2>& particlesInLinked) {
    for (b2Vec2 particlePosition : particlesInLinked) {
        if ((particlePosition - pos).Length() < radius*2) {
            return false;
        }
    }//todo too strong? pointtest inaccurate?
    return true;
}
