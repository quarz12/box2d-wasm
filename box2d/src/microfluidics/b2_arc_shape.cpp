//
// Created by Daniel on 02.06.2024.
//

#include "box2d/b2_arc_shape.h"
#include "box2d/b2_block_allocator.h"
#include <new>
#include <emscripten/em_asm.h>

void b2ArcShape::SetOneSided(const b2Vec2& center, const b2Vec2& v0, const b2Vec2& start, const b2Vec2& end, const b2Vec2& v3)
{
    m_start = start;
    m_end = end;
    m_center = center;
    m_vertex0 = v0;
    m_vertex3 = v3;
    m_oneSided = true;
    m_radius=(start-center).Length();
}

void b2ArcShape::SetTwoSided(const b2Vec2& center, const b2Vec2& start, const b2Vec2& end)
{
    m_start = start;
    m_end = end;
    m_center=center;
    m_oneSided = false;
    m_radius=(start-center).Length();
}

b2Shape* b2ArcShape::Clone(b2BlockAllocator* allocator) const
{
    void* mem = allocator->Allocate(sizeof(b2ArcShape));
    b2ArcShape* clone = new (mem) b2ArcShape;
    *clone = *this;
    return clone;
}

int32 b2ArcShape::GetChildCount() const
{
    return 1;
}

bool b2ArcShape::TestPoint(const b2Transform& xf, const b2Vec2& p) const
{
    B2_NOT_USED(xf);
    B2_NOT_USED(p);
    return false;
}

void b2ArcShape::ComputeDistance(const b2Transform& transform, const b2Vec2& point, float* distance, b2Vec2* normal, int32 childIndex) const
{
    B2_NOT_USED(childIndex);

    b2Vec2 center = transform.p + b2Mul(transform.q, m_center); // Center of the circle

    // Check if the closest point is within the arc
    b2Vec2 toPoint = point - center; // Vector from center to point point
    b2Vec2 toStart = m_start - center;
    b2Vec2 toEnd = m_end - center;

    float distanceToPoint=toPoint.Length();

    // Normalize the vectors to get directions
    toPoint.Normalize();
    toStart.Normalize();
    toEnd.Normalize();

    double angleToPoint = b2Atan2(toPoint.y, toPoint.x);
    double angleToStart = b2Atan2(toStart.y, toStart.x);
    double angleToEnd = b2Atan2(toEnd.y, toEnd.x);
    angleToEnd=b2_pi-angleToEnd; //flip yaxis todo remove if renderer changes?
    angleToStart=b2_pi-angleToStart;
    angleToPoint=b2_pi-angleToPoint;
    // Normalize angles to [0, 2π]
    if (angleToPoint < 0)
        angleToPoint += 2.0f * b2_pi;
    if (angleToStart < 0)
        angleToStart += 2.0f * b2_pi;
    if (angleToEnd < 0)
        angleToEnd += 2.0f * b2_pi;

    bool isWithinArc = false;
    if (angleToStart > angleToEnd) {
        isWithinArc = (angleToPoint <= angleToStart && angleToPoint >= angleToEnd);
    } else {
        isWithinArc = (angleToPoint > angleToStart && angleToPoint >= angleToEnd);
    }

    if (isWithinArc)
    {
        *distance = m_radius-distanceToPoint;
        *normal = *distance>0 ? -toPoint : toPoint;
        *distance = fabs(*distance);
    }
    else
    {
        // Compute distance to the closest endpoint of the arc
        b2Vec2 pointToStart=point-m_start;
        b2Vec2 pointToEnd=point-m_end;
        float distanceToStart = pointToStart.Normalize();
        float distanceToEnd = pointToEnd.Normalize();

        if (distanceToStart < distanceToEnd)
        {
            *distance = distanceToStart;
            *normal = pointToStart;
        }
        else
        {
            *distance = distanceToEnd;
            *normal = pointToEnd;
        }
    }
}


