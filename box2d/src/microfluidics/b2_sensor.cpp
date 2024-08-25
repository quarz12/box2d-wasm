//
// Created by Daniel on 22.08.2024.
//

#include "box2d/b2_sensor.h"
#include "box2d/b2_body.h"

void b2Sensor::SensePressure(b2TimeStep &step, std::list<b2ParticleBodyContact> &contacts) {
    //todo nicht particlebodycontacts sondern particle particle für particles in kontakt mit body
    float pressure = CalculateTheoreticalAvgPressure(step, contacts).Length();
    pressureSamples.push_back(pressure);
    if (pressureSamples.size() > intervalTimeSteps && intervalTimeSteps > 0)
        pressureSamples.pop_front();
}

void b2Sensor::SenseSpeed(b2TimeStep &step, std::list<b2ParticleBodyContact> &contacts) {
    b2Vec2 velocity;
    for (b2ParticleBodyContact contact: contacts) {
//        print("-----------------------------------------------");
//        print(m_system->GetVelocityBuffer()[contact.index].ToString());
//        print((m_system->GetVelocityBuffer()[contact.index]*step.dt).ToString());
        velocity += m_system->GetVelocityBuffer()[contact.index];
    }
    float avgSpeed = contacts.size() > 0 ? velocity.Length() / contacts.size() : 0;
    speedSamples.push_back(avgSpeed);
    if (speedSamples.size() > intervalTimeSteps && intervalTimeSteps > 0)
        speedSamples.pop_front();
}

void b2Sensor::Solve(b2TimeStep &step, std::list<b2ParticleBodyContact> &contacts) {
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

b2Vec2 b2Sensor::CalculateTheoreticalAvgPressure(b2TimeStep step, std::list<b2ParticleBodyContact> &contacts) const {
    unsigned long length = contacts.size();
    if (length == 0)
        return b2Vec2_zero;
    float criticalPressure = m_system->GetCriticalPressure(step);
    float pressurePerWeight = m_system->GetDef().pressureStrength * criticalPressure;
    // applies pressure between each particle & body in contact
    float velocityPerPressure = step.dt / (m_system->GetDef().density * m_system->m_particleDiameter);
    b2Vec2 pressure;
    for (b2ParticleBodyContact contact: contacts) {
//        int32 a = contact.index;
//        b2Body *b = contact.body;
        float w = contact.weight;
        float m = contact.mass;
        b2Vec2 n = contact.normal;
//        b2Vec2 p = m_system->GetPositionBuffer()[a];
        float h = pressurePerWeight * w;
        b2Vec2 f = velocityPerPressure * w * m * h * n;
//        b2Vec2 force = -m_system->GetParticleInvMass() * f;
//        m_velocityBuffer.data[a] -= GetParticleInvMass() * f;   //recoil particle
        pressure += f;
//        b->ApplyLinearImpulse(f, p, true);  only take f for now, ignores point of pressure
    }
    return pressure / length;
}


void b2Valve::Solve(b2TimeStep &step, std::list<b2ParticleBodyContact> &contacts) {
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
