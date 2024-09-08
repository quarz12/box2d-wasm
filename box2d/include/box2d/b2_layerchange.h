//
// Created by Daniel on 08.09.2024.
//

#ifndef B2_LAYERCHANGE_H
#define B2_LAYERCHANGE_H
#include "b2_circle_shape.h"


struct b2ParticleBodyContact;
class b2ParticleSystem;

class b2LayerChange : public b2CircleShape {
public:
    b2LayerChange* m_linked = nullptr;
    std::list<b2Sensor*> connectedChannels;
    b2ParticleSystem* m_system = nullptr;

    b2LayerChange() {
        m_isObserver = true;
        isLayerChange = true;
    }

    void Configure(b2ParticleSystem* system);

    inline void Link(b2LayerChange* target) { m_linked = target; };
    inline b2LayerChange* asLayerChange() { return this; };
    inline void AddChannel(b2Sensor* sensor) { connectedChannels.push_back(sensor); };

    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    void Solve(const std::list<b2ParticleBodyContact>& contacts);
};


#endif //B2_LAYERCHANGE_H
