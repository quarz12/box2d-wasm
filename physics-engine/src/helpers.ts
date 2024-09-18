export class Helpers {
    constructor(private readonly box2D: typeof Box2D & EmscriptenModule) {
    }

    /** to replace original C++ operator = */
    copyVec2 = (vec: Box2D.b2Vec2): Box2D.b2Vec2 => {
        const {b2Vec2} = this.box2D;
        return new b2Vec2(vec.get_x(), vec.get_y());
    }

    /** to replace original C++ operator * (float) */
    scaleVec2 = (vec: Box2D.b2Vec2, scale: number): void => {
        vec.set_x(scale * vec.get_x());
        vec.set_y(scale * vec.get_y());
    }

    /** to replace original C++ operator *= (float) */
    scaledVec2 = (vec: Box2D.b2Vec2, scale: number): Box2D.b2Vec2 => {
        const {b2Vec2} = this.box2D;
        return new b2Vec2(scale * vec.get_x(), scale * vec.get_y());
    }
}