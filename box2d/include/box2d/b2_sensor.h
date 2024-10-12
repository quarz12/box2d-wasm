//
// Created by Daniel on 22.08.2024.
//

#ifndef BOX2D_B2SENSOR_H
#define BOX2D_B2SENSOR_H
#include "b2_edge_shape.h"
#include "b2_time_step.h"
#include <list>

#include "b2_arc_shape.h"
struct b2ParticleBodyContact;
class b2ParticleSystem;
class b2Valve;
class b2ForceField;
class b2Body;

class b2Sensor : public b2EdgeShape {
public:
    bool pressureSensor = false;
    bool speedSensor = false;
    float avg_pressure = 0;
    float avg_speed = 0;
    b2Vec2 avg_force = b2Vec2_zero;
    std::list<float> pressureSamples{}, speedSamples{};
    int32 intervalTimeSteps;
    b2ParticleSystem* m_system;
    bool isValve = false;

    b2Sensor() {
        isSensor = true;
        m_isObserver = true;
        m_radius = 0.0f;
    };

    inline void Configure(bool speed, bool pressure, b2ParticleSystem* system, int32 intervalSteps) {
        speedSensor = speed;
        pressureSensor = pressure;
        intervalTimeSteps = intervalSteps;
        m_system = system;
        if (m_system == nullptr) {
            print("system is nullptr");
        };
    }

    void SensePressure(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts);

    void SenseSpeed(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts);

    ///in Pascal
    float GetAvgPressure() const;

    float GetAvgSpeed() const;

    float CalculatePressure(b2TimeStep& step, std::list<b2ParticleBodyContact>& observations) const;

    void Solve(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts);

    bool IsPressureSensor() const { return pressureSensor; };
    bool IsSpeedSensor() const { return speedSensor; };
    inline float Length() const { return (m_vertex1 - m_vertex2).Length(); };

    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    inline b2Sensor* GetNext() const { return m_next; }
    inline void SetNext(b2Sensor* next) { m_next = next; }

    b2Sensor* AsSensor() override { return (b2Sensor*) this; };

    inline int32 GetSampleSize() const { return pressureSamples.size(); };

    virtual inline b2Valve* AsValve() { return nullptr; };
    bool debug=false;
#if LIQUIDFUN_EXTERNAL_LANGUAGE_API
    /// Set this as an isolated edge, with direct floats.
	void Set(float vx1, float vy1, float vx2, float vy2);
#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API

private:
    b2Sensor* m_next = nullptr;
};

class b2Gate : public b2EdgeShape {
public:
    b2Gate() {
        isGate = true;
        isClosed = false;
        m_hasCollision = false;
    };
    inline bool IsOpen() const { return !isClosed; }

    void Open();

    void Close();

    b2Gate* Clone(b2BlockAllocator* allocator) const override;

    inline b2Gate* AsGate() override { return this; };

private:
    bool isClosed;
};

class b2Valve : public b2Sensor {
public:
    b2Valve() {
        isValve = true;
    }

    void Configure(b2ParticleSystem* system, int32 intervalSteps, b2Gate* connectedGate, float threshold);

    b2Gate* gate;
    float m_threshold = 0;
    bool m_hasForceField = false;
    b2ForceField* m_ff1;
    b2ForceField* m_ff2;

    void SetForceField(float forceStrength, bool isTimed, b2Body* body, b2ParticleSystem* system);

    void SetForceField(b2ForceField* ff1, b2ForceField* ff2) {
        m_hasForceField = true;
        m_ff1 = ff1;
        m_ff2 = ff2;
    }

    inline void SetGate(b2Gate* g) { gate = g; };

    inline void SetThreshold(float pressure) {
        m_threshold = pressure;
    };

    void Update() const;

    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    inline b2Valve* AsValve() override { return this; }
};

class b2CircleSensor : public b2ArcShape {
public:
    float avg_pressure = 0;
    std::list<float> pressureSamples{};
    int32 intervalTimeSteps;
    b2ParticleSystem* m_system = nullptr;
    bool debug = false;
    b2CircleSensor() {
        isSensor = true;
        m_isObserver = true;
    }
    void Configure(b2ParticleSystem* system, b2Vec2& center, float radius,int32 intervalSteps=60) {
        SetTwoSided(center,0,2*b2_pi,radius);
        m_system = system;
        intervalTimeSteps=intervalSteps;
    }
    b2CircleSensor* Clone(b2BlockAllocator* allocator) const override;
    void Solve(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts);
    float GetAvgPressure() const;
    inline b2CircleSensor* AsCircleSensor() override{ return this; };
    float CalculatePressure(b2TimeStep& step, std::list<b2ParticleBodyContact>& observations) const;
};
#endif //BOX2D_B2SENSOR_H
