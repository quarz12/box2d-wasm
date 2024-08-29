//
// Created by Daniel on 22.08.2024.
//

#ifndef BOX2D_B2SENSOR_H
#define BOX2D_B2SENSOR_H
#pragma once
#include "b2_edge_shape.h"
#include "b2_time_step.h"
#include <list>
#include <numeric>
class b2ParticleBodyContact;
class b2ParticleSystem;

class b2Sensor : public b2EdgeShape {
public:
    bool pressureSensor= false;
    bool speedSensor= false;
    std::list<float> pressureSamples{}, speedSamples{};
    int32 intervalTimeSteps;
    b2ParticleSystem *m_system;

    b2Sensor() {
        isSensor = true;
        m_isObserver=true;
        m_radius = 0.0f;
    };

    inline void Configure(bool speed, bool pressure, b2ParticleSystem* system, int32 intervalSteps){
        speedSensor=speed;
        pressureSensor=pressure;
        intervalTimeSteps=intervalSteps;
        m_system=system;
        if(m_system== nullptr){
            print("system is nullptr");
        };
    }

    void SensePressure(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts);

    void SenseSpeed(b2TimeStep& step, std::list<b2ParticleBodyContact>& contacts);

    float GetAvgPressure();

    float GetAvgSpeed();

    float CalculateTheoreticalAvgPressure(b2TimeStep step, std::list<b2ParticleBodyContact>& observations) const;

    void Solve(b2TimeStep& step, std::list<b2ParticleBodyContact> &contacts);

    bool IsPressureSensor() const{ return pressureSensor;};
    bool IsSpeedSensor() const { return speedSensor;};


    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    inline b2Sensor* GetNext(){return m_next;}
    inline void SetNext(b2Sensor* next){m_next=next;}

    b2Sensor* AsSensor() override {return (b2Sensor*) this;};

    inline int32 GetSampleSize() const {return pressureSamples.size();};


#if LIQUIDFUN_EXTERNAL_LANGUAGE_API
    /// Set this as an isolated edge, with direct floats.
	void Set(float vx1, float vy1, float vx2, float vy2);
#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API

private:
    b2Sensor* m_next= nullptr;
};

class b2Gate : public b2EdgeShape {
public:
    inline bool isOpen() const { return !isClosed; }

    void open();

    void close();

private:
    bool isClosed = false;
};

class b2Valve : public b2Sensor {

    inline void Configure(bool speed, bool pressure, b2ParticleSystem* system, int32 intervalSteps, b2Gate* connectedGate){
        this->b2Sensor::Configure(speed, pressure, system, intervalSteps);
        gate=connectedGate;
    }

    b2Gate *gate;
    float threshold = 0;

    inline void setGate(b2Gate *g) { gate = g; };

    inline void setThreshold(float pressure) {
        threshold = pressure;
    };

    void Solve(b2TimeStep& step, std::list<b2ParticleBodyContact> &contacts);

    b2Shape* Clone(b2BlockAllocator* allocator) const override;
};

#endif //BOX2D_B2SENSOR_H
