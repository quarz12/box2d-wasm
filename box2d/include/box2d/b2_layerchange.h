//
// Created by Daniel on 08.09.2024.
//

#ifndef B2_LAYERCHANGE_H
#define B2_LAYERCHANGE_H
#include "b2_circle_shape.h"
#include "b2_sensor.h"


class b2CircleSensor;
struct b2ParticleBodyContact;
class b2ParticleSystem;

class b2LayerChange : public b2CircleShape {
public:
    b2LayerChange* m_linked = nullptr;
    b2ParticleSystem* m_system = nullptr;
    b2CircleSensor* m_sensor = nullptr;
    bool debug = false;
    b2LayerChange() {
        m_isObserver = true;
        isLayerChange = true;
    }

    void Configure(b2ParticleSystem* system, b2CircleSensor* sensor);

    inline void Link(b2LayerChange* target) { m_linked = target; };
    inline b2LayerChange* AsLayerChange() override { return this; };

    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    void Solve(const std::list<b2ParticleBodyContact>& contacts) const;

    b2Vec2 ForceAway(const b2Vec2& position, float force) const;

    float GetLayerShiftProbability() const;

    float GetPressure() const {
        return m_sensor->GetAvgPressure();
    }

    static bool TestPointForShift(b2Vec2& pos, float radius, const std::list<b2Vec2>& particlesInLinked);
};


#endif //B2_LAYERCHANGE_H
