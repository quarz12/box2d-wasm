// https://nodejs.org/api/esm.html#esm_customizing_esm_specifier_resolution_algorithm
// NodeJS ES module resolution (as opposed to using explicit module resolution).
// this entrypoint is provided to support environments that have access to a bundler,
// or NodeJS when launched like so:
// node --experimental-specifier-resolution=node
import { simd } from 'wasm-feature-detect'

export default async (...args) => {
  const hasSIMD = await simd();
  const Box2DModule = await (
    hasSIMD
      ? import('../../build/flavour/simd/es/Box2D.simd.js')
      : import('../../build/flavour/standard/es/Box2D.js')
  );
  const { 'default': Box2DFactory } = Box2DModule;
  return await Box2DFactory(...args);
};