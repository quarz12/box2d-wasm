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
import { makeDebugDraw } from './debugDraw.js';

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
    b2ParticleSystem,
    b2ParticleSystemDef,
    b2RevoluteJoint,
    b2RevoluteJointDef,
    JSQueryCallback,
    castObject,
    destroy,
    getPointer,
    HEAPF32,
    NULL,
      b2ParticleDef,
      wrapPointer,
    b2ChainShape,
    b2ParticleColor,
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

  const gravity = new b2Vec2(0, 10);
  const world = new b2World(gravity);

  const bd_ground = new b2BodyDef();
  const ground = world.CreateBody(bd_ground);

  //edges
  {
    {//left
    const chain = new b2ChainShape();
    let corners=[new b2Vec2(0,21),new b2Vec2(0,0),new b2Vec2(25,0),new b2Vec2(25,21)];

        const buffer = _malloc(corners.length * 8);
        let offset = 0;
        for (let i=0; i<corners.length; i++) {
          HEAPF32[buffer + offset >> 2] = corners[i].get_x();
          HEAPF32[buffer + (offset + 4) >> 2] = corners[i].get_y();
          offset += 8;
        }
        const ptr_wrapped = wrapPointer(buffer, b2Vec2);
        chain.CreateLoop(ptr_wrapped, corners.length);

    ground.CreateFixture(chain,1);
    /*
    shape.SetTwoSided(new b2Vec2(0,21),new b2Vec2(0,0));
    ground.CreateFixture(shape,1);
  }

  {//right
    const shape = new b2EdgeShape();
    shape.SetTwoSided(new b2Vec2(25,0),new b2Vec2(25,21));
    ground.CreateFixture(shape,1);
  }
  {//floor
    const shape = new b2EdgeShape();
    shape.SetTwoSided(new b2Vec2(25,21),new b2Vec2(0,21));
    ground.CreateFixture(shape,1);
  }
  {//ceiling
    const shape = new b2EdgeShape();
    shape.SetTwoSided(new b2Vec2(0,0),new b2Vec2(25,0));
    ground.CreateFixture(shape,1);
  }*/
  }}

  const sideLengthMetres = 1;
  const square = new b2PolygonShape();
  square.SetAsBox(sideLengthMetres/2, sideLengthMetres/2);
  const circle = new b2CircleShape();
  circle.set_m_radius(sideLengthMetres/2);

  const ZERO = new b2Vec2(0, 0);
  const temp = new b2Vec2(0, 0);
  /**
   * @param {Box2D.b2Body} body
   * @param {number} index
   * @returns {void}
   */
  const initPosition = (body, index) => {
    temp.Set(4 + sideLengthMetres*(Math.random()-0.5), -sideLengthMetres*index);
    body.SetTransform(temp, 0);
    body.SetLinearVelocity(ZERO);
    body.SetAwake(1);
    body.SetEnabled(1);
  }

  // make falling boxes
  // const boxCount = 10;
  // for (let i = 0; i < boxCount; i++) {
  //   const bd = new b2BodyDef();
  //   bd.set_type(b2_dynamicBody);
  //   bd.set_position(ZERO);
  //   const body = world.CreateBody(bd);
  //   body.CreateFixture(i % 2 ? square : circle, 1);
  //   initPosition(body, i);
  // }

  // make particles
  const partSysDef=new b2ParticleSystemDef();
  partSysDef.radius=0.05;
  partSysDef.dampingStrength=0.1;
  const particleSystem=world.CreateParticleSystem(partSysDef);
  const amount =1;
  for (let i = 0; i < amount; i++) {
    const pt=new b2ParticleGroupDef();
    const shape=new b2PolygonShape();
    shape.SetAsBox(1,1,new b2Vec2(1,0),0);
    pt.shape=shape;
    pt.set_color(new b2ParticleColor(100,0,100,1));
    pt.set_position(new b2Vec2(5,0));
    particleSystem.CreateParticleGroup(pt);
  }

  const debugDraw = makeDebugDraw(ctx, pixelsPerMeter, box2D);
  world.SetDebugDraw(debugDraw);

  // calculate no more than a 60th of a second during one world.Step() call
  const maxTimeStepMs = 1/60*1000;
  /** @param {number} deltaMs */
  const step = (deltaMs) => {
    const clampedDeltaMs = Math.min(deltaMs, maxTimeStepMs);
    world.Step(clampedDeltaMs/1000, 3, 2, 3);
  };
  const drawCanvas = () => {
    ctx.fillStyle = 'rgb(255,255,255)';  //set background color
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.save();
    ctx.scale(pixelsPerMeter, pixelsPerMeter);
    const { x, y } = cameraOffsetMetres;
    ctx.translate(x, y);
    ctx.lineWidth /= pixelsPerMeter;
    ctx.lineWidth*=3;
    
    world.DebugDraw();

    ctx.restore();
  };

  /** @type {?number} */
  let handle;
  (function loop(prevMs) {
    const nowMs = window.performance.now();
    handle = requestAnimationFrame(loop.bind(null, nowMs));
    const deltaMs = nowMs-prevMs;
    step(deltaMs);
    drawCanvas();
  }(window.performance.now()));
});