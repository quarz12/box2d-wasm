//
// Created by Daniel on 16.10.2024.
//

#include "box2d/b2_gate.h"

#include "box2d/b2_block_allocator.h"

void b2Gate::Open() {
    m_hasCollision = false;
    isClosed = false;
}

void b2Gate::Close() {
    m_hasCollision = true;
    isClosed = true;
}


b2Gate* b2Gate::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2Gate));
    b2Gate* clone = new(mem) b2Gate;
    *clone = *this;
    return clone;
}
