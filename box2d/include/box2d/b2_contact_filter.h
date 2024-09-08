//
// Created by Daniel on 01.08.2024.
//
#pragma once
#include "b2_world_callbacks.h"

class b2MicrofluidicsContactFilter: public b2ContactFilter{
public:
    ~b2MicrofluidicsContactFilter() override {}
    bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) override;

    bool ShouldCollide(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex) override;
    bool ShouldCollide(b2ParticleSystem* particleSystem, int32 particleIndexA, int32 particleIndexB) override;

    inline void SetParticleSystem(b2ParticleSystem* system){
        m_system=system;
    }

private:
    b2ParticleSystem* m_system = nullptr;
};