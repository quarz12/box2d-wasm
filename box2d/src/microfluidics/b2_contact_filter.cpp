//
// Created by Daniel Martin on 25.05.2024.
//

#include "box2d/b2_contact_filter.h"
#include "box2d/b2_arc_shape.h"
#include "box2d/b2_edge_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_particle_system.h"
#include "box2d/b2_shape.h"


bool b2MicrofluidicsContactFilter::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
    return true;
}

bool b2MicrofluidicsContactFilter::ShouldCollide(b2Fixture *fixture, b2ParticleSystem *particleSystem, int32 particleIndex) {
    if (!fixture->HasCollision())
        return false;
    b2Shape *shape = fixture->GetShape();
    b2Shape::Type type = shape->GetType();
    b2Vec2 pos = m_system->GetPositionBuffer()[particleIndex];
    b2Transform tf = fixture->GetBody()->GetTransform();
    if (fixture->GetShape()->m_isLineSegment) {
        if (type == b2Shape::e_edge) {
            b2EdgeShape *edge = (b2EdgeShape *) shape;
            if (edge->CloserToPrev(pos, tf) || edge->CloserToNext(pos, tf))
                return false;
        } else if (type == b2Shape::e_arc) {
            b2ArcShape *arc = (b2ArcShape *) shape;
            if (arc->CloserToPrev(pos, tf) || arc->CloserToNext(pos, tf))
                return false;
        }
    }
    return true;
}

bool b2MicrofluidicsContactFilter::ShouldCollide(b2ParticleSystem* particleSystem,
                                                 int32 particleIndexA, int32 particleIndexB){return true;};
