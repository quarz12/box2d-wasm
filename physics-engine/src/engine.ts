import Box2DFactory from 'box2d-wasm';

const Box2DFactory_: typeof import('box2d-wasm') = Box2DFactory;
import {LayerBuilder, Layer} from "./layerBuilder"
import {Sensor, Inlet} from "./wrappers";
// @ts-ignore
let box2d = await Box2DFactory_();


export class Engine {
    constructor(xml: String) {
        const {b2ParticleSystemDef} = box2d;
        let systemDef = new b2ParticleSystemDef();
        let gravitationalSlow = 0.1;
        this.layerBuilder = new LayerBuilder(box2d, systemDef, gravitationalSlow, this.inlets, this.sensors);
        //todo
    }

    private sensors: Map<number, Sensor> = new Map<number, Sensor>();
    private inlets: Map<number, Inlet> = new Map<number, Inlet>();
    private layers: Map<number, Layer> = new Map<number, Layer>();
    private layerBuilder: LayerBuilder;

    private newLayer(xml: String) {
        let id = 1;
        let layer = this.layerBuilder.createLayer(xml);
        this.layers.set(id, layer);
    }

    getSensor(id: number): Sensor | undefined {
        return this.sensors.get(id);
    }

    getInlet(id: number): Inlet | undefined {
        return this.inlets.get(id);
    }

    step() {
        for (const [id, layer] of this.layers) {
            layer.step();
        }
    }
}


