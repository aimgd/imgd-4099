import { default as seagulls } from './seagulls.js'

import {Pane} from './tweakpane.min.js';


async function main() { 
  const render     = await seagulls.import('render-shader.wgsl'),
        compute    = await seagulls.import('compute-shader.wgsl'),
        sg         = await seagulls.init(),
        res        = [window.innerWidth, window.innerHeight],
        PARTICLE_COUNT  = 4096,
        PARTICLE_STRIDE = 6,
        WORKGROUP_SIZE  = 8,
        state      = new Float32Array(PARTICLE_COUNT * PARTICLE_STRIDE)

  for( let i = 0; i < PARTICLE_COUNT * PARTICLE_STRIDE; i+= PARTICLE_STRIDE ) {
    state[ i ] = -1 + Math.random() * 2
    state[ i + 1 ] = -1 + Math.random() * 2
    state[ i + 2 ] = Math.random() * 0.001 - 0.0005
    state[ i + 3 ] = Math.random() * 0.001 - 0.0005
    state[ i + 4 ] = 0
  }

  // example of cool settings:
  // curl-weight        1.70
  // wave-strength      2.60
  // velocity-coloring  14.10
  // velocity-shaping   1.90

  const PARAMS = {
    curl_weight: 4.30,
    wave_strength: 3.80,
    velocity_coloring: 18.70,
    velocity_shaping: 1.0
  };

  const pane = new Pane();  
  Object.keys(PARAMS).map(x=>{pane.addBinding(PARAMS, x)})



  let frame = 0

  sg.uniforms({
      res, 
      frame, 
      curl_weight:        PARAMS.curl_weight, 
      wave_strength:      PARAMS.wave_strength, 
      velocity_coloring:  PARAMS.velocity_coloring, 
      velocity_shaping:   PARAMS.velocity_shaping})
    .onframe(()=>{ 
      sg.uniforms.frame = frame++ 
      sg.uniforms.curl_weight       = PARAMS.curl_weight;
      sg.uniforms.wave_strength     = PARAMS.wave_strength;
      sg.uniforms.velocity_coloring = PARAMS.velocity_coloring;
      sg.uniforms.velocity_shaping  = PARAMS.velocity_shaping;
    })
    .buffers({state})
    .backbuffer(false)
    .blend(true)
    .compute(compute, PARTICLE_COUNT / (WORKGROUP_SIZE * WORKGROUP_SIZE))
    .render(render)
    .run( PARTICLE_COUNT )
}
main()