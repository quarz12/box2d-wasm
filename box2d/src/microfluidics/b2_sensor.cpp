//
// Created by Daniel on 22.08.2024.
//

#include "box2d/b2_sensor.h"
#include "box2d/b2_body.h"

void b2Sensor::SensePressure(b2TimeStep& step, std::list<b2ParticleBodyContact> &contacts) {
    float value=CalculateTheoreticalPressure(step,contacts).Length();
    pressureSamples.push_back(value);
    if (pressureSamples.size() > intervalTimeSteps && intervalTimeSteps > 0)
        pressureSamples.pop_front();
}
//TODO divide by amount of contacts
void b2Sensor::SenseSpeed(b2TimeStep& step, std::list<b2ParticleBodyContact> &contacts) {
    b2Vec2 velocity;//TODO use actual movement
    for (b2ParticleBodyContact contact: contacts) {
        velocity+=m_system->GetVelocityBuffer()[contact.index];
    }
    float value=velocity.Length();
    speedSamples.push_back(value);
    if (speedSamples.size() > intervalTimeSteps && intervalTimeSteps > 0)
        speedSamples.pop_front();
}

void b2Sensor::Solve(b2TimeStep& step, std::list<b2ParticleBodyContact> &contacts) {
    if (IsPressureSensor()) {
        SensePressure(step, contacts);
    }
    if (IsSpeedSensor()) {
        SenseSpeed(step, contacts);
    }
}

float b2Sensor::GetAvgPressure() {
    if (pressureSamples.empty())
        return 0;
    return std::accumulate(pressureSamples.begin(), pressureSamples.end(), 0.0) / pressureSamples.size();
}

float b2Sensor::GetAvgSpeed() {
    if (speedSamples.empty())
        return 0;
    return std::accumulate(speedSamples.begin(), speedSamples.end(), 0.0) / speedSamples.size();
}

b2Vec2 b2Sensor::CalculateTheoreticalPressure(b2TimeStep step, std::list<b2ParticleBodyContact>& contacts) const {
    float criticalPressure = m_system->GetCriticalPressure(step);
    float pressurePerWeight = m_system->GetDef().pressureStrength * criticalPressure;
    // applies pressure between each particle & body in contact
    float velocityPerPressure = step.dt / (m_system->GetDef().density * m_system->m_particleDiameter);
    b2Vec2 pressure;
    for (b2ParticleBodyContact contact: contacts) {
        int32 a = contact.index;
        b2Body *b = contact.body;
        float w = contact.weight;
        float m = contact.mass;
        b2Vec2 n = contact.normal;
//        b2Vec2 p = m_system->GetPositionBuffer()[a];
        float h = pressurePerWeight * w;
        b2Vec2 f = velocityPerPressure * w * m * h * n;
//        b2Vec2 force = -m_system->GetParticleInvMass() * f;
//        m_velocityBuffer.data[a] -= GetParticleInvMass() * f;   //recoil particle
        pressure+=f;
//        b->ApplyLinearImpulse(f, p, true);  only take f for now, ignores point of pressure
    }
    return pressure;
}


void b2Valve::Solve(b2TimeStep& step, std::list<b2ParticleBodyContact> &contacts) {
    this->b2Sensor::Solve(step, contacts); //todo verify
    if (pressureSamples.size() > intervalTimeSteps && GetAvgPressure() > threshold) {
        if (gate->isOpen())
            gate->close();
    } else {
        if (!gate->isOpen())
            gate->open();
    }
}

void b2Gate::open() {
    //TODO
    isClosed = false;
}

void b2Gate::close() {
    //TODO
    isClosed = true;
}
