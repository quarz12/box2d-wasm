//
// Created by Daniel on 08.09.2024.
//

#ifndef B2_LAYERCHANGE_H
#define B2_LAYERCHANGE_H
#include "b2_circle_shape.h"
#include <vector>

#include "b2_sensor.h"

struct b2ParticleBodyContact;
class b2ParticleSystem;

class b2LayerChange : public b2CircleShape {
public:
    b2LayerChange* m_linked = nullptr;
    std::vector<b2Sensor*> connectedChannels;
    b2ParticleSystem* m_system = nullptr;

    b2LayerChange() {
        m_isObserver = true;
        isLayerChange = true;
    }

    void Configure(b2ParticleSystem* system); //todo also take circle params

    inline void Link(b2LayerChange* target) { m_linked = target; };
    inline b2LayerChange* AsLayerChange() override { return this; };
    inline void AddChannel(b2Sensor* sensor) {
        // sensor->forceSensor=true;
        connectedChannels.push_back(sensor);
    };
    inline b2Sensor* GetChannel(int32 index) const { return connectedChannels[index]; };
    inline int32 GetChannelCount() const { return connectedChannels.size(); };

    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    void Solve(const std::list<b2ParticleBodyContact>& contacts) const;

    float GetLayerShiftProbability() const;

    float GetAvgPressure() const;
};


#endif //B2_LAYERCHANGE_H
