export const helpers=(box2d: typeof Box2D & EmscriptenModule)=>{
    const {
        b2Shape,
    }=box2d;
    const test=()=>{

    };

    return {test};
}
export const link=( a:Box2D.b2ArcShape| Box2D.b2EdgeShape,  b:Box2D.b2ArcShape| Box2D.b2EdgeShape):boolean=>{
    return a.AddConnection(b) && b.AddConnection(a);
}