//
// Created by Daniel on 08.09.2024.
//

#include "box2d/b2_layerchange.h"
#include <random>

#include "box2d/b2_particle_system.h"

b2Vec2 RandomPointInCircle(float radius, b2Vec2 center);
void b2LayerChange::Configure(b2ParticleSystem* system) {
    m_system = system;
//todo
}

b2Shape* b2LayerChange::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2LayerChange));
    b2LayerChange* clone = new(mem) b2LayerChange;
    *clone = *this;
    return clone;
}

void b2LayerChange::Solve(const std::list<b2ParticleBodyContact>& contacts) {
//todo
    //m_system->MoveParticleToSystem()
}

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
