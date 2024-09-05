import Box2DFactory from 'box2d-wasm';

const Box2DFactory_: typeof import('box2d-wasm') = Box2DFactory;
import {makeDebugDraw, vecArrToPointer, arrToVecArr} from './debugDraw';
import {link} from "./helpers"

// @ts-ignore
let box2d = await Box2DFactory_();
const {
    b2BodyDef,
    b2EdgeShape,
    b2Vec2,
    b2PolygonShape,
    b2World,
    b2ParticleGroupDef,
    b2ParticleSystemDef,
    b2ParticleColor,
    b2FixtureDef,
    b2_tensileParticle,
    b2ParticleDef,
    b2ArcShape,
    b2_staticPressureParticle,
    b2_viscousParticle,
    b2_frictionParticle,
    b2_fixtureContactFilterParticle,
    b2MicrofluidicsContactFilter,
    b2_airParticle,
    b2_adhesiveParticle,
    b2Sensor,
    b2ForceField,
    b2ParticleSystem,
    b2Gate,
    b2Valve,
    b2Inlet,
    //enum values are part of the base Box2D object
} = box2d;
/** @type {HTMLCanvasElement} */
const canvas: HTMLCanvasElement = document.getElementById("demo-canvas") as HTMLCanvasElement;
const ctx: CanvasRenderingContext2D = canvas?.getContext('2d') as CanvasRenderingContext2D;
const canvas2: HTMLCanvasElement = document.getElementById("demo-canvas2") as HTMLCanvasElement;
const ctx2: CanvasRenderingContext2D = canvas2?.getContext('2d') as CanvasRenderingContext2D;
const pixelsPerMeter: number = 3200;
const cameraOffsetMetres: { x: number; y: number } = {
    x: 0,
    y: 0
};
const pauseBtn: HTMLElement | null = document.getElementById("pauseBtn");
let isPaused = false;
pauseBtn?.addEventListener("click", () => {
    isPaused = !isPaused;
})
{
    const stepForm = document.getElementById("stepBtn");
    stepForm?.addEventListener("click", () => {
        if (isPaused) {
            let form = document.getElementById("stepInterval") as HTMLFormElement | null
            let stepInt = parseInt(form?.value);
            for (let i = 0; i < stepInt; i++) {
                step(1 / 60 * 1000);
                // drawCanvas();
            }
            drawCanvas(ctx, world);
        }
    })

    const summonPartBtn = document.getElementById("summonPartBtn");
    summonPartBtn?.addEventListener("click", () => {
        summonParticles();
    })
    const fricBtn1 = document.getElementById("btn1");
    fricBtn1?.addEventListener("click", () => {
        if (inlet.IsActive())
            inlet.Deactivate();
        else{
            inlet.Activate();
        }
    })
    const test1btn = document.getElementById("test1");
    test1btn?.addEventListener("click", () => {
        console.log(particleSystem.GetParticleCount());
    });
    const xy = document.getElementById("summonxy");
    xy?.addEventListener("click", () => {
        let x = document.getElementById("x") as HTMLFormElement;
        let y = document.getElementById("y") as HTMLFormElement;
        summonParticlexy(parseFloat(x.value), parseFloat(y.value));
    });
    const force = document.getElementById("applyForce");
    force?.addEventListener("click", () => {
        let x = document.getElementById("fx") as HTMLFormElement;
        let y = document.getElementById("fy") as HTMLFormElement;
        applyForce(parseFloat(x.value), parseFloat(y.value));
    });
}
const gravity = new b2Vec2(0, 0);
const world = new b2World(gravity, 0.1);
const bd_ground = new b2BodyDef();
const ground = world.CreateBody(bd_ground);
//edges
// make particles
const partSysDef = new b2ParticleSystemDef();
partSysDef.radius = 3/1000;//3mm  100cm=1m
partSysDef.dampingStrength = 0;//.5;
partSysDef.pressureStrength = 0.5; //prevents laminar flow
partSysDef.staticPressureStrength = 0.1;
partSysDef.surfaceTensionNormalStrength = 0.05;
partSysDef.surfaceTensionPressureStrength = 0.05;
partSysDef.frictionRate = 0.0; // irrelevant?
partSysDef.viscousStrength = 1.0;
partSysDef.maxAirPressure = 30;
//1 results in equal force to pressure
partSysDef.adhesiveStrength = 0.001;
partSysDef.adhesionRadius = 0.01;
partSysDef.staticPressureIterations = 10;
const particleSystem = world.CreateParticleSystem(partSysDef);
// let sensor;
// {
//     let s = new b2Sensor();
//     s.Configure(true, true, particleSystem, 60);
// // sensor.SetTwoSided(new b2Vec2(23,5), new b2Vec2(25,5));
//     s.SetTwoSided(new b2Vec2(10, 0), new b2Vec2(10, 2));
//     sensor = ground.CreateFixture(s, 0);
// }
// let ff = new b2ForceField();
// ff.SetAsBox(0.1,1,new b2Vec2(10,1),0);
// ff.Configure(new b2Vec2(-50,0),true,3,particleSystem);
// let f=ground.CreateFixture(ff, 0);

