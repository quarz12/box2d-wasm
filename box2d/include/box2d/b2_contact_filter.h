//
// Created by Daniel on 01.08.2024.
//
#pragma once
#include "b2_world_callbacks.h"
#include "box2d/b2_world_callbacks.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_particle_system.h"
#include "box2d/b2_arc_shape.h"
#include "box2d/b2_edge_shape.h"
#include "box2d/b2_contact_filter.h"

#ifndef BOX2D_B2_CONTACT_FILTER_H
#define BOX2D_B2_CONTACT_FILTER_H

#endif //BOX2D_B2_CONTACT_FILTER_H

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
    b2ParticleSystem* m_system;
};