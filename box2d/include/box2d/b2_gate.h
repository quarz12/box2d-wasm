//
// Created by Daniel on 16.10.2024.
//

#ifndef B2_GATE_H
#define B2_GATE_H
#include "b2_edge_shape.h"


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



#endif //B2_GATE_H
