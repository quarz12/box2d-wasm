import {Inlet, Sensor} from "./wrappers";

export class Layer {
    constructor(private readonly world: Box2D.b2World, private readonly body: Box2D.b2Body, private readonly system: Box2D.b2ParticleSystem,
                private readonly inletMap: Map<number, Inlet>, private readonly sensorMap: Map<number, Sensor>) {
    }

    private timestep = 1 / 60 * 1000;
    private velocityIterations = 1;
    private positionIterations = 1;
    private particleIterations = 1;

    step() {
        this.world.Step(this.timestep, this.velocityIterations, this.positionIterations, this.particleIterations);
    }

    createGate(box2D: typeof Box2D & EmscriptenModule, valve: Box2D.b2Valve) {
        const {b2Gate} = box2D;
        let gate = new b2Gate();


        let fixture = this.body.CreateFixture(gate, 0);
        valve.AddGate(fixture.GetShape().AsGate());
    }
}

export class LayerBuilder {
    constructor(private readonly box2D: typeof Box2D & EmscriptenModule, private readonly systemDef: Box2D.b2ParticleSystemDef,
                private readonly gravitationalSlow: number, private readonly inletMap: Map<number, Inlet>, private readonly sensorMap: Map<number, Sensor>, private readonly valveLayerIndexMap: Map<Box2D.b2Valve, number>) {
    }

    createLayer = (xmlLayer: String): Layer => {
        const {
            b2World, b2Vec2, b2Valve, b2BodyDef
        } = this.box2D;
        let world = new b2World(new b2Vec2(0, 0), this.gravitationalSlow);
        let system = world.CreateParticleSystem(this.systemDef);
        let body = world.CreateBody(new b2BodyDef());
        let layer = new Layer(world, body, system, this.inletMap, this.sensorMap);
        //todo for object in xmllayer -> create
        return layer;
    }
}