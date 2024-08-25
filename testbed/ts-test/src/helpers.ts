export const helpers = (box2d: typeof Box2D & EmscriptenModule) => {
    const {
        b2Shape,
    } = box2d;
    const test = () => {

    };

    return {test};
}
export const createLayerChange = (shape1: Box2D.b2Shape, shape2: Box2D.b2Shape, sys1: Box2D.b2ParticleSystem, sys2: Box2D.b2ParticleSystem,
                                  normal1: Box2D.b2Vec2, normal2: Box2D.b2Vec2, ground1: Box2D.b2Body, ground2: Box2D.b2Body): void => {


}
export const link = (a: Box2D.b2ArcShape | Box2D.b2EdgeShape, b: Box2D.b2ArcShape | Box2D.b2EdgeShape): boolean => {
    return a.AddConnection(b) && b.AddConnection(a);
}