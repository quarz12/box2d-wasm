//
// Created by Daniel on 02.06.2024.
//

#include "box2d/b2_arc_shape.h"
#include "box2d/b2_block_allocator.h"
#include "box2d/b2_edge_shape.h"
#include <new>


void b2ArcShape::SetTwoSided(const b2Vec2& center, const b2Vec2& start, const b2Vec2& end) {
    m_vertex1 = start;
    m_vertex2 = end;
    m_center = center;
    m_radius = (start - center).Length();
    fromRadians = b2Atan2(start.y-center.y, start.x-center.x);
    toRadians = b2Atan2(end.y-center.y, end.x-center.x);
}

void b2ArcShape::SetTwoSided(const b2Vec2& center, const float startRadians, const float endRadians,
                             const float radius) {
    fromRadians = startRadians;
    toRadians = endRadians;
    m_vertex1.x = center.x + radius * cos(fromRadians);
    m_vertex1.y = center.y + radius * sin(fromRadians);
    m_vertex2.x = center.x + radius * cos(toRadians);
    m_vertex2.y = center.y + radius * sin(toRadians);
    m_center = center;
    m_radius = radius;
}

b2Shape* b2ArcShape::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(b2ArcShape));
    b2ArcShape* clone = new(mem) b2ArcShape;
    *clone = *this;
    return clone;
}

int32 b2ArcShape::GetChildCount() const {
    return 1;
}

bool b2ArcShape::TestPoint(const b2Transform& xf, const b2Vec2& p) const {
    B2_NOT_USED(xf);
    B2_NOT_USED(p);
    return false;
}

void b2ArcShape::ComputeDistance(const b2Transform& transform, const b2Vec2& point, float* distance, b2Vec2* normal,
                                 int32 childIndex) const {
    B2_NOT_USED(childIndex);

    b2Vec2 center = transform.p + b2Mul(transform.q, m_center); // Center of the circle

    // Check if the closest point is within the arc
    b2Vec2 toPoint = point - center; // Vector from center to point point

    float distanceToPoint = toPoint.Length();

    double angleToPoint = b2Atan2(toPoint.y, toPoint.x);
    double angleToStart = fromRadians;
    double angleToEnd = toRadians;
    // angleToEnd = b2_pi - angleToEnd; //flip yaxis todo remove if renderer changes?
    // angleToStart = b2_pi - angleToStart;
    // angleToPoint = b2_pi - angleToPoint;
    // Normalize angles to [0, 2π]
    if (angleToPoint < 0)
        angleToPoint += 2.0f * b2_pi;
    if (angleToStart < 0)
        angleToStart += 2.0f * b2_pi;
    if (angleToEnd < 0)
        angleToEnd += 2.0f * b2_pi;

    bool isWithinArc;
    if (angleToStart < angleToEnd) {
        isWithinArc = angleToStart <= angleToPoint && angleToPoint <= angleToEnd;
    } else {
        isWithinArc = (angleToStart <= angleToPoint && angleToPoint < 2 * b2_pi) || (
                          angleToPoint <= angleToEnd && angleToPoint > 0);
    }

    if (isWithinArc) {
        *distance = m_radius - distanceToPoint;
        *normal = *distance > 0 ? -toPoint : toPoint;
        *distance = fabs(*distance);
    } else {
        // Compute distance to the closest endpoint of the arc
        b2Vec2 pointToStart = point - m_vertex1;
        b2Vec2 pointToEnd = point - m_vertex2;
        float distanceToStart = pointToStart.Normalize();
        float distanceToEnd = pointToEnd.Normalize();

        if (distanceToStart < distanceToEnd) {
            *distance = distanceToStart;
            *normal = pointToStart;
        } else {
            *distance = distanceToEnd;
            *normal = pointToEnd;
        }
    }
}


