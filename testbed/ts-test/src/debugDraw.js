/**
 * Forked from Box2D.js
 * @see https://github.com/kripken/box2d.js/blob/f75077b/helpers/embox2d-helpers.js
 * @author dmagunov + Huy Nguyen + fork contributions from Alex Birch
 * @see https://github.com/kripken/box2d.js/blob/49dddd6/helpers/embox2d-html5canvas-debugDraw.js
 * @author dmagunov + fork contributions from Alex Birch
 * @license Zlib https://opensource.org/licenses/Zlib
 * License evidence: https://github.com/kripken/box2d.js/blob/master/README.markdown#box2djs
 *   "box2d.js is zlib licensed, just like Box2D."
 * 
 * @typedef {import('box2d-wasm')} Box2DFactory
 * @param {CanvasRenderingContext2D} ctx
 * @param {number} pixelsPerMeter
 * @param {typeof Box2D & EmscriptenModule} box2D
 */
export const makeDebugDraw = (ctx, pixelsPerMeter, box2D) => {
  const {
    b2Color,
    b2Draw: { e_shapeBit, e_particleBit, e_aabbBit},
    b2Transform,
    b2Vec2,
    JSDraw,
    wrapPointer,
      b2ParticleColor
  } = box2D;

  /**
   * to replace original C++ operator =
   * @param {Box2D.b2Vec2} vec
   * @returns {Box2D.b2Vec2}
   */
  const copyVec2 = vec =>
    new b2Vec2(vec.get_x(), vec.get_y());

  /**
   * to replace original C++ operator *= (float) 
   * @param {Box2D.b2Vec2} vec
   * @param {number} scale
   * @returns {Box2D.b2Vec2}
   */
  const scaledVec2 = (vec, scale) =>
    new b2Vec2(scale * vec.get_x(), scale * vec.get_y());
    
  /**
   * @param {Box2D.b2Color|Box2D.b2ParticleColor} color
   * @param {string} type "b2Color" or "b2ParticleColor"
   * @returns {string}
   */
  const getRgbaStr = (color, type) => {
    if(type==="b2Color") {
      const red = (color.get_r() * 255) | 0;
      const green = (color.get_g() * 255) | 0;
      const blue = (color.get_b() * 255) | 0;
      let alpha = (color.get_a()) | 0;
      return `${red},${green},${blue},${alpha}`;
    }
    else if (type==="b2ParticleColor"){
      const red = color.get_r() | 0;
      const green = color.get_g() | 0;
      const blue = color.get_b() | 0;
      const alpha = (color.get_a()/255);
      return `${red},${green},${blue},${alpha}`;
    }
  };

  /**
   * @param {string} rgbaStr
   * @returns {void}
   */
  const setCtxColor = (rgbaStr) => {
    ctx.fillStyle = `rgba(${rgbaStr})`;
    ctx.strokeStyle = `rgba(${rgbaStr})`;
  };

  /**
   * @param {Box2D.b2Vec2[]} vertices
   * @param {boolean} fill
   * @returns {void}
   */
  const drawPolygon = (vertices, fill) => {
    ctx.beginPath();
    let first = true;
    for (const vertex of vertices) {
      if (first) {
        ctx.moveTo(vertex.get_x(), vertex.get_y());
        first = false;
      } else {
        ctx.lineTo(vertex.get_x(), vertex.get_y());
      }
    }
    ctx.closePath();
    if (fill) {
      ctx.fill();
    }
    ctx.stroke();
  };

  /**
   * @param {Box2D.b2Vec2} center
   * @param {number} radius
   * @param {Box2D.b2Vec2} axis
   * @param {boolean} fill bad for performance
   * @returns {void}
   */
  const drawCircle = (center, radius, axis, fill) => {
      ctx.beginPath();
      ctx.arc(center.get_x(), center.get_y(), radius, 0, 2 * Math.PI, false);
      // fill=true;
      if (fill) {
        ctx.fill();
      }
      ctx.stroke();
    if (fill) {
      //render axis marker
      const vertex = copyVec2(center);
      vertex.op_add(scaledVec2(axis, radius));
      ctx.beginPath();
      ctx.moveTo(center.get_x(), center.get_y());
      ctx.lineTo(vertex.get_x(), vertex.get_y());
      ctx.stroke();
    }
  };

  /**
   * @param {Box2D.b2Vec2} center
   * @param {number} radius
   * @param {Box2D.b2Vec2} axis
   * @param {number} startRadians
   * @param {number} endRadians
   * @returns {void}
   */
  const drawArc = (center, radius, axis, startRadians, endRadians ) => {
      ctx.beginPath();
      ctx.arc(center.get_x(), center.get_y(), radius, startRadians, endRadians, false);
      ctx.stroke();
  };

  /**
   * @param {Box2D.b2Vec2} vert1
   * @param {Box2D.b2Vec2} vert2
   * @returns {void}
   */
  const drawSegment = (vert1, vert2) => {
    ctx.beginPath();
    ctx.moveTo(vert1.get_x(), vert1.get_y());
    ctx.lineTo(vert2.get_x(), vert2.get_y());
    ctx.stroke();
  };

  /**
   * @param {Box2D.b2Vec2} vertex
   * @param {number} sizeMetres
   * @returns {void}
   */
  const drawPoint = (vertex, sizeMetres) => {
    const sizePixels = sizeMetres/pixelsPerMeter;
    ctx.fillRect(
      vertex.get_x()-sizePixels/2,
      vertex.get_y()-sizePixels/2,
      sizePixels,
      sizePixels
      );
  };

  /**
   * @param {Box2D.b2Transform} transform
   * @param {number} sizeMetres
   * @returns {void}
   */
  const drawTransform = transform => {
    const pos = transform.get_p();
    const rot = transform.get_q();
    
    ctx.save();
    ctx.translate(pos.get_x(), pos.get_y());
    ctx.scale(0.5, 0.5);
    ctx.rotate(rot.GetAngle());
    ctx.lineWidth *= 2;
    ctx.restore();
  }

  /** {@link Box2D.b2Vec2} is a struct of `float x, y` */
  const sizeOfB2Vec = Float32Array.BYTES_PER_ELEMENT * 2;
  /** {@link Box2D.b2Color} is a struct of `float r,g,b` */
  const  sizeOfB2ParticleColor= Uint8Array.BYTES_PER_ELEMENT*4

  /**
   * rebuild array from pointer to its first element
   * @param {number} array_p pointer to {@link Box2D.b2Vec2}
   * @param {number} numElements length of array
   * @param {number} sizeOfElement size of an instance of the array element
   * @param {typeof Box2D.b2Vec2| typeof Box2D.b2ParticleColor} ctor constructor for the array element
   * @return {Box2D.b2Vec2[]|Box2D.b2ParticleColor[]}
   */
  const reifyArray = (array_p, numElements, sizeOfElement, ctor) =>
    Array(numElements)
      .fill(undefined)
      .map((_, index) =>
        wrapPointer(array_p + index * sizeOfElement, ctor)
      );

  const dummy0Vec=new b2Vec2(0, 0);
  const debugDraw = Object.assign(new JSDraw(), {
    /**
     * @param {number} vert1_p pointer to {@link Box2D.b2Vec2}
     * @param {number} vert2_p pointer to {@link Box2D.b2Vec2}
     * @param {number} color_p pointer to {@link Box2D.b2Color}
     * @returns {void}
     * Draws a line
     */
    DrawSegment(vert1_p, vert2_p, color_p) {
      const color = wrapPointer(color_p, b2Color);
      setCtxColor(getRgbaStr(color,"b2Color"));
      setCtxColor("0,0,0,1")
      const vert1 = wrapPointer(vert1_p, b2Vec2);
      const vert2 = wrapPointer(vert2_p, b2Vec2);
      drawSegment(vert1, vert2);
    },
    /**
     * @param {number} vertices_p pointer to Array<{@link Box2D.b2Vec2}>
     * @param {number} vertexCount
     * @param {number} color_p pointer to {@link Box2D.b2Color}
     * @returns {void}
     */
    DrawPolygon(vertices_p, vertexCount, color_p) {
      const color = wrapPointer(color_p, b2Color);
      setCtxColor(getRgbaStr(color,"b2Color"));
      const vertices = reifyArray(vertices_p, vertexCount, sizeOfB2Vec, b2Vec2);
      drawPolygon(vertices, vertexCount, false);
    },
    /**
     * @param {number} vertices_p pointer to Array<{@link Box2D.b2Vec2}>
     * @param {number} vertexCount
     * @param {number} color_p pointer to {@link Box2D.b2Color}
     * @returns {void}
     */
    DrawSolidPolygon(vertices_p, vertexCount, color_p) {
      const color = wrapPointer(color_p, b2Color);
      setCtxColor(getRgbaStr(color,"b2Color"));
      const vertices = reifyArray(vertices_p, vertexCount, sizeOfB2Vec, b2Vec2);
      drawPolygon(vertices, vertexCount, true);
    },
    /**
     * @param {number} center_p pointer to {@link Box2D.b2Vec2}
     * @param {number} radius
     * @param {number} color_p pointer to {@link Box2D.b2Color}
     * @returns {void}
     */
    DrawCircle(center_p, radius, color_p) {
      const color = wrapPointer(color_p, b2Color);
      setCtxColor(getRgbaStr(color,"b2Color"));
      const center = wrapPointer(center_p, b2Vec2);
      drawCircle(center, radius, dummy0Vec, false);
    },
    /**
     * @param {number} center_p pointer to {@link Box2D.b2Vec2}
     * @param {number} radius
     * @param {number} axis_p pointer to {@link Box2D.b2Vec2}
     * @param {number} color_p pointer to {@link Box2D.b2Color}
     * @returns {void}
     */
    DrawSolidCircle(center_p, radius, axis_p, color_p) {
      const color = wrapPointer(color_p, b2Color);
      setCtxColor(getRgbaStr(color,"b2Color"));
      const center = wrapPointer(center_p, b2Vec2);
      const axis = wrapPointer(axis_p, b2Vec2);
      drawCircle(center, radius, axis, true);
    },
    /**
     * @param {number} transform_p pointer to {@link Box2D.b2Transform}
     * @returns {void}
     */
    DrawTransform(transform_p) {
      const transform = wrapPointer(transform_p, b2Transform);
      drawTransform(transform);
    },
    /**
     * @param {number} vertex_p pointer to {@link Box2D.b2Vec2}
     * @param {number} sizeMetres
     * @param {number} pointer to {@link Box2D.b2Color}
     * @returns {void}
     */
    DrawPoint(vertex_p, sizeMetres, color_p) {
      const color = wrapPointer(color_p, b2Color);
      setCtxColor(getRgbaStr(color,"b2Color"));
      const vertex = wrapPointer(vertex_p, b2Vec2);
      drawPoint(vertex, sizeMetres);
    },

    /**
     *
     * @param {number} centers_p pointer to {@link Box2D.b2Vec2}
     * @param {number} radius
     * @param {number} colors_p pointer to {@link Box2D.b2Color}
     * @param {number} count
     * @returns {void}
     */
    DrawParticles(centers_p, radius, colors_p, count){
      const centers=reifyArray(centers_p, count, sizeOfB2Vec, b2Vec2);
      const colors=reifyArray(colors_p, count, sizeOfB2ParticleColor, b2ParticleColor);
      for (let i = 0; i < count; i++) {
        const center=centers[i];
        const color=colors[i];
        setCtxColor(getRgbaStr(color,"b2ParticleColor"));
        let tmp=ctx.lineWidth;
        ctx.lineWidth*=0.5;
        drawCircle(center, radius, dummy0Vec, false);
        ctx.lineWidth=tmp;
      }
  },
    /**
     * @param {number} center_p pointer to {@link Box2D.b2Vec2}
     * @param {number} radius
     * @param {number} start
     * @param {number} end
     * @param {number} color_p to {@link Box2D.b2Color}
     * @returns {void}
     */
    DrawArc(center_p, radius, start, end, color_p){
      const color = wrapPointer(color_p, b2Color);
      setCtxColor(getRgbaStr(color,"b2Color"));
      setCtxColor("0,0,0,1");
      const center = wrapPointer(center_p, b2Vec2);
      drawArc(center, radius, dummy0Vec, start, end);
    }
  });
  debugDraw.SetFlags(e_shapeBit | e_particleBit);
  return debugDraw;
};
export const vecArrToPointer=(arr,box2D)=>{
  const {
    HEAPF32,
      _malloc,
      wrapPointer,
      b2Vec2
  } = box2D;
  const buffer = _malloc(arr.length * 8);
  let offset = 0;
  for (let i=0; i<arr.length; i++) {
    HEAPF32[buffer + offset >> 2] = arr[i].get_x();
    HEAPF32[buffer + (offset + 4) >> 2] = arr[i].get_y();
    offset += 8;
  }
  return wrapPointer(buffer, b2Vec2);
};
/**
 * array of tuples to array of vectors
 * @param arr
 * @param box2D
 * @returns {Box2D.b2Vec2[]}
 */
export const arrToVecArr=(arr,box2D)=>{
  const {
    b2Vec2
  }=box2D;

  return arr.map(tuple=>new b2Vec2(tuple[0],tuple[1]));
}