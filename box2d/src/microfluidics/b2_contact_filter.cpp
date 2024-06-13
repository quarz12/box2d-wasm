//
// Created by Daniel Martin on 25.05.2024.
//
#include "box2d/b2_world_callbacks.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_particle_system.h"

class PumpContactFilter: public b2ContactFilter{



public:
    ~PumpContactFilter() override {}

    bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) override {
        return b2ContactFilter::ShouldCollide(fixtureA, fixtureB);
    }

    bool ShouldCollide(b2Fixture* fixtureA, b2ParticleSystem* particleSystem, int32 particleIndex) override
    //TODO for valve
    {
        const b2ParticleSystem& system=*particleSystem;
        const b2Filter& filterA = fixtureA->GetFilterData();
//        const b2Filter& filterB = fixtureB->GetFilterData();
//
//        if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0)
//        {
//            return filterA.groupIndex > 0;
//        }
//
//        bool collide = (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;
//        return collide;
        return false;
    }
};