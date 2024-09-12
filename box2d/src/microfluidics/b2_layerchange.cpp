//
// Created by Daniel on 08.09.2024.
//

#include "box2d/b2_layerchange.h"
#include <random>

#include "box2d/b2_particle_system.h"
#include "box2d/b2_sensor.h"
std::knuth_b engine;

b2Vec2 RandomPointInCircle(float radius, b2Vec2 center);

void b2LayerChange::Configure(b2ParticleSystem* system) {
    m_system = system;
}

b2Shape* b2LayerChange::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2LayerChange));
    b2LayerChange* clone = new(mem) b2LayerChange;
    *clone = *this;
    return clone;
}

void b2LayerChange::Solve(const std::list<b2ParticleBodyContact>& contacts) {
    float chance = GetLayerShiftProbability();
    std::bernoulli_distribution d(chance);
    for (auto contact : contacts) {
        bool changeLayer = d(engine);
        if (changeLayer) {
            b2Vec2 position = RandomPointInCircle(m_linked->m_radius, m_linked->m_p);
            m_system->MoveParticleToSystem(contact.index, m_linked->m_system, &position);
        }
    }
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

constexpr float MOVEPARTICLEGUARANTEED = 1;

float b2LayerChange::GetLayerShiftProbability() const {
    float pThis = GetAvgPressure();
    float pLinked = m_linked->GetAvgPressure(); //todo use directed force
    if (pThis <= pLinked)
        return 0;
    float diff = pThis - pLinked;
    return 1 / MOVEPARTICLEGUARANTEED * diff;
}
bool IsMovingTowards(const b2Vec2& velocity, const b2Vec2& norm);
float b2LayerChange::GetAvgPressure() const {
    if (connectedChannels.empty())
        return 0;
    // float avgPressure = 0;
    // for (const b2Sensor* sensor : connectedChannels) {
    //     avgPressure += sensor->GetAvgPressure();
    // }
    // return avgPressure / connectedChannels.size();
    float avgPressure = 0;
    for (const b2Sensor* sensor : connectedChannels) {
        b2Vec2 sensorCenter = sensor->m_vertex2 + (sensor->m_vertex1 - sensor->m_vertex2) / 2;
        b2Vec2 normFromSensorToThis = sensorCenter - m_p;
        b2Vec2 pressure=*sensor->GetAvgDirectionalPressure();
        //project pressure onto norm
        float dot=b2Dot(pressure,normFromSensorToThis);
        float lensquared = b2Dot(normFromSensorToThis, normFromSensorToThis);
        if (lensquared <= b2_epsilon) {
            return 0;
        }
        float projectionScalar = dot/lensquared;
        b2Vec2 projected=projectionScalar*normFromSensorToThis;
        if (IsMovingTowards(projected, normFromSensorToThis)) {
            avgPressure += projected.Length();
        }
    }
    return avgPressure / connectedChannels.size();
}
bool IsMovingTowards(const b2Vec2& velocity, const b2Vec2& norm)
{
    // Calculate dot product between velocity and toTarget
    float dotProduct = b2Dot(velocity, norm);

    // If dot product is positive, it's moving towards the target, else away
    return dotProduct > 0;
}