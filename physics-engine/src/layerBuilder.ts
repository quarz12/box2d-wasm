import {Inlet, Sensor} from "./wrappers";

export class Layer {
    constructor(private readonly world: Box2D.b2World, private readonly system: Box2D.b2ParticleSystem, private readonly inletMap: Map<number,Inlet>, private readonly sensorMap: Map<number, Sensor>) {
    }

    private timestep = 1 / 60 * 1000;
    private velocityIterations = 1;
    private positionIterations = 1;
    private particleIterations = 1;

    step() {
        this.world.Step(this.timestep, this.velocityIterations, this.positionIterations, this.particleIterations);
    }
}

export class LayerBuilder {
    constructor(private readonly box2D: typeof Box2D & EmscriptenModule, private readonly systemDef: Box2D.b2ParticleSystemDef,
                private readonly gravitationalSlow: number, private readonly inletMap: Map<number,Inlet>, private readonly sensorMap: Map<number, Sensor>) {
    }

    createLayer = (xmlLayer: String): Layer => {
        const {
            b2World, b2Vec2
        } = this.box2D;
        let world = new b2World(new b2Vec2(0, 0), this.gravitationalSlow);
        let system = world.CreateParticleSystem(this.systemDef);
        let layer=new Layer(world, system, this.inletMap, this.sensorMap);
        //todo for object in xmllayer -> create
        return layer;
    }
}