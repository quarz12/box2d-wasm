export class Layer {
    step(){

    }
}

export const LayerBuilder = (box2D: typeof Box2D & EmscriptenModule) => {
    const newLayer = (): Layer => {
        return new Layer();
    }
    return {newLayer}
}