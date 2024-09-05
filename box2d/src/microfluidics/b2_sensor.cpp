//
// Created by Daniel on 22.08.2024.
//

#include "box2d/b2_sensor.h"
#include "box2d/b2_body.h"
#include "box2d/b2_particle_system.h"
#include "box2d/b2_force_field.h"
#include "box2d/b2_fixture.h"

void b2Sensor::SensePressure(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts) {
    float sample = CalculateTheoreticalAvgPressure(step, contacts);
    pressureSamples.push_back(sample);
    avg_pressure += sample / intervalTimeSteps;
    if (pressureSamples.size() > intervalTimeSteps && intervalTimeSteps > 0) {
        avg_pressure -= pressureSamples.front() / intervalTimeSteps;
        pressureSamples.pop_front();
    }
}

void b2Sensor::SenseSpeed(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts) {
    b2Vec2 velocity;
    for (b2ParticleBodyContact contact : contacts) {
        velocity += m_system->GetVelocityBuffer()[contact.index];
    }
    float sample = !contacts.empty() ? velocity.Length() / contacts.size() : 0;
    speedSamples.push_back(sample);
    avg_speed += sample / intervalTimeSteps;
    if (speedSamples.size() > intervalTimeSteps && intervalTimeSteps > 0) {
        avg_speed -= speedSamples.front() / intervalTimeSteps;
        speedSamples.pop_front();
    }
}

void b2Sensor::Solve(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts) {
    if (IsPressureSensor()) {
        SensePressure(step, contacts);
    }
    if (IsSpeedSensor()) {
        SenseSpeed(step, contacts);
    }
}

float b2Sensor::GetAvgPressure() const {
    if (pressureSamples.size() == intervalTimeSteps)
        return avg_pressure;
    return 0;
}

float b2Sensor::GetAvgSpeed() const {
    if (speedSamples.size() == intervalTimeSteps)
        return avg_speed;
    return 0;
}

float b2Sensor::CalculateTheoreticalAvgPressure(b2TimeStep step, std::list<b2ParticleBodyContact>& observations) const {
    if (observations.empty())
        return 0;
    std::list<int32> particles;
    for (b2ParticleBodyContact observation : observations) {
        particles.push_back(observation.index);
    }
    std::list<b2ParticleContact> contacts;
    auto allContacts = m_system->GetContacts();
    for (int32 particleIndex : particles) {
        for (int i = 0; i < m_system->GetContactCount(); ++i) {
            b2ParticleContact contact = allContacts[i];
            if (contact.GetIndexA() == particleIndex || contact.GetIndexB() == particleIndex) {
                contacts.push_back(contact);
            }
        }
    }
    if (contacts.empty())
        return 0;
    //remove duplicates
    contacts.sort([](const b2ParticleContact& a, b2ParticleContact& b) -> bool {
        if (a.GetIndexA() == b.GetIndexA()) {
            return a.GetIndexB() < b.GetIndexB();
        }
        return a.GetIndexA() < b.GetIndexA();
    });
    contacts.unique();
    std::map<int32, float> accBuffer;
    std::map<int32, float> pressureBuffer;
    auto staticPressureBuffer = m_system->GetStaticPressureBuffer();
    for (int i = 0; i < m_system->GetParticleCount(); ++i) {
        if (contains(particles, i))
            accBuffer[i] = staticPressureBuffer[i];
    }
    float velocityPerPressure = step.dt / (m_system->GetDef().density * m_system->m_particleDiameter);
    for (auto contact : contacts) {
        int32 a = contact.GetIndexA();
        int32 b = contact.GetIndexB();
        float w = contact.GetWeight();
        b2Vec2 n = contact.GetNormal();
        float h = accBuffer[a] + accBuffer[b];
        b2Vec2 f = velocityPerPressure * w * h * n;
        if (contains(particles, a))
            pressureBuffer[a] += f.Length();
        if (contains(particles, b))
            pressureBuffer[b] += f.Length();
    }
    float pressure = 0;
    for (auto KVPair : pressureBuffer) {
        pressure += KVPair.second;
    }
    return pressure / pressureBuffer.size();
}

b2Shape* b2Sensor::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2Sensor));
    b2Sensor* clone = new(mem) b2Sensor;
    *clone = *this;
    return clone;
}


void b2Valve::Update() {
    if (pressureSamples.size() >= intervalTimeSteps && GetAvgPressure() > m_threshold) {
        if (gate->IsOpen()) {
            gate->Close();
            if (m_hasForceField) {
                if (m_ff1 != nullptr)
                    m_ff1->Activate();
                if (m_ff2 != nullptr)
                    m_ff2->Activate();
            }
        }
    } else {
        if (!gate->IsOpen()) {
            gate->Open();
            if (m_hasForceField) {
                if (m_ff1 != nullptr)
                    m_ff1->Deactivate();
                if (m_ff2 != nullptr)
                    m_ff2->Deactivate();
            }
        }
    }
}

b2Shape* b2Valve::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2Valve));
    b2Valve* clone = new(mem) b2Valve;
    *clone = *this;
    return clone;
}

void b2Valve::Configure(b2ParticleSystem* system, int32 intervalSteps, b2Gate* connectedGate, float threshold) {
    this->b2Sensor::Configure(false, true, system, intervalSteps);
    gate = connectedGate;
    m_threshold = threshold;
}

void b2Valve::SetForceField(float forceStrength, bool isTimed, b2Body* body, b2ParticleSystem* system) {
    m_hasForceField = true;
    float radius = m_system->GetDef().radius;
    b2ForceField ff1, ff2;
    b2Vec2 surfaceTangent = gate->m_vertex1 - gate->m_vertex2;
    b2Vec2 orthogonal;
    orthogonal.x = -surfaceTangent.y;
    orthogonal.y = surfaceTangent.x;
    orthogonal.Normalize();
    float width, height, distanceToGate;
    width = radius;
    height = (gate->m_vertex1 - gate->m_vertex2).Length();
    b2Vec2 gateCenter = gate->m_vertex2 + (gate->m_vertex1 - gate->m_vertex2) / 2;
    b2Vec2 invertedForce = -orthogonal * forceStrength;
    b2Vec2 force = orthogonal * forceStrength;
    distanceToGate = radius;
    float angle= b2Atan2(orthogonal.y,orthogonal.x);
    { //ff1
        b2Vec2 ff1Center = gateCenter + orthogonal * (distanceToGate + width);
        ff1.SetAsBox(width / 2, height / 2, ff1Center, angle);
        if (isTimed)
            ff1.Configure(force, true, 0.2, system);
        else
            ff1.Configure(force, false,0,system);
        b2Fixture* fixture = body->CreateFixture(&ff1, 0);
        m_ff1 = fixture->GetShape()->AsForceField();
        m_ff1->Deactivate();
    }
    { //ff2
        b2Vec2 ff2Center = gateCenter - orthogonal * (distanceToGate + width);
        ff2.SetAsBox(width / 2, height / 2, ff2Center, angle);
        if (isTimed)
            ff2.Configure(invertedForce, true, 0.2, system);
        else
            ff2.Configure(force, false,0,system);
        b2Fixture* fixture = body->CreateFixture(&ff2, 0);
        m_ff2 = fixture->GetShape()->AsForceField();
        m_ff2->Deactivate();
    }
}

void b2Gate::Open() {
    m_hasCollision = false;
    isClosed = false;
}

void b2Gate::Close() {
    m_hasCollision = true;
    isClosed = true;
}

b2Gate* b2Gate::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2Gate));
    b2Gate* clone = new(mem) b2Gate;
    *clone = *this;
    return clone;

}
