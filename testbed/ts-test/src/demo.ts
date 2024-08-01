import Box2DFactory from 'box2d-wasm';
const Box2DFactory_: typeof import('box2d-wasm') = Box2DFactory;
import {makeDebugDraw, vecArrToPointer, arrToVecArr} from './debugDraw';
import {link} from "./helpers"
Box2DFactory_().then(box2D => {
    const {
        b2BodyDef,
        b2EdgeShape,
        b2Vec2,
        b2PolygonShape,
        b2World,
        b2ParticleGroupDef,
        b2ParticleSystemDef,
        b2ChainShape,
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
        //enum values are part of the base Box2D object
    } = box2D;
    /** @type {HTMLCanvasElement} */
    const canvas: HTMLCanvasElement = document.getElementById("demo-canvas") as HTMLCanvasElement;
    const ctx: CanvasRenderingContext2D = canvas?.getContext('2d') as CanvasRenderingContext2D;
    const canvas2: HTMLCanvasElement = document.getElementById("demo-canvas2") as HTMLCanvasElement;
    const ctx2: CanvasRenderingContext2D = canvas2?.getContext('2d') as CanvasRenderingContext2D;
    const pixelsPerMeter: number = 32;
    const cameraOffsetMetres: { x: number; y: number } = {
        x: 0,
        y: 0
    };
    const pauseBtn: HTMLElement | null =document.getElementById("pauseBtn");
    let isPaused=false;
    pauseBtn?.addEventListener("click",()=>{
        isPaused=!isPaused;
    })
    {
        const stepForm=document.getElementById("stepBtn");
        stepForm?.addEventListener("click",()=>{
            if(isPaused){
                let form=document.getElementById("stepInterval") as HTMLFormElement | null
                let stepInt=parseInt(form?.value);
                for (let i = 0; i < stepInt; i++) {
                    step(1/60*1000);
                    // drawCanvas();
                }
                drawCanvas(ctx, world);
                drawCanvas(ctx2, world2);
            }
        })

        const summonPartBtn = document.getElementById("summonPartBtn");
        summonPartBtn?.addEventListener("click", () => {
            summonParticles();
        })
        const summonParticleBtn = document.getElementById("summonParticleBtn");
        summonParticleBtn?.addEventListener("click", () => {
            summonParticle();
        })
        const fricBtn1 = document.getElementById("fricBtn1");
        fricBtn1?.addEventListener("click", () => {
            summonfricParticles1();
        })
        const fricBtn2 = document.getElementById("fricBtn2");
        fricBtn2?.addEventListener("click", () => {
            summonfricParticles2();
        })
        const linebtn = document.getElementById("linebtn");
        linebtn?.addEventListener("click", () => {
            summonLine();
        })
        const test1btn=document.getElementById("test1");
        test1btn?.addEventListener("click",()=>{
            console.log(particleSystem.GetParticleCount());
            console.log(particleSystem2.GetParticleCount());
        });
        const xy=document.getElementById("summonxy");
        xy?.addEventListener("click",()=>{
            let x=document.getElementById("x") as HTMLFormElement;
            let y=document.getElementById("y") as HTMLFormElement;
            summonParticlexy(parseFloat(x.value),parseFloat(y.value));
        });
    }
    const gravity = new b2Vec2(0,0);
    const world = new b2World(gravity,0.5);
    const world2=new b2World(new b2Vec2(0,0),0.5);
    const bd_ground = new b2BodyDef();
    const ground = world.CreateBody(bd_ground);
    const ground2=world2.CreateBody(bd_ground);
    //edges
    {
        const chain = new b2ChainShape();
        let corners = [new b2Vec2(0, 21), new b2Vec2(0, 0), new b2Vec2(25, 0), new b2Vec2(25, 21)];
        chain.CreateLoop(vecArrToPointer(corners, box2D), corners.length);
        let fix=ground.CreateFixture(chain, 0);
    }
    {
        const chain = new b2ChainShape();
        let corners = [new b2Vec2(0, 21), new b2Vec2(0, 0), new b2Vec2(25, 0), new b2Vec2(25, 21)];
        chain.CreateLoop(vecArrToPointer(corners, box2D), corners.length);
        let fix=ground2.CreateFixture(chain, 0);
    }
    //world1
    {
        {//2nd line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(2,13),new b2Vec2(21,13));
            ground.CreateFixture(line,0);
        }
        {//1st line
            const line = new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(0, 2), new b2Vec2(6, 2));
            const line2 = new b2EdgeShape();
            line2.SetTwoSided(new b2Vec2(6, 2), new b2Vec2(21, 2));
            link(line,line2);
            let f=ground.CreateFixture(line, 0);
            let f2=ground.CreateFixture(line2, 0);
            let t=f.GetShape();
            let t2=f2.GetShape();
            // const chain=new b2ChainShape();
            // let first=new b2Vec2(0,2);
            // let last= new b2Vec2(21,2);
            // chain.CreateChain(vecArrToPointer([first,new b2Vec2(6,2),last],box2D),3,first,last);
            // ground.CreateFixture(chain,0);
        }

        {//3rd line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(2,15),new b2Vec2(6,15));
            ground.CreateFixture(line,0);
            const line2=new b2EdgeShape();
            line2.SetTwoSided(new b2Vec2(6,15),new b2Vec2(21,15));
            ground.CreateFixture(line2,0);
        }
        {// |
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(23,4),new b2Vec2(23,11));
            ground.CreateFixture(line,0);
        }
        {//make bot arc
            const arc=new b2ArcShape();
            arc.SetTwoSided(new b2Vec2(21,11), new b2Vec2(25,11), new b2Vec2(21,15));
            ground.CreateFixture(arc,0);
            const arc2=new b2ArcShape();
            arc2.SetTwoSided(new b2Vec2(21,11), new b2Vec2(23,11), new b2Vec2(21,13));
            ground.CreateFixture(arc2,0);
            const arc3=new b2ArcShape();
            arc3.SetTwoSided(new b2Vec2(2,15), new b2Vec2(0,15), new b2Vec2(2,13));
            ground.CreateFixture(arc3,0);
        }
        {//make top arc
            const arc=new b2ArcShape();
            arc.SetTwoSided(new b2Vec2(21,4), new b2Vec2(21,0), new b2Vec2(25,4));
            const arc2=new b2ArcShape();
            arc2.SetTwoSided(new b2Vec2(21,4), new b2Vec2(21,2), new b2Vec2(23,4));
            // arc2.AddConnection(line2);//all setprev setnext must be done before createfixture
            ground.CreateFixture(arc,0);
            ground.CreateFixture(arc2,0);
        }
        {
            const l1=new b2EdgeShape();
            l1.SetTwoSided(new b2Vec2(2,7), new b2Vec2(5,7));
            const l2=new b2EdgeShape();
            l2.SetTwoSided(new b2Vec2(2,9), new b2Vec2(5,9));
            const arc=new b2ArcShape();
            arc.SetTwoSided(new b2Vec2(5,8), new b2Vec2(5,7), new b2Vec2(5,9));
            let success;
            success=arc.AddConnection(l1);
            success=arc.AddConnection(l2);
            success=l1.AddConnection(arc);
            success=l2.AddConnection(arc);
            ground.CreateFixture(l1,0);
            ground.CreateFixture(l2,0);
            ground.CreateFixture(arc,0);

        }
    }
    //world2
    {
        {//2nd line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(2,13),new b2Vec2(21,13));
            ground2.CreateFixture(line,0);
        }
        {//1st line
            const line = new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(0, 2), new b2Vec2(6, 2));
            const line2 = new b2EdgeShape();
            line2.SetTwoSided(new b2Vec2(6, 2), new b2Vec2(21, 2));
            link(line,line2);
            let f=ground2.CreateFixture(line, 0);
            let f2=ground2.CreateFixture(line2, 0);
            let t=f.GetShape();
            let t2=f2.GetShape();
            // const chain=new b2ChainShape();
            // let first=new b2Vec2(0,2);
            // let last= new b2Vec2(21,2);
            // chain.CreateChain(vecArrToPointer([first,new b2Vec2(6,2),last],box2D),3,first,last);
            // ground.CreateFixture(chain,0);
        }

        {//3rd line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(2,15),new b2Vec2(6,15));
            ground2.CreateFixture(line,0);
            const line2=new b2EdgeShape();
            line2.SetTwoSided(new b2Vec2(6,15),new b2Vec2(21,15));
            ground2.CreateFixture(line2,0);
        }
        {// |
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(23,4),new b2Vec2(23,11));
            ground2.CreateFixture(line,0);
        }
        {//make bot arc
            const arc=new b2ArcShape();
            arc.SetTwoSided(new b2Vec2(21,11), new b2Vec2(25,11), new b2Vec2(21,15));
            ground2.CreateFixture(arc,0);
            const arc2=new b2ArcShape();
            arc2.SetTwoSided(new b2Vec2(21,11), new b2Vec2(23,11), new b2Vec2(21,13));
            ground2.CreateFixture(arc2,0);
            const arc3=new b2ArcShape();
            arc3.SetTwoSided(new b2Vec2(2,15), new b2Vec2(0,15), new b2Vec2(2,13));
            ground2.CreateFixture(arc3,0);
        }
        {//make top arc
            const arc=new b2ArcShape();
            arc.SetTwoSided(new b2Vec2(21,4), new b2Vec2(21,0), new b2Vec2(25,4));
            const arc2=new b2ArcShape();
            arc2.SetTwoSided(new b2Vec2(21,4), new b2Vec2(21,2), new b2Vec2(23,4));
            // arc2.AddConnection(line2);//all setprev setnext must be done before createfixture
            ground2.CreateFixture(arc,0);
            ground2.CreateFixture(arc2,0);
        }
        {
            const l1=new b2EdgeShape();
            l1.SetTwoSided(new b2Vec2(2,7), new b2Vec2(5,7));
            const l2=new b2EdgeShape();
            l2.SetTwoSided(new b2Vec2(2,9), new b2Vec2(5,9));
            const arc=new b2ArcShape();
            arc.SetTwoSided(new b2Vec2(5,8), new b2Vec2(5,7), new b2Vec2(5,9));
            let success;
            success=arc.AddConnection(l1);
            success=arc.AddConnection(l2);
            success=l1.AddConnection(arc);
            success=l2.AddConnection(arc);
            ground2.CreateFixture(l1,0);
            ground2.CreateFixture(l2,0);
            ground2.CreateFixture(arc,0);

        }
    }
    // make particles
    const partSysDef = new b2ParticleSystemDef();
    partSysDef.radius = 0.2;
    partSysDef.dampingStrength = 0;//.5;
    partSysDef.pressureStrength=0.1; //prevents laminar flow
    partSysDef.staticPressureStrength=0.1;
    partSysDef.surfaceTensionNormalStrength=0.005;
    partSysDef.surfaceTensionPressureStrength=0.005;
    partSysDef.frictionRate=0.0;
    partSysDef.viscousStrength=1.0;
    const particleSystem = world.CreateParticleSystem(partSysDef);
    const particleSystem2= world2.CreateParticleSystem(partSysDef);
    let filter=new b2MicrofluidicsContactFilter();
    filter.SetParticleSystem(particleSystem);
    world.SetContactFilter(filter);
    {   //layer change line
        const fixtureDef= new b2FixtureDef();
        const line=new b2EdgeShape();
        line.SetTwoSided(new b2Vec2(23,10),new b2Vec2(25,10));
        fixtureDef.shape=line;
        let fixture=ground.CreateFixture(fixtureDef);
        fixture.SetLayerChange(particleSystem2);
        const fixtureDef2= new b2FixtureDef();
        const line2=new b2EdgeShape();
        line.SetTwoSided(new b2Vec2(23,9),new b2Vec2(25,9));
        fixtureDef2.shape=line2;
        let fixture2=ground2.CreateFixture(fixtureDef);
    }
    function summonParticles() {
        const pt = new b2ParticleGroupDef();
        pt.flags=b2_tensileParticle;
        pt.flags=b2_staticPressureParticle | b2_viscousParticle | b2_frictionParticle;
        let shape = new b2PolygonShape();
        shape.SetAsBox(5, 0.9, new b2Vec2(6, 1), 0);  //particle spawn area| len/2,height/2, center, angle
        pt.shape = shape;
        pt.set_linearVelocity(new b2Vec2(1,0));
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        // pt.linearVelocity=new b2Vec2(5,0);
        particleSystem.CreateParticleGroup(pt);
    }
    function summonParticle() {
        const pt = new b2ParticleDef();
        pt.flags=b2_staticPressureParticle|b2_viscousParticle|b2_frictionParticle;
        // pt.flags=b2_tensileParticle;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        pt.set_position(new b2Vec2(1, 1.81));
        pt.velocity=new b2Vec2(10,0);
        particleSystem.CreateParticle(pt);
    }
    function summonParticlexy(x:number,y:number) {
        const pt = new b2ParticleDef();
        pt.flags=b2_staticPressureParticle|b2_viscousParticle|b2_frictionParticle|b2_fixtureContactFilterParticle;
        // pt.flags=b2_tensileParticle;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        pt.set_position(new b2Vec2(x, y));
        // pt.velocity=new b2Vec2(10,0);
        particleSystem.CreateParticle(pt);
    }
    function summonfricParticles1(){
        const pt = new b2ParticleDef();
        pt.flags=b2_staticPressureParticle|b2_viscousParticle|b2_frictionParticle;
        // pt.flags=b2_tensileParticle;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        pt.set_position(new b2Vec2(1, 7.001));
        pt.velocity=new b2Vec2(2,0);
        particleSystem.CreateParticle(pt);
    }
    function summonfricParticles2(){
        const pt2 = new b2ParticleDef();
        pt2.flags=b2_staticPressureParticle|b2_viscousParticle|b2_frictionParticle;
        // pt.flags=b2_tensileParticle;
        //alpha is divided by 255 to get value between 0-1
        pt2.set_color(new b2ParticleColor(0, 100, 255, 255));
        pt2.set_position(new b2Vec2(15, 4.2));
        pt2.velocity=new b2Vec2(-2,0);
        particleSystem.CreateParticle(pt2);
    }

    function summonLine(){
        const pt = new b2ParticleGroupDef();
        pt.stride=0.2;    //density of particle spawns, for ideal fit radius*2
        pt.flags=b2_staticPressureParticle|b2_viscousParticle|b2_frictionParticle;
        // pt.flags=b2_staticPressureParticle&b2_viscousParticle;
        let shape = new b2PolygonShape();
        shape.SetAsBox(3,1,new b2Vec2(5,14),0);  //particle spawn area| len/2,height/2, center, angle
        pt.shape = shape;   //13.1, 15
        pt.stride=0.19;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        pt.linearVelocity=new b2Vec2(2,0);
        particleSystem.CreateParticleGroup(pt);
        // let p=new b2ParticleGroupDef();
        // p.stride=0.2;
        // p.flags=b2_staticPressureParticle&b2_viscousParticle;
        // shape = new b2EdgeShape();
        // shape.SetTwoSided(new b2Vec2(4,13.100000381469727),new b2Vec2(15,13.100000381469727));  //particle spawn area| len/2,height/2, center, angle
        // p.shape = shape;
        // p.stride=0.19;
        // //alpha is divided by 255 to get value between 0-1
        // p.set_color(new b2ParticleColor(0, 100, 255, 255));
        // p.linearVelocity=new b2Vec2(2,0);
        // particleSystem.CreateParticleGroup(p);

    }
    // console.log=function (){}; //disable console logs
    const debugDraw = makeDebugDraw(ctx, pixelsPerMeter, box2D);
    const debugDraw2 = makeDebugDraw(ctx2, pixelsPerMeter, box2D);
    world.SetDebugDraw(debugDraw);
    world2.SetDebugDraw(debugDraw2);

    // calculate no more than a 60th of a second during one world.Step() call
    const maxTimeStepMs = 1 / 60 * 1000;
    /** @param {number} deltaMs */
    const step = (deltaMs: number) => {
        const clampedDeltaMs = Math.min(deltaMs, maxTimeStepMs);
        world.Step(clampedDeltaMs / 1000, 3, 2, 3);
        world2.Step(clampedDeltaMs / 1000, 3, 2, 3);
    };
    const drawCanvas = (ctx:CanvasRenderingContext2D, world: Box2D.b2World) => {
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

    /** @type {?number} */

    (function loop(prevMs) {
        const nowMs = window.performance.now();
        requestAnimationFrame(loop.bind(null, nowMs));
        let deltaMs = nowMs - prevMs;
        deltaMs=1/60*1000;
        if(!isPaused) {
            step(deltaMs);
            drawCanvas(ctx, world);
            drawCanvas(ctx2, world2);
        }
    }(window.performance.now()));
});