// let gate=new b2Gate();
// gate.SetTwoSided(new b2Vec2(9,0),new b2Vec2(9,2));
// let fg=ground.CreateFixture(gate,0);
// let v=new b2Valve;
// gate=fg.GetShape().AsGate();
// v.Configure(particleSystem,60,gate,4);
// v.SetTwoSided(new b2Vec2(15,0),new b2Vec2(15,2));
// v.SetForceField(100,true, ground, particleSystem);
// let valve=ground.CreateFixture(v,0);
let e1=new b2EdgeShape();
e1.SetTwoSided(new b2Vec2(0,0),new b2Vec2(0.2,0));
ground.CreateFixture(e1,0);
let e2=new b2EdgeShape();
e2.SetTwoSided(new b2Vec2(0,0.02),new b2Vec2(0.2,0.02));
ground.CreateFixture(e2,0);

let filter = new b2MicrofluidicsContactFilter();
filter.SetParticleSystem(particleSystem);
world.SetContactFilter(filter);

let infix = new b2Inlet();
let def = new b2ParticleDef();
def.flags = b2_staticPressureParticle | b2_viscousParticle | b2_frictionParticle | b2_fixtureContactFilterParticle | b2_adhesiveParticle;
def.set_color(new b2ParticleColor(0, 100, 255, 255));
infix.Configure(particleSystem, def, new b2Vec2(0.001, 0), new b2Vec2(0.01, 0), new b2Vec2(0.01, 0.02));
infix.SetAsBox(partSysDef.radius*3, 0.01, new b2Vec2(0.01, 0.01), 0);
let inlet = ground.CreateFixture(infix, 0).GetShape().AsInlet();
particleSystem.RegisterInlet(inlet);
let tmp=new b2EdgeShape();
tmp.SetTwoSided(new b2Vec2(0.01,0), new b2Vec2(0.01,0.02));
tmp.set_m_hasCollision(false);
ground.CreateFixture(tmp,0);

let s=new b2Sensor()
s.SetTwoSided(new b2Vec2(5,0),new b2Vec2(5,2));
s.Configure(true,true,particleSystem,60);
s=ground.CreateFixture(s,0).GetShape().AsSensor();
function summonParticles() {
    const pt = new b2ParticleGroupDef();
    pt.flags = b2_tensileParticle;
    pt.flags = b2_staticPressureParticle | b2_viscousParticle | b2_frictionParticle | b2_fixtureContactFilterParticle | b2_adhesiveParticle;
    let shape = new b2PolygonShape();
    shape.SetAsBox(5, 0.9, new b2Vec2(6, 1), 0);  //particle spawn area | len/2,height/2, center, angle
    pt.shape = shape;
    pt.set_linearVelocity(new b2Vec2(1, 0));
    //alpha is divided by 255 to get value between 0-1
    pt.set_color(new b2ParticleColor(0, 100, 255, 255));
    // pt.linearVelocity=new b2Vec2(5,0);
    particleSystem.CreateParticleGroup(pt);
}

function summonParticlexy(x: number, y: number) {
    const pt = new b2ParticleDef();
    pt.flags = b2_staticPressureParticle | b2_viscousParticle | b2_frictionParticle | b2_fixtureContactFilterParticle | b2_adhesiveParticle;
    // pt.flags=b2_tensileParticle;
    //alpha is divided by 255 to get value between 0-1
    pt.set_color(new b2ParticleColor(0, 100, 255, 255));
    pt.set_position(new b2Vec2(x, y));
    // pt.velocity=new b2Vec2(10,0);
    particleSystem.CreateParticle(pt);
}

function applyForce(x: number, y: number) {
    particleSystem.ApplyForce(0, particleSystem.GetParticleCount(), new b2Vec2(x, y));
}

const debugDraw = makeDebugDraw(ctx, pixelsPerMeter, box2d);
const debugDraw2 = makeDebugDraw(ctx2, pixelsPerMeter, box2d);
world.SetDebugDraw(debugDraw);

// calculate no more than a 60th of a second during one world.Step() call
const maxTimeStepMs = 1 / 60 * 1000;
/** @param {number} deltaMs */
const step = (deltaMs: number) => {
    const clampedDeltaMs = Math.min(deltaMs, maxTimeStepMs);
    world.Step(clampedDeltaMs / 1000, 3, 2, 3);
};
const drawCanvas = (ctx: CanvasRenderingContext2D, world: Box2D.b2World) => {
    ctx.fillStyle = 'rgb(255,255,255)';  //set background color
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.save();
    ctx.scale(pixelsPerMeter, pixelsPerMeter);
    const {x, y} = cameraOffsetMetres;
    ctx.translate(x, y);
    ctx.lineWidth /= pixelsPerMeter;
    ctx.lineWidth *= 3;
    world.DebugDraw();
    ctx.restore();
};
let deltaMs = 1000 / 60;
(function loop() {
    requestAnimationFrame(loop);
    if (!isPaused) {
        step(deltaMs);
        drawCanvas(ctx, world);
    }
}());

Object.assign(window, {
    particleSystem,
    ctx,
    world,
    // sensor,
    // gate,
    // valve,
    inlet,
    b2Vec2,
    // f,
    // fg,
    s,
})

