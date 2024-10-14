#include "box2d/b2_shape.h"
#include "box2d/b2_arc_shape.h"
#include "box2d/b2_edge_shape.h"

bool b2Shape::IsContinuous(const b2Shape* shape, const b2Vec2& connectionPoint) {
    b2Vec2 tangent;
    b2Vec2 thisTangent;
    if (shape->GetType() == e_arc) {
        b2ArcShape* arc = (b2ArcShape*) shape;
        b2Vec2 lineToConnection = connectionPoint - arc->m_center;
        tangent = b2Vec2(-lineToConnection.y, lineToConnection.x);
    } else if (shape->GetType() == e_edge) {
        b2EdgeShape* edge = (b2EdgeShape*) shape;
        tangent = edge->m_vertex1 - edge->m_vertex2;
    }
    if (GetType() == e_arc) {
        b2ArcShape* arc = (b2ArcShape*) this;
        b2Vec2 lineToConnection = connectionPoint - arc->m_center;
        thisTangent = b2Vec2(-lineToConnection.y, lineToConnection.x);
    } else if (GetType() == e_edge) {
        b2EdgeShape* edge = (b2EdgeShape*) this;
        thisTangent = edge->m_vertex1 - edge->m_vertex2;
    }
    tangent.Normalize();
    thisTangent.Normalize();
    float cross = b2Cross(thisTangent, tangent);
    print("cross:"+str(cross));
    print("abscross:"+str(abs(cross)));
    print("smaller:"+str(abs(cross)<3.f/90.f));
    if (abs(cross) < 3.f/90.f) {
        print("true");
        return true;
    }
    return false;
}

b2Vec2 b2Shape::GetSharedPoint(const b2Shape* shape) const {
    if (m_vertex1 == shape->m_vertex1 || m_vertex1 == shape->m_vertex2)
        return m_vertex1;
    else if (m_vertex2 == shape->m_vertex1 || m_vertex2 == shape->m_vertex2)
        return m_vertex2;
    return b2Vec2(0,0);
}
bool b2Shape::AddConnection(b2Shape& newConnection) {
    if(newConnection.m_vertex1==m_vertex1 || newConnection.m_vertex2==m_vertex1) {
        previousSegment = &newConnection;
        m_isLineSegment= true;
        b2Vec2 sharedPoint = GetSharedPoint(&newConnection);
        continuousInPrev = IsContinuous(&newConnection, sharedPoint);
        return true;
    }
    else if(newConnection.m_vertex1==m_vertex2 || newConnection.m_vertex2==m_vertex2){
        nextSegment = &newConnection;
        m_isLineSegment= true;
        b2Vec2 sharedPoint = GetSharedPoint(&newConnection);
        continuousInNext = IsContinuous(&newConnection, sharedPoint);
        return true;
    }
    else {
        return false;
    }
}