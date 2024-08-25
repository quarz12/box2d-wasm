//
// Created by Daniel on 22.08.2024.
//

#ifndef BOX2D_B2SENSOR_H
#define BOX2D_B2SENSOR_H
#pragma once
#include "b2_polygon_shape.h"
#include "b2_time_step.h"
#include "b2_particle_system.h"
#include <list>
#include "b2_edge_shape.h"
#include <numeric>
#include "b2_particle_system.h"


class b2Sensor : public b2EdgeShape { //TODO use edgeshape?
public:
    bool pressureSensor= false;
    bool speedSensor= false;
    std::list<float> pressureSamples, speedSamples;
    int32 intervalTimeSteps;
    b2ParticleSystem *m_system;

    b2Sensor() {
        isSensor = true;
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

    b2Vec2 CalculateTheoreticalAvgPressure(b2TimeStep step, std::list<b2ParticleBodyContact>& contacts) const;

    void Solve(b2TimeStep& step, std::list<b2ParticleBodyContact> &contacts);

    bool IsPressureSensor() const{ return pressureSensor;};
    bool IsSpeedSensor() const { return speedSensor;};


    inline b2Shape* Clone(b2BlockAllocator* allocator) const override
    {
        void* mem = allocator->Allocate(sizeof(b2Sensor));
        b2Sensor* clone = new (mem) b2Sensor;
        *clone = *this;
        return clone;
    }

    inline b2Sensor* GetNext(){return m_next;}
    inline void SetNext(b2Sensor* next){m_next=next;}

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

    inline b2Shape* Clone(b2BlockAllocator* allocator) const override
    {
        void* mem = allocator->Allocate(sizeof(b2Valve));
        b2Valve* clone = new (mem) b2Valve;
        *clone = *this;
        return clone;
    }
};

#endif //BOX2D_B2SENSOR_H