/// check if input ray hits the shape
bool b2ArcShape::RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
                         const b2Transform& transform, int32 childIndex) const
{
    B2_NOT_USED(childIndex);
    b2Vec2 position = transform.p + b2Mul(transform.q, m_center);
    b2Vec2 s = input.p1 - position; //circle center to ray start
    float b = b2Dot(s, s) - m_radius * m_radius;

    // Solve quadratic equation.
    b2Vec2 r = input.p2 - input.p1;
    float c =  b2Dot(s, r);
    float rr = b2Dot(r, r);
    float sigma = c * c - rr * b;

    // Check for negative discriminant and short segment.
    // no intersection
    if (sigma < 0.0f || rr < b2_epsilon)
    {
        return false;
    }

    // Find the point of intersection of the line with the circle.
    float fractionOfRayUntilCollision = -(c + b2Sqrt(sigma));
    if(fractionOfRayUntilCollision < 0)
        fractionOfRayUntilCollision = -(c - b2Sqrt(sigma));
    // Is the intersection point on the segment?
    if (0.0f <= fractionOfRayUntilCollision && fractionOfRayUntilCollision <= input.maxFraction * rr) {
        fractionOfRayUntilCollision /= rr; //✓
        b2Vec2 intersectionPoint = input.p1 + fractionOfRayUntilCollision * r;//✓

        // Check if the intersection point is within the arc segment
        b2Vec2 toIntersection = intersectionPoint - position;
        b2Vec2 toStart = m_start - position;
        b2Vec2 toEnd = m_end - position;
        float distanceToStart=toStart.Length();
        // Normalize the vectors to get directions
        toIntersection.Normalize();
        toStart.Normalize();
        toEnd.Normalize();

        // Calculate angles in radian
        double angleToIntersection = b2Atan2(toIntersection.y,toIntersection.x);
        double angleToStart = b2Atan2(toStart.y, toStart.x);
        double angleToEnd = b2Atan2(toEnd.y, toEnd.x);
        angleToEnd=b2_pi-angleToEnd; //flip yaxis todo remove if renderer changes?
        angleToStart=b2_pi-angleToStart;
        angleToIntersection=b2_pi-angleToIntersection;
        // Normalize angles to [0, 2π]
        if (angleToIntersection < 0)
            angleToIntersection += 2 * b2_pi;
        if (angleToStart < 0)
            angleToStart += 2 * b2_pi;
        if (angleToEnd < 0)
            angleToEnd += 2 * b2_pi;

        bool isWithinArc = false;

        if (angleToStart > angleToEnd) {
            isWithinArc = (angleToIntersection <= angleToStart && angleToIntersection >= angleToEnd);
        } else {
            isWithinArc = (angleToIntersection > angleToStart && angleToIntersection >= angleToEnd);
        }

//        EM_ASM({
//            console.log("ray from: "+$0+","+$1);
//            console.log("ray to: "+$2+","+$3);
//            console.log("angle to start: "+$4);
//            console.log("angle to end: "+$5);
//            console.log("angle to intersection: "+$6);
//                   if ($8 > 2 && $9 > 2)
//                       console.log("point of intersection: " + $8 + "," + $9);
//            console.log("iswithin: "+Boolean($7));
//               }, input.p1.x, input.p1.y, input.p2.x, input.p2.y, angleToStart, angleToEnd, angleToIntersection,
//               isWithinArc, intersectionPoint.x, intersectionPoint.y
//        );

        b2Vec2 surfaceNorm=(intersectionPoint-position);
        surfaceNorm.Normalize();
        if (isWithinArc) {
            output->fraction =  fractionOfRayUntilCollision;
            output->normal = distanceToStart<m_radius ? surfaceNorm : -surfaceNorm;
//            EM_ASM(console.log("surface norm: "+$0+","+$1),output->normal.x,output->normal.y);
            return true;
        }
    }
    return false;
}

void b2ArcShape::ComputeAABB(b2AABB* aabb, const b2Transform& transform, int32 childIndex) const
{
    B2_NOT_USED(childIndex);

    b2Vec2 p = transform.p + b2Mul(transform.q, m_center);
    aabb->lowerBound.Set(p.x - m_radius, p.y - m_radius);
    aabb->upperBound.Set(p.x + m_radius, p.y + m_radius);
}

void b2ArcShape::ComputeMass(b2MassData* massData, float density) const
{
    B2_NOT_USED(density);

    massData->mass = 0.0f;
    massData->center=0.5f*(m_start-m_end);
    massData->I = 0.0f;
}
