//
// Created by Daniel on 02.06.2024.
//

#ifndef BOX2D_B2_ARC_SHAPE_H
#define BOX2D_B2_ARC_SHAPE_H

#include "b2_shape.h"
#include "b2_api.h"

class B2_API b2ArcShape: public b2Shape
{
public:
    b2ArcShape();

    /// Set this as a part of a sequence. Vertex v0 precedes the edge and vertex v3
    /// follows. These extra vertices are used to provide smooth movement
    /// across junctions. This also makes the collision one-sided. The edge
    /// normal points to the right looking from v1 to v2.
    /// arc direction is clockwise
    void SetOneSided(const b2Vec2& start, const b2Vec2& end, const b2Vec2& center,const b2Vec2& v2, const b2Vec2& v3);

    /// Set this as an isolated edge. Collision is two-sided. arc direction is clockwise
    void SetTwoSided(const b2Vec2& start, const b2Vec2& end, const b2Vec2& center);

    /// Implement b2Shape.
    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    /// @see b2Shape::GetChildCount
    int32 GetChildCount() const override;

    /// Implement b2Shape.
    bool TestPoint(const b2Transform& transform, const b2Vec2& p) const override;

    // @see b2Shape::ComputeDistance
    void ComputeDistance(const b2Transform& xf, const b2Vec2& point, float* distance, b2Vec2* normal, int32 childIndex) const override;

    /// Implement b2Shape.
    /// @note because the circle is solid, rays that start inside do not hit because the normal is
    /// not defined.
    bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
                 const b2Transform& transform, int32 childIndex) const override;

    /// @see b2Shape::ComputeAABB
    void ComputeAABB(b2AABB* aabb, const b2Transform& transform, int32 childIndex) const override;

    /// @see b2Shape::ComputeMass
    void ComputeMass(b2MassData* massData, float density) const override;

#if LIQUIDFUN_EXTERNAL_LANGUAGE_API
    /// Set position with direct floats.
	void SetPosition(float x, float y) { m_p.Set(x, y); }

	/// Get x-coordinate of position.
	float GetPositionX() const { return m_p.x; }

	/// Get y-coordinate of position.
	float GetPositionY() const { return m_p.y; }
#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API

    /// start/end/center of circle
    /// start/end must have equal distance to center
    b2Vec2 m_start, m_end, m_center;

    /// Optional adjacent vertices. These are used for smooth collision.
    b2Vec2 m_vertex0, m_vertex3; //probably not used for particles

    float m_lineRadius;
    // draw endpoints?
    bool m_oneSided= false;
};

inline b2ArcShape::b2ArcShape()
{
    m_type = e_arc;
    m_radius = 0.0f;
    m_start.SetZero();
    m_end.SetZero();
    m_center.SetZero();
    m_lineRadius=b2_polygonRadius;
    m_vertex0.SetZero();
    m_vertex3.SetZero();
}

#endif //BOX2D_B2_ARC_SHAPE_H
