import { default as seagulls } from './seagulls.js'

import {Pane} from './tweakpane.min.js';


async function main() {
  const shader     = seagulls.constants.vertex + '\n'
                     + await seagulls.import('pixel-shader.wgsl'),
        compute    = await seagulls.import('compute-shader.wgsl'),
        sg         = await seagulls.init(),
        res        = [window.innerWidth, window.innerHeight],
        size       = 2*(window.innerWidth * window.innerHeight),
        state      = new Float32Array(size)

  for (let i=0; i<size; i+=2) {
    state[i]   = 1 // A
    state[i+1] = 0 // B
  }

  let x = Math.floor(window.innerWidth/2)
  let y = Math.floor(window.innerHeight/2)

  for (let _x=x-10; _x<=x+10; ++_x)  
    for (let _y=y-10; _y<=y+10; ++_y) {
      // if ((_x-x)**2+(_y-y)**2 > 50*50) continue
      // state[(_x+_y*res[0])*2+0] = 0;
      state[(_x+_y*res[0])*2+1] = 1;
    }  


  const PARAMS = { Da:1.0, 
                   Db:0.5, 
                   k:0.062, 
                   f:0.0545,
                   light_a: {r:255,g:0,b:0},
                   light_b: {r:0,g:0,b:255},
                   light_angle_a: 0,
                   light_angle_b: 0 };

  const pane = new Pane();  
  Object.keys(PARAMS).map(x=>{pane.addBinding(PARAMS, x)})


  sg.uniforms({res, 
               Da: PARAMS.Da, 
               Db: PARAMS.Db, 
               k:  PARAMS.k, 
               f:  PARAMS.f,
               light_a_rgb: [0,0,0],
               light_b_rgb: [0,0,0],
               light_angles: [0,0] })
    .buffers({stateA:state, stateB:state})
    .backbuffer(false)
    .onframe(()=>{
      sg.uniforms.Da = PARAMS.Da
      sg.uniforms.Db = PARAMS.Db
      sg.uniforms.k  = PARAMS.k
      sg.uniforms.f  = PARAMS.f

      sg.uniforms.light_a_rgb = [ PARAMS.light_a.r / 255, PARAMS.light_a.g / 255, PARAMS.light_a.b / 255 ]
      sg.uniforms.light_b_rgb = [ PARAMS.light_b.r / 255, PARAMS.light_b.g / 255, PARAMS.light_b.b / 255 ]
    
      sg.uniforms.light_angles = [ PARAMS.light_angle_a, PARAMS.light_angle_b ]
    })
    .pingpong(1)
    .compute(compute,
             [Math.ceil(window.innerWidth / 8), 
              Math.ceil(window.innerHeight / 8),
              1],
             {pingpong:['stateA']})
    .render(shader)
    .run()
}
main()