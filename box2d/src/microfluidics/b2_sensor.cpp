//
// Created by Daniel on 22.08.2024.
//

#include "box2d/b2_sensor.h"
#include "box2d/b2_body.h"
#include "box2d/b2_particle_system.h"

void b2Sensor::SensePressure(b2TimeStep &step, std::list<b2ParticleBodyContact> &contacts) {
    float sample = CalculateTheoreticalAvgPressure(step, contacts);
    pressureSamples.push_back(sample);
    avg_pressure+=sample/intervalTimeSteps;
    if (pressureSamples.size() > intervalTimeSteps && intervalTimeSteps > 0) {
        avg_pressure-=pressureSamples.front()/intervalTimeSteps;
        pressureSamples.pop_front();
    }
}

void b2Sensor::SenseSpeed(b2TimeStep &step, std::list<b2ParticleBodyContact> &contacts) {
    b2Vec2 velocity;
    print("-----------------------------------------------");
    for (b2ParticleBodyContact contact: contacts) {
        print(m_system->GetVelocityBuffer()[contact.index].ToString());
//        print((m_system->GetVelocityBuffer()[contact.index]*step.dt).ToString());
        velocity += m_system->GetVelocityBuffer()[contact.index];
    }
    print("-----------------------------------------------");
    float sample = !contacts.empty() ? velocity.Length() / contacts.size() : 0;
    print("speed: "+ str(sample));
    speedSamples.push_back(sample);
    avg_speed+=sample/intervalTimeSteps;
    if (speedSamples.size() > intervalTimeSteps && intervalTimeSteps > 0){
        avg_speed-=speedSamples.front()/intervalTimeSteps;
        speedSamples.pop_front();
    }
}

void b2Sensor::Solve(b2TimeStep &step, std::list<b2ParticleBodyContact> &contacts) {
    if (IsPressureSensor()) {
        SensePressure(step, contacts);
    }
    if (IsSpeedSensor()) {
        SenseSpeed(step, contacts);
    }
}

float b2Sensor::GetAvgPressure() const {
    if (pressureSamples.size()==intervalTimeSteps)
        return avg_pressure;
    else return 0;
}

float b2Sensor::GetAvgSpeed() const {
    if (speedSamples.size()==intervalTimeSteps)
        return avg_speed;
    else return 0;
}

float b2Sensor::CalculateTheoreticalAvgPressure(b2TimeStep step, std::list<b2ParticleBodyContact> &observations) const {
    unsigned long length = observations.size();
    if (length == 0)
        return 0;
    std::list<int32> particles;
    for (b2ParticleBodyContact observation: observations) {
        particles.push_back(observation.index);
    }
    std::list<b2ParticleContact> contacts;
    auto allContacts=m_system->GetContacts();
    for (int32 particleIndex: particles) {
        for (int i = 0; i < m_system->GetContactCount(); ++i) {
            b2ParticleContact contact = allContacts[i];
            if (contact.GetIndexA()==particleIndex||contact.GetIndexB()==particleIndex) {
                contacts.push_back(contact);
            }
        }
    }
    //remove duplicates
    contacts.sort([](const b2ParticleContact& a, b2ParticleContact& b)->bool {
        if (a.GetIndexA() == b.GetIndexA()) {
            return a.GetIndexB() < b.GetIndexB();
        }
        return a.GetIndexA() < b.GetIndexA();
    });
    contacts.unique();
//    for (auto c:contacts) {
//        print("found "+str(c.GetIndexA())+" "+str(c.GetIndexB()));
//    }
    std::map<int32,float> accBuffer;
    std::map<int32,float> pressureBuffer;
    auto staticPressureBuffer=m_system->GetStaticPressureBuffer();
    for (int i = 0; i < m_system->GetParticleCount(); ++i) {
        if (contains(particles, i))
            accBuffer[i]=staticPressureBuffer[i];
    }
    float velocityPerPressure = step.dt / (m_system->GetDef().density * m_system->m_particleDiameter);
    for (auto contact:contacts) {
        int32 a = contact.GetIndexA();
        int32 b = contact.GetIndexB();
        float w = contact.GetWeight();
        b2Vec2 n = contact.GetNormal();
        float h = accBuffer[a] + accBuffer[b];
        b2Vec2 f = velocityPerPressure * w * h * n;
        if (contains(particles,a))
            pressureBuffer[a] += f.Length();
        if (contains(particles,b))
            pressureBuffer[b] += f.Length();
    }
    float pressure = 0;
    for (auto KVPair:pressureBuffer) {
        pressure+=KVPair.second;
    }
    return pressure/pressureBuffer.size();
}

b2Shape *b2Sensor::Clone(b2BlockAllocator *allocator) const {
    {
        void* mem = allocator->Allocate(sizeof(b2Sensor));
        b2Sensor* clone = new (mem) b2Sensor;
        *clone = *this;
        return clone;
    }
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

b2Shape *b2Valve::Clone(b2BlockAllocator *allocator) const {
    {
        void* mem = allocator->Allocate(sizeof(b2Valve));
        b2Valve* clone = new (mem) b2Valve;
        *clone = *this;
        return clone;
    }
}

void b2Gate::open() {
    //TODO
    m_hasCollision= false;
    isClosed = false;
}

void b2Gate::close() {
    //TODO
    m_hasCollision= true;
    isClosed = true;
}
