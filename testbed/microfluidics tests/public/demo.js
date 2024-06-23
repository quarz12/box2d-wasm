/**
 * Forked from Box2D.js
 * @see https://github.com/kripken/box2d.js/blob/f75077b/helpers/embox2d-helpers.js
 * @author dmagunov + Huy Nguyen + fork contributions from Alex Birch
 * @see https://github.com/kripken/box2d.js/blob/49dddd6/helpers/embox2d-html5canvas-debugDraw.js
 * @author dmagunov + fork contributions from Alex Birch
 * @see https://github.com/kripken/box2d.js/blob/925a279/demo/webgl/box2d.wasm.html
 * @author Alon Zakai + Huy Nguyen + fork contributions from Alex Birch
 * @license Zlib https://opensource.org/licenses/Zlib
 * License evidence: https://github.com/kripken/box2d.js/blob/master/README.markdown#box2djs
 *   "box2d.js is zlib licensed, just like Box2D."
 */
import Box2DFactory from './box2d-wasm/dist/es/entry.js';
import {makeDebugDraw, vecArrToPointer, arrToVecArr} from './debugDraw.js';

/**
 * Aliasing this variable solely for the purpose of documenting its type.
 * @type {import('box2d-wasm')}
 */
const Box2DFactory_ = Box2DFactory;
Box2DFactory_().then(box2D => {
    const {
        b2_dynamicBody,
        b2BodyDef,
        b2CircleShape,
        b2EdgeShape,
        b2Vec2,
        b2PolygonShape,
        b2World,
        b2AABB,
        b2ParticleGroupDef,
        b2ParticleSystemDef,
        b2ChainShape,
        b2ParticleColor,
        b2Pump,
        b2_tensileParticle,
        b2ParticleDef,
        b2ArcShape,
        b2_staticPressureParticle,
        b2_viscousParticle,
        _malloc,
        b2_adhesiveParticle //enum values are part of the base Box2D object
    } = box2D;

    /** @type {HTMLCanvasElement} */
    const canvas = document.getElementById("demo-canvas");
    const ctx = canvas.getContext('2d');

    const pixelsPerMeter = 32;
    const cameraOffsetMetres = {
        x: 0,
        y: 0
    };

    const pauseBtn=document.getElementById("pauseBtn");
    let isPaused=false;
    pauseBtn.addEventListener("click",()=>{
        isPaused=!isPaused;
    })
    {
    const stepForm=document.getElementById("stepBtn");
    stepForm.addEventListener("click",()=>{
        if(isPaused){
            let stepInt=parseInt(document.getElementById("stepInterval").value);
            for (let i = 0; i < stepInt; i++) {
                step(1/60*1000);
            }
            drawCanvas();
        }
    })

        const summonPartBtn = document.getElementById("summonPartBtn");
        summonPartBtn.addEventListener("click", () => {
            summonParticles();
        })
        const summonParticleBtn = document.getElementById("summonParticleBtn");
        summonParticleBtn.addEventListener("click", () => {
            summonParticle();
        })
        const fricBtn1 = document.getElementById("fricBtn1");
        fricBtn1.addEventListener("click", () => {
            summonfricParticles1();
        })
        const fricBtn2 = document.getElementById("fricBtn2");
        fricBtn2.addEventListener("click", () => {
            summonfricParticles2();
        })
        const linebtn = document.getElementById("linebtn");
        linebtn.addEventListener("click", () => {
            summonLine();
        })
    }
    const gravity = new b2Vec2(0, 0);
    const world = new b2World(gravity);

    const bd_ground = new b2BodyDef();
    const ground = world.CreateBody(bd_ground);
    //edges
    {
        const chain = new b2ChainShape();
        let corners = [new b2Vec2(0, 21), new b2Vec2(0, 0), new b2Vec2(25, 0), new b2Vec2(25, 21)];
        chain.CreateLoop(vecArrToPointer(corners, box2D), corners.length);
        ground.CreateFixture(chain, 0);
    }
    //channel
    if (false)
    {

        // {// U
        //     const chain = new b2ChainShape();
        //     let corners = arrToVecArr([[0,4],[24, 4],[24,10], [1,10]], box2D);
        //     chain.CreateChain(vecArrToPointer(corners, box2D), corners.length);
        //     ground.CreateFixture(chain, 0);
        // }
        {//bot line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(0,11),new b2Vec2(23,11));
            line.m_vertex0=new b2Vec2(0,0);
            line.m_vertex3=new b2Vec2(25,10);
            ground.CreateFixture(line,0);
        }
        {//top line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(0,3),new b2Vec2(24,3));
            ground.CreateFixture(line,0);
        }
        {//2nd line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(0,4),new b2Vec2(23,4));
            ground.CreateFixture(line,0);
            // const line = new b2EdgeShape();
            // const line2=new b2EdgeShape();
            // line.SetTwoSided(new b2Vec2(0,4),new b2Vec2(15,4));
            // line.m_vertex3=new b2Vec2(23,4);
            // line2.SetTwoSided(new b2Vec2(15,4),new b2Vec2(23,4));
            // line2.m_vertex0=new b2Vec2(0,4);
            // ground.CreateFixture(line,0);
            // ground.CreateFixture(line2,0);
        }
        {//3rd line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(1,10),new b2Vec2(23,10));
            ground.CreateFixture(line,0);
        }
        {// |
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(24,5),new b2Vec2(24,9));
            ground.CreateFixture(line,0);
        }
        {//make arc
            const arc=new b2ArcShape();
            // arc.SetTwoSided(new b2Vec2(24,9), new b2Vec2(23,10), new b2Vec2(23,9));
            arc.SetTwoSided(new b2Vec2(23,9), new b2Vec2(24,9), new b2Vec2(23,10));
            arc.m_vertex0=new b2Vec2(24,5);
            arc.m_vertex3=new b2Vec2(1,10);
            arc.m_oneSided=true;
            ground.CreateFixture(arc,0);
            const arc2=new b2ArcShape();
            arc2.SetTwoSided(new b2Vec2(23,9), new b2Vec2(25,9), new b2Vec2(23,11));
            arc2.m_vertex0=new b2Vec2(25,5);
            arc2.m_vertex3=new b2Vec2(0,11);
            arc2.m_oneSided=true;
            ground.CreateFixture(arc2,0);
        }
        {//make arc
            const arc=new b2ArcShape();
            arc.SetTwoSided(new b2Vec2(23,5), new b2Vec2(23,3), new b2Vec2(25,5));
            ground.CreateFixture(arc,0);
            arc.m_oneSided=true;
            const arc2=new b2ArcShape();
            arc2.SetTwoSided(new b2Vec2(23,5), new b2Vec2(23,4), new b2Vec2(24,5));
            arc2.m_oneSided=true;
            ground.CreateFixture(arc2,0);
        }
    }
    else
    {
        {//bot line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(0,13),new b2Vec2(21,13));
            ground.CreateFixture(line,0);
        }
        {//2nd line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(0,2),new b2Vec2(21,2));
            ground.CreateFixture(line,0);
        }
        {//3rd line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(2,15),new b2Vec2(23,15));
            ground.CreateFixture(line,0);
        }
        {// |
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(23,4),new b2Vec2(23,11));
            ground.CreateFixture(line,0);
        }
        {//make bot arc
            const arc=new b2ArcShape();
            // arc.SetTwoSided(new b2Vec2(24,9), new b2Vec2(23,10), new b2Vec2(23,9));
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
            ground.CreateFixture(arc,0);
            arc.m_oneSided=true;
            const arc2=new b2ArcShape();
            arc2.SetTwoSided(new b2Vec2(21,4), new b2Vec2(21,2), new b2Vec2(23,4));
            arc2.m_oneSided=true;
            ground.CreateFixture(arc2,0);
        }
    }

    // make particles
    const partSysDef = new b2ParticleSystemDef();
    partSysDef.radius = 0.1;
    partSysDef.dampingStrength = 0.5;
    partSysDef.pressureStrength=0.01; //prevents laminar flow
    partSysDef.staticPressureStrength=1;
    partSysDef.surfaceTensionNormalStrength=0.005;
    partSysDef.surfaceTensionPressureStrength=0.005;
    partSysDef.frictionRate=0.01;
    partSysDef.viscousStrength=10.0;
    const particleSystem = world.CreateParticleSystem(partSysDef);
    function summonParticles() {
        const pt = new b2ParticleGroupDef();
        pt.stride=0.2;    //density of particle spawns, for ideal fit radius*2
        pt.flags=b2_tensileParticle;
        pt.flags=b2_staticPressureParticle&b2_viscousParticle;
        let shape = new b2PolygonShape();
        shape.SetAsBox(5, 0.9, new b2Vec2(6, 1), 0);  //particle spawn area| len/2,height/2, center, angle
        pt.shape = shape;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        // pt.linearVelocity=new b2Vec2(5,0);
        particleSystem.CreateParticleGroup(pt);
    }
    function summonParticle() {
        const pt = new b2ParticleDef();
        pt.flags=b2_staticPressureParticle&b2_viscousParticle;
        // pt.flags=b2_tensileParticle;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        pt.set_position(new b2Vec2(1, 10.21));
        pt.velocity=new b2Vec2(10,0);
        const particle=particleSystem.CreateParticle(pt);

    }
    function summonfricParticles1(){
        const pt = new b2ParticleDef();
        pt.flags=b2_staticPressureParticle&b2_viscousParticle;
        // pt.flags=b2_tensileParticle;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        pt.set_position(new b2Vec2(1, 4.001));
        pt.velocity=new b2Vec2(2,0);
        particleSystem.CreateParticle(pt);

    }
    function summonfricParticles2(){
        const pt2 = new b2ParticleDef();
        pt2.flags=b2_staticPressureParticle&b2_viscousParticle;
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
        // pt.flags=b2_tensileParticle;
        // pt.flags=b2_staticPressureParticle&b2_viscousParticle;
        let shape = new b2PolygonShape();
        shape.SetAsBox(3,1,new b2Vec2(5,14),0);  //particle spawn area| len/2,height/2, center, angle
        pt.shape = shape;   //13.1, 15
        pt.stride=0.19;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(0, 100, 255, 255));
        pt.linearVelocity=new b2Vec2(2,0);
        const group=particleSystem.CreateParticleGroup(pt);
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
    world.SetDebugDraw(debugDraw);

    // calculate no more than a 60th of a second during one world.Step() call
    const maxTimeStepMs = 1 / 60 * 1000;
    /** @param {number} deltaMs */
    const step = (deltaMs) => {
        const clampedDeltaMs = Math.min(deltaMs, maxTimeStepMs);
        world.Step(clampedDeltaMs / 1000, 3, 2, 3);
    };
    const drawCanvas = () => {
        ctx.fillStyle = 'rgb(255,255,255)';  //set background color
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        ctx.save();
        ctx.scale(pixelsPerMeter, pixelsPerMeter);
        const {x, y} = cameraOffsetMetres;
        ctx.translate(x, y);
        ctx.lineWidth /= pixelsPerMeter;
        ctx.lineWidth *= 3;

        world.DebugDraw();
        world.GetJointList()
        ctx.restore();
    };

    /** @type {?number} */
    let handle;
    (function loop(prevMs) {
        const nowMs = window.performance.now();
        handle = requestAnimationFrame(loop.bind(null, nowMs));
        const deltaMs = nowMs - prevMs;
        if(!isPaused) {
            step(deltaMs);
            drawCanvas();
        }
    }(window.performance.now()));
});
const draw =()=>{
    const canvas = document.getElementById("demo-canvas");
    const ctx = canvas.getContext('2d');
    const setCtxColor = (rgbaStr) => {
        ctx.fillStyle = `rgba(${rgbaStr})`;
        ctx.strokeStyle = `rgba(${rgbaStr})`;
    };
    const drawPoint = (x,y) => {
        setCtxColor("255,0,0,1");
        const sizePixels = 10;
        ctx.fillRect(
            x*32-sizePixels/2,
            y*32-sizePixels/2,
            sizePixels,
            sizePixels
        );
    };
    return drawPoint;
}
window.draw = draw;
export {
    draw,
};
