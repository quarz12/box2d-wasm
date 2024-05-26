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
        _malloc
    } = box2D;

    /** @type {HTMLCanvasElement} */
    const canvas = document.getElementById("demo-canvas");
    const ctx = canvas.getContext('2d');

    const pixelsPerMeter = 32;
    const cameraOffsetMetres = {
        x: 0,
        y: 0
    };

    const gravity = new b2Vec2(0, 0);
    const world = new b2World(gravity);

    const bd_ground = new b2BodyDef();
    const ground = world.CreateBody(bd_ground);
    //edges
    {
        const chain = new b2ChainShape();
        let corners = [new b2Vec2(0, 21), new b2Vec2(0, 0), new b2Vec2(25, 0), new b2Vec2(25, 21)];
        chain.CreateLoop(vecArrToPointer(corners, box2D), corners.length);
        ground.CreateFixture(chain, 1);
        console.log(chain.is_pump);
    }
    //channel
    {
        {//left L
            const chain = new b2ChainShape();
            let corners = arrToVecArr([[0, 4], [10, 4], [11, 7]], box2D);
            chain.CreateChain(vecArrToPointer(corners, box2D), corners.length);
            ground.CreateFixture(chain, 1);
        }
        {//right L
            const chain = new b2ChainShape();
            let corners = arrToVecArr([[25, 4], [15, 4], [14, 7]], box2D);
            chain.CreateChain(vecArrToPointer(corners, box2D), corners.length);
            ground.CreateFixture(chain, 1);
        }
        {//top line
            const line=new b2EdgeShape();
            line.SetTwoSided(new b2Vec2(0,2),new b2Vec2(25,2));
            ground.CreateFixture(line,1);
        }
    }

    //make pump
    {
        const pump = new b2Pump(new b2Vec2(0,0.2));
        pump.SetAsBox(1,1,new b2Vec2(12.5,3),0);
        const bd = new b2BodyDef();
        bd.set_position(new b2Vec2(0,0));
        const body = world.CreateBody(bd);
        body.CreateFixture(pump,1);
    }

    // make particles
    const partSysDef = new b2ParticleSystemDef();
    partSysDef.radius = 0.05;
    partSysDef.dampingStrength = 0;
    const particleSystem = world.CreateParticleSystem(partSysDef);
    particleSystem.SetGravityScale(1);
    const amount = 1;
    for (let i = 0; i < amount; i++) {
        const pt = new b2ParticleGroupDef();
        const shape = new b2PolygonShape();
        shape.SetAsBox(5, 1, new b2Vec2(5, 3), 0);  //particle spawn area
        pt.shape = shape;
        //alpha is divided by 255 to get value between 0-1
        pt.set_color(new b2ParticleColor(100, 0, 255, 255));
        pt.set_position(new b2Vec2(5, 0));
        particleSystem.CreateParticleGroup(pt);
    }

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
        step(deltaMs);
        drawCanvas();
    }(window.performance.now()));
});