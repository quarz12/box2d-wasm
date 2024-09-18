import Box2DFactory from 'box2d-wasm';

const Box2DFactory_: typeof import('box2d-wasm') = Box2DFactory;
import {LayerBuilder, Layer} from "./layerBuilder"
import {Sensor, Inlet} from "./wrappers";
// @ts-ignore
let box2d = await Box2DFactory_();
const {
    b2Sensor,
} = box2d;

let layerBuilder = LayerBuilder(box2d);

export class Engine {
    private sensors: Map<number, Sensor> = new Map<number, Sensor>();
    private inlets: Map<number, Inlet> = new Map<number, Inlet>();
    private layers: Map<number, Layer> = new Map<number, Layer>();

    getSensor(id: number): Sensor | undefined {
        return this.sensors.get(id);
    }

    getInlet(id: number): Inlet | undefined {
        return this.inlets.get(id);
    }

    configure(xml:String){
        //TODO
    }
    step(){
        for (const [id,layer] of this.layers) {
            layer.step();
        }
    }
}


