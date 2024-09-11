import Box2DFactory from 'box2d-wasm';

const Box2DFactory_: typeof import('box2d-wasm') = Box2DFactory;
import {makeDebugDraw, vecArrToPointer, arrToVecArr} from './debugDraw';
import {link, linkLayerChange} from "./helpers"

// @ts-ignore
let box2d = await Box2DFactory_();
const {
    b2BodyDef,
    b2EdgeShape,
    b2Vec2,
    b2PolygonShape,
    b2World,
    b2ParticleGroupDef,
    b2ParticleColor,
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
    b2ParticleSystemDef,
    b2LayerChange,
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
            drawCanvas(ctx2,world2);
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
        else {
            inlet.Activate();
        }
    })
    const test1btn = document.getElementById("test1");
    test1btn?.addEventListener("click", () => {
        console.log(particleSystem.GetParticleCount());
        console.log(particleSystem2.GetParticleCount());
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
const world2 = new b2World(gravity, 0.1);
const bd_ground = new b2BodyDef();
const ground = world.CreateBody(bd_ground);
const ground2 = world2.CreateBody(bd_ground);
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
const particleFlags= b2_staticPressureParticle | b2_viscousParticle | b2_frictionParticle | b2_fixtureContactFilterParticle | b2_adhesiveParticle;

const particleSystem = world.CreateParticleSystem(partSysDef);
const particleSystem2 = world2.CreateParticleSystem(partSysDef);
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
def.flags = particleFlags;
def.set_color(new b2ParticleColor(0, 100, 255, 255));
infix.Configure(particleSystem, def, new b2Vec2(0.0001, 0), new b2Vec2(0.01, 0), new b2Vec2(0.01, 0.02));
infix.SetAsBox(partSysDef.radius*3, 0.01, new b2Vec2(0.01, 0.01), 0);
let inlet = ground.CreateFixture(infix, 0).GetShape().AsInlet();
particleSystem.RegisterInlet(inlet);
let tmp=new b2EdgeShape();
tmp.SetTwoSided(new b2Vec2(0.01,0), new b2Vec2(0.01,0.02));
tmp.set_m_hasCollision(false);
ground.CreateFixture(tmp, 0);
let e=new b2EdgeShape();
e.SetTwoSided(new b2Vec2(0.2,0),new b2Vec2(0.2,0.02));
ground.CreateFixture(e,0);
let layerchangeA=new b2LayerChange();
layerchangeA.m_p=new b2Vec2(0.08,0.01);
layerchangeA.m_radius=0.01;
layerchangeA.Configure(particleSystem);
{//sensors A
    let s1:Box2D.b2Sensor=new b2Sensor();
    s1.Configure(false,true,particleSystem, 60);
    s1.SetTwoSided(new b2Vec2(0.07,0),new b2Vec2(0.07,0.02));
    layerchangeA.AddChannel(ground.CreateFixture(s1,0).GetShape().AsSensor());
    let s2=new b2Sensor();
    s2.Configure(false,true,particleSystem, 60);
    s2.SetTwoSided(new b2Vec2(0.09,0),new b2Vec2(0.09,0.02));
    layerchangeA.AddChannel(ground.CreateFixture(s2,0).GetShape().AsSensor());
}

let layerchangeB=new b2LayerChange();
layerchangeB.m_p=new b2Vec2(0.08,0.13);
layerchangeB.m_radius=0.01;
layerchangeB.Configure(particleSystem2);
{//sensors B
    let s1=new b2Sensor();
    s1.Configure(false,true,particleSystem, 60);
    s1.SetTwoSided(new b2Vec2(0.07,0.12),new b2Vec2(0.07,0.14));
    layerchangeB.AddChannel(ground2.CreateFixture(s1,0).GetShape().AsSensor());
    let s2=new b2Sensor();
    s2.Configure(false,true,particleSystem, 60);
    s2.SetTwoSided(new b2Vec2(0.09,0.12),new b2Vec2(0.09,0.14));
    layerchangeB.AddChannel(ground2.CreateFixture(s2,0).GetShape().AsSensor());
    let s3=new b2Sensor();
    s3.Configure(false,true,particleSystem, 60);
    s3.SetTwoSided(new b2Vec2(0.07,0.12),new b2Vec2(0.09,0.12));
    layerchangeB.AddChannel(ground2.CreateFixture(s3,0).GetShape().AsSensor());
    let s4=new b2Sensor();
    s4.Configure(false,true,particleSystem, 60);
    s4.SetTwoSided(new b2Vec2(0.07,0.14),new b2Vec2(0.09,0.14));
    layerchangeB.AddChannel(ground2.CreateFixture(s4,0).GetShape().AsSensor());
}
layerchangeA=ground.CreateFixture(layerchangeA,0).GetShape().AsLayerChange();
layerchangeB=ground2.CreateFixture(layerchangeB,0).GetShape().AsLayerChange();
linkLayerChange(layerchangeA,layerchangeB);
// system 2
{
    let e1=new b2EdgeShape();
    e1.SetTwoSided(new b2Vec2(0.07,0), new b2Vec2(0.07,0.12));
    ground2.CreateFixture(e1,0);
    let e2=new b2EdgeShape();
    e2.SetTwoSided(new b2Vec2(0.09,0), new b2Vec2(0.09,0.12));
    ground2.CreateFixture(e2,0);
    let e3=new b2EdgeShape();
    e3.SetTwoSided(new b2Vec2(0,0.12), new b2Vec2(0.07,0.12));
    ground2.CreateFixture(e3,0);
    let e4=new b2EdgeShape();
    e4.SetTwoSided(new b2Vec2(0,0.14), new b2Vec2(0.07,0.14));
    ground2.CreateFixture(e4,0);
    let e5=new b2EdgeShape();
    e5.SetTwoSided(new b2Vec2(0.09,0.12), new b2Vec2(0.2,0.12));
    ground2.CreateFixture(e5,0);
    let e6=new b2EdgeShape();
    e6.SetTwoSided(new b2Vec2(0.09,0.14), new b2Vec2(0.2,0.14));
    ground2.CreateFixture(e6,0);
    let e7=new b2EdgeShape();
    e7.SetTwoSided(new b2Vec2(0.07,0.14), new b2Vec2(0.07,0.2));
    ground2.CreateFixture(e7,0);
    let e8=new b2EdgeShape();
    e8.SetTwoSided(new b2Vec2(0.09,0.14), new b2Vec2(0.09,0.2));
    ground2.CreateFixture(e8,0);
}
function summonParticles() {
    const pt = new b2ParticleGroupDef();
    pt.flags = particleFlags;
    let shape = new b2PolygonShape();
    shape.SetAsBox(0.05, 0.009, new b2Vec2(0.05, 0.009), 0);  //particle spawn area | len/2,height/2, center, angle
    pt.shape = shape;
    pt.set_linearVelocity(new b2Vec2(1, 0));
    //alpha is divided by 255 to get value between 0-1
    pt.set_color(new b2ParticleColor(0, 100, 255, 255));
    // pt.linearVelocity=new b2Vec2(5,0);
    particleSystem.CreateParticleGroup(pt);
}

function summonParticlexy(x: number, y: number) {
    const pt = new b2ParticleDef();
    pt.flags = particleFlags;
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
world2.SetDebugDraw(debugDraw2);

// calculate no more than a 60th of a second during one world.Step() call
const maxTimeStepMs = 1 / 60 * 1000;
/** @param {number} deltaMs */
const step = (deltaMs: number) => {
    const clampedDeltaMs = Math.min(deltaMs, maxTimeStepMs);
    world.Step(clampedDeltaMs / 1000, 3, 2, 1);
    world2.Step(clampedDeltaMs/1000 , 3, 2, 1);
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
        drawCanvas(ctx2,world2);
    }
}());

Object.assign(window, {
    particleSystem,
    particleSystem2,
    ctx,
    world,
    inlet,
    b2Vec2,
    layerchangeA,
    layerchangeB,
})

