// MIT License

// Copyright (c) 2019 Erin Catto
// Copyright (c) 2013 Google, Inc.

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "box2d/b2_edge_shape.h"
#include "box2d/b2_block_allocator.h"
#include <new>

void b2EdgeShape::SetOneSided(const b2Vec2& v0, const b2Vec2& v1, const b2Vec2& v2, const b2Vec2& v3)
{
	m_vertex0 = v0;
	m_vertex1 = v1;
	m_vertex2 = v2;
	m_vertex3 = v3;
	m_oneSided = true;
}

void b2EdgeShape::SetTwoSided(const b2Vec2& v1, const b2Vec2& v2)
{
	m_vertex1 = v1;
	m_vertex2 = v2;
	m_oneSided = false;
}

b2Shape* b2EdgeShape::Clone(b2BlockAllocator* allocator) const
{
	void* mem = allocator->Allocate(sizeof(b2EdgeShape));
	b2EdgeShape* clone = new (mem) b2EdgeShape;
	*clone = *this;
	return clone;
}

int32 b2EdgeShape::GetChildCount() const
{
	return 1;
}

bool b2EdgeShape::TestPoint(const b2Transform& xf, const b2Vec2& p) const
{
	B2_NOT_USED(xf);
	B2_NOT_USED(p);
	return false;
}

void b2EdgeShape::ComputeDistance(const b2Transform& xf, const b2Vec2& p, float* distance, b2Vec2* normal, int32 childIndex) const
{
	B2_NOT_USED(childIndex);

	b2Vec2 v1 = b2Mul(xf, m_vertex1);
	b2Vec2 v2 = b2Mul(xf, m_vertex2);

	b2Vec2 d = p - v1;
	b2Vec2 s = v2 - v1;
	float ds = b2Dot(d, s);
	if (ds > 0)
	{
		float s2 = b2Dot(s, s);
		if (ds > s2)
		{
			d = p - v2;
		}
		else
		{
			d -= ds / s2 * s;
		}
	}

	float d1 = d.Length();
	*distance = d1;
	*normal = d1 > 0 ? 1 / d1 * d : b2Vec2_zero;

}

// p = p1 + t * d
// v = v1 + s * e
// p1 + t * d = v1 + s * e
// s * e - t * d = p1 - v1
bool b2EdgeShape::RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
							const b2Transform& xf, int32 childIndex) const
{
	B2_NOT_USED(childIndex);

	// Put the ray into the edge's frame of reference.
	b2Vec2 p1 = b2MulT(xf.q, input.p1 - xf.p);
	b2Vec2 p2 = b2MulT(xf.q, input.p2 - xf.p);
	b2Vec2 d = p2 - p1;

	b2Vec2 v1 = m_vertex1;
	b2Vec2 v2 = m_vertex2;
	b2Vec2 e = v2 - v1;

	// Normal points to the right, looking from v1 at v2
	b2Vec2 normal(e.y, -e.x);
	normal.Normalize();

	// q = p1 + t * d
	// dot(normal, q - v1) = 0
	// dot(normal, p1 - v1) + t * dot(normal, d) = 0
	float numerator = b2Dot(normal, v1 - p1);
	if (m_oneSided && numerator > 0.0f)
	{
		return false;
	}

	float denominator = b2Dot(normal, d);

	if (denominator == 0.0f)
	{
		return false;
	}

	float t = numerator / denominator;  //intersection fraction of ray
	if (t < 0.0f || input.maxFraction < t)
	{
		return false;
	}

	b2Vec2 q = p1 + t * d;  //intersection point on edge

	// q = v1 + s * r
	// s = dot(q - v1, r) / dot(r, r)
	b2Vec2 r = v2 - v1;
	float rr = b2Dot(r, r);
	if (rr == 0.0f)
	{
		return false;
	}

	float s = b2Dot(q - v1, r) / rr;    //is q on the edge?
	if (s < 0.0f || s > 1.0f )  // is s not between 0,1
	{
		return false;
	}

	output->fraction = t;
	if (numerator > 0.0f)
	{
		output->normal = -b2Mul(xf.q, normal);
	}
	else
	{
		output->normal = b2Mul(xf.q, normal);
	}
	return true;
}

void b2EdgeShape::ComputeAABB(b2AABB* aabb, const b2Transform& xf, int32 childIndex) const
{
	B2_NOT_USED(childIndex);

	b2Vec2 v1 = b2Mul(xf, m_vertex1);
	b2Vec2 v2 = b2Mul(xf, m_vertex2);

	b2Vec2 lower = b2Min(v1, v2);
	b2Vec2 upper = b2Max(v1, v2);

	aabb->lowerBound = lower;
	aabb->upperBound = upper;
}

void b2EdgeShape::ComputeMass(b2MassData* massData, float density) const
{
	B2_NOT_USED(density);

	massData->mass = 0.0f;
	massData->center = 0.5f * (m_vertex1 + m_vertex2);
	massData->I = 0.0f;
}

bool b2EdgeShape::CloserToNext(b2Vec2 point) const {
    if(nextSegment== nullptr)
        return false;
    float distanceThis, distanceNext;
    b2Vec2 normThis,normNext;
    b2Transform transform;
    ComputeDistance(transform, point, &distanceThis, &normThis, 0);
    nextSegment->ComputeDistance(transform, point, &distanceNext, &normNext,0);
    return distanceNext<distanceThis;
    //TODO check Angle
}

bool b2EdgeShape::CloserToPrev(b2Vec2 point) const {
    if(previousSegment== nullptr)
        return false;
    float distanceThis, distancePrev;
    b2Vec2 normThis,normNext;
    b2Transform transform;
    ComputeDistance(transform, point, &distanceThis, &normThis, 0);
    previousSegment->ComputeDistance(transform, point, &distancePrev, &normNext,0);
    return distancePrev<distanceThis;
    //TODO
}

bool b2EdgeShape::AddConnection(b2Shape& next) {
    if(next.m_vertex1==m_vertex1 || next.m_vertex2==m_vertex1) {
        previousSegment = &next;
        m_isLineSegment= true;
        return true;
    }
    else if(next.m_vertex1==m_vertex2 || next.m_vertex2==m_vertex2){
        nextSegment = &next;
        m_isLineSegment= true;
        return true;
    }
    else{
        return false;
    }
}
