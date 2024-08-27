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

b2Vec2 b2Sensor::CalculateTheoreticalAvgPressure(b2TimeStep step, std::list<b2ParticleBodyContact> &observations) const {
    unsigned long length = observations.size();
    if (length == 0)
        return b2Vec2_zero;
    std::list<int32> particles;
    for (b2ParticleBodyContact observation: observations) {
        particles.push_back(observation.index);
    }
    std::list<b2ParticleContact> contacts;
    auto allContacts=m_system->GetContacts();
    for (int32 particleIndex: particles) {
        for (int i = 0; i < m_system->GetContactCount(); ++i) {
            b2ParticleContact contact = allContacts[i];
            if (contact.GetIndexA()==particleIndex||contact.GetIndexB()==particleIndex)
                contacts.push_back(contact);
        }
    }
    b2Vec2 pressure;
    std::map<int32,float> map;
    float velocityPerPressure = step.dt / (m_system->GetDef().density * m_system->m_particleDiameter);
    for (auto contact:contacts) {
        int32 a = contact.GetIndexA();
        int32 b = contact.GetIndexB();
        float w = contact.GetWeight();
        b2Vec2 n = contact.GetNormal();
        float h = map[a] + map[b];
        b2Vec2 f = velocityPerPressure * w * h * n;
        if (contains(particles,a))
            pressure -= f;
        if (contains(particles,b))
            pressure += f;
    }
    return pressure;
}

void b2Sensor::SetTwoSided(const b2Vec2& v1, const b2Vec2& v2)
{
    m_vertex1 = v1;
    m_vertex2 = v2;
}

bool b2Sensor::TestPoint(const b2Transform& xf, const b2Vec2& p) const
{
    B2_NOT_USED(xf);
    B2_NOT_USED(p);
    return false;
}

void b2Sensor::ComputeDistance(const b2Transform& xf, const b2Vec2& p, float* distance, b2Vec2* normal, int32 childIndex) const
{
    B2_NOT_USED(childIndex);

    b2Vec2 v1 = b2Mul(xf, m_vertex1);
    b2Vec2 v2 = b2Mul(xf, m_vertex2);

    b2Vec2 d = p - v1;
    b2Vec2 s = v2 - v1;
    float ds = b2Dot(d, s);
    if (ds > 0)
    {
        float s2 = b2Dot(s, s);
        if (ds > s2)
        {
            d = p - v2;
        }
        else
        {
            d -= ds / s2 * s;
        }
    }

    float d1 = d.Length();
    *distance = d1;
    *normal = d1 > 0 ? 1 / d1 * d : b2Vec2_zero;

}

// p = p1 + t * d
// v = v1 + s * e
// p1 + t * d = v1 + s * e
// s * e - t * d = p1 - v1
bool b2Sensor::RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
                          const b2Transform& xf, int32 childIndex) const
{
    B2_NOT_USED(childIndex);

    // Put the ray into the edge's frame of reference.
    b2Vec2 p1 = b2MulT(xf.q, input.p1 - xf.p);
    b2Vec2 p2 = b2MulT(xf.q, input.p2 - xf.p);
    b2Vec2 d = p2 - p1;

    b2Vec2 v1 = m_vertex1;
    b2Vec2 v2 = m_vertex2;
    b2Vec2 e = v2 - v1;

    // Normal points to the right, looking from v1 at v2
    b2Vec2 normal(e.y, -e.x);
    normal.Normalize();

    // q = p1 + t * d
    // dot(normal, q - v1) = 0
    // dot(normal, p1 - v1) + t * dot(normal, d) = 0
    float numerator = b2Dot(normal, v1 - p1);

    float denominator = b2Dot(normal, d);

    if (denominator == 0.0f)
    {
        return false;
    }

    float t = numerator / denominator;  //intersection fraction of ray
    if (t < 0.0f || input.maxFraction < t)
    {
        return false;
    }

    b2Vec2 q = p1 + t * d;  //intersection point on edge

    // q = v1 + s * r
    // s = dot(q - v1, r) / dot(r, r)
    b2Vec2 r = v2 - v1;
    float rr = b2Dot(r, r);
    if (rr == 0.0f)
    {
        return false;
    }

    float s = b2Dot(q - v1, r) / rr;    //is q on the edge?
    if (s < 0.0f || s > 1.0f )  // is s not between 0,1
    {
        return false;
    }

    output->fraction = t;
    if (numerator > 0.0f)
    {
        output->normal = -b2Mul(xf.q, normal);
    }
    else
    {
        output->normal = b2Mul(xf.q, normal);
    }
    return true;
}

void b2Sensor::ComputeAABB(b2AABB* aabb, const b2Transform& xf, int32 childIndex) const
{
    B2_NOT_USED(childIndex);

    b2Vec2 v1 = b2Mul(xf, m_vertex1);
    b2Vec2 v2 = b2Mul(xf, m_vertex2);

    b2Vec2 lower = b2Min(v1, v2);
    b2Vec2 upper = b2Max(v1, v2);

    aabb->lowerBound = lower;
    aabb->upperBound = upper;
}

void b2Sensor::ComputeMass(b2MassData* massData, float density) const
{
    B2_NOT_USED(density);

    massData->mass = 0.0f;
    massData->center = 0.5f * (m_vertex1 + m_vertex2);
    massData->I = 0.0f;
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
    m_hasCollision= false;
    isClosed = false;
}

void b2Gate::close() {
    //TODO
    m_hasCollision= true;
    isClosed = true;
}