/// check if input ray hits the shape
bool b2ArcShape::RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
                         const b2Transform& transform, int32 childIndex) const {
    B2_NOT_USED(childIndex);
    b2Vec2 center = transform.p + b2Mul(transform.q, m_center);
    b2Vec2 s = input.p1 - center; //circle center to ray start
    float b = b2Dot(s, s) - m_radius * m_radius;

    // Solve quadratic equation.
    b2Vec2 r = input.p2 - input.p1;
    float c = b2Dot(s, r);
    float rr = b2Dot(r, r);
    float sigma = c * c - rr * b;

    // Check for negative discriminant and short segment.
    // no intersection
    if (sigma < 0.0f || rr < b2_epsilon) {
        return false;
    }

    // Find the point of intersection of the line with the circle.
    float fractionOfRayUntilCollision = -(c + b2Sqrt(sigma));
    if (fractionOfRayUntilCollision < 0)
        fractionOfRayUntilCollision = -(c - b2Sqrt(sigma));
    // Is the intersection point on the segment?
    if (0.0f <= fractionOfRayUntilCollision && fractionOfRayUntilCollision <= input.maxFraction * rr) {
        fractionOfRayUntilCollision /= rr; //✓
        b2Vec2 intersectionPoint = input.p1 + fractionOfRayUntilCollision * r; //✓

        // Check if the intersection point is within the arc segment
        b2Vec2 toIntersection = intersectionPoint - center;
        float distanceToStartPoint = s.Length();
        // Normalize the vectors to get directions
        toIntersection.Normalize();

        // Calculate angles in radian
        double angleToIntersection = b2Atan2(toIntersection.y, toIntersection.x);
        double angleToStart = fromRadians;
        double angleToEnd = toRadians;
        // angleToEnd=b2_pi-angleToEnd; //flip yaxis todo remove if renderer changes?
        // angleToStart=b2_pi-angleToStart;
        // angleToIntersection=b2_pi-angleToIntersection;
        // Normalize angles to [0, 2π]
        if (angleToIntersection < 0)
            angleToIntersection += 2 * b2_pi;
        if (angleToStart < 0)
            angleToStart += 2 * b2_pi;
        if (angleToEnd < 0)
            angleToEnd += 2 * b2_pi;

        bool isWithinArc;
        print("angle to end:" + str(angleToEnd));
        print("angle to start:" + str(angleToStart));
        print("angle to intersection:" + str(angleToIntersection));
        if (angleToStart < angleToEnd) {
            isWithinArc = angleToStart <= angleToIntersection && angleToIntersection <= angleToEnd;
        } else {
            isWithinArc = (angleToStart <= angleToIntersection && angleToIntersection <= 2 * b2_pi) || (
                              angleToEnd >= angleToIntersection && angleToIntersection >= 0);
        }
        print("isWithinArc:" + str(isWithinArc));
        b2Vec2 surfaceNorm = intersectionPoint - center;
        surfaceNorm.Normalize();
        if (isWithinArc) {
            output->fraction = fractionOfRayUntilCollision;
            output->normal = distanceToStartPoint > m_radius ? surfaceNorm : -surfaceNorm;
            print(output->normal.ToString());
            return true;
        }
    }
    return false;
}

void b2ArcShape::ComputeAABB(b2AABB* aabb, const b2Transform& transform, int32 childIndex) const {
    B2_NOT_USED(childIndex);

    b2Vec2 p = transform.p + b2Mul(transform.q, m_center);
    aabb->lowerBound.Set(p.x - m_radius, p.y - m_radius);
    aabb->upperBound.Set(p.x + m_radius, p.y + m_radius);
    //TODO can be optimized for arcs <= 3/4 circle, careful with start<end
}

void b2ArcShape::ComputeMass(b2MassData* massData, float density) const {
    B2_NOT_USED(density);

    massData->mass = 0.0f;
    massData->center = 0.5f * (m_vertex1 - m_vertex2);
    massData->I = 0.0f;
}

bool b2ArcShape::CloserToNext(b2Vec2& point, b2Transform& tf) const {
    if (nextSegment == nullptr)
        return false;
    float distanceThis, distanceNext;
    b2Vec2 normThis, normNext;
    b2Transform transform = tf;
    ComputeDistance(transform, point, &distanceThis, &normThis, 0);
    nextSegment->ComputeDistance(transform, point, &distanceNext, &normNext, 0);
    if (distanceNext == distanceThis) {
        return this < nextSegment;
    }
    return distanceNext < distanceThis;
    //TODO check Angle
}

bool b2ArcShape::CloserToPrev(b2Vec2& point, b2Transform& tf) const {
    if (previousSegment == nullptr)
        return false;
    float distanceThis, distancePrev;
    b2Vec2 normThis, normNext;
    b2Transform transform = tf;
    ComputeDistance(transform, point, &distanceThis, &normThis, 0);
    previousSegment->ComputeDistance(transform, point, &distancePrev, &normNext, 0);
    if (distancePrev == distanceThis) {
        return this > previousSegment;
    }
    return distancePrev < distanceThis;
    //TODO
}

bool b2ArcShape::AddConnection(b2Shape& next) {
    if (next.GetType() == b2Shape::Type::e_edge) {
        b2EdgeShape edge = (b2EdgeShape&) next;
        if (edge.m_vertex1 == m_vertex1 || edge.m_vertex2 == m_vertex1) {
            previousSegment = &next;
            m_isLineSegment = true;
            return true;
        } else if (edge.m_vertex1 == m_vertex2 || edge.m_vertex2 == m_vertex2) {
            nextSegment = &next;
            m_isLineSegment = true;
            return true;
        } else {
            return false;
        }
    } else if (next.GetType() == b2Shape::Type::e_arc) {
        b2ArcShape arc = (b2ArcShape&) arc;
        if (arc.m_vertex1 == m_vertex1 || arc.m_vertex2 == m_vertex1) {
            previousSegment = &next;
            m_isLineSegment = true;
            return true;
        } else if (arc.m_vertex1 == m_vertex2 || arc.m_vertex2 == m_vertex2) {
            nextSegment = &next;
            m_isLineSegment = true;
            return true;
        } else {
            return false;
        }
    } else { return false; }
}
