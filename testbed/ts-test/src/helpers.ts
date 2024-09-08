export const helpers = (box2d: typeof Box2D & EmscriptenModule) => {
    const {
        b2Shape,
    } = box2d;
    const test = () => {

    };

    return {test};
}
export const link = (a: Box2D.b2Shape, b: Box2D.b2Shape): boolean => {
    return a.AddConnection(b) && b.AddConnection(a);
}

export const linkLayerChange= (a: Box2D.b2LayerChange, b: Box2D.b2LayerChange)=>{
    a.Link(b);
    b.Link(a);
}