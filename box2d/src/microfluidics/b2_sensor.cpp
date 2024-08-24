//
// Created by Daniel on 22.08.2024.
//

#include "box2d/b2_sensor.h"

void b2Sensor::SensePressure(std::list<b2ParticleBodyContact> &contacts) {
    print("sensePressure");
    float value;//TODO
    pressureSamples.push_back(value);
    if (pressureSamples.size() > intervalTimeSteps && intervalTimeSteps > 0)
        pressureSamples.pop_front();
}

void b2Sensor::SenseSpeed(std::list<b2ParticleBodyContact> &contacts) {
    print("senseSpeed");
    for (b2ParticleBodyContact contact: contacts) {
        print(m_system->GetPositionBuffer()[contact.index].ToString());
    }
    float value;//TODO
    speedSamples.push_back(value);
    if (speedSamples.size() > intervalTimeSteps && intervalTimeSteps > 0)
        speedSamples.pop_front();
}

void b2Sensor::Solve(std::list<b2ParticleBodyContact> &contacts) {
    if (IsPressureSensor()) {
        SensePressure(contacts);
    }
    if (IsSpeedSensor()) {
        SenseSpeed(contacts);
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


void b2Valve::Solve(std::list<b2ParticleBodyContact> &contacts) {
    this->b2Sensor::Solve(contacts); //todo verify
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
