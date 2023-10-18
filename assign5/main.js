import { default as seagulls } from './seagulls.js'

import {Pane} from './tweakpane.min.js'


const WORKGROUP_SIZE = 4,
      PROPERTIES     = 6,
      CELL_SIZE      = 16,
      res         = [window.innerWidth, window.innerHeight],
      buffer_size = [Math.floor(res[0]/CELL_SIZE), Math.floor(res[1]/CELL_SIZE)]


const render      = seagulls.constants.vertex + '\n' +
                    eval(`\`${await seagulls.import('render-shader.wgsl')}\``),
      compute     = eval(`\`${await seagulls.import('compute-shader.wgsl')}\``),
      sg          = await seagulls.init()

const vants     = new Float32Array(PROPERTIES * WORKGROUP_SIZE * WORKGROUP_SIZE),
      pheramone = new Float32Array(buffer_size[0] * buffer_size[1]),
      gfx_data  = new Float32Array(buffer_size[0] * buffer_size[1]) 

for (let i = 0; i < vants.length; i += PROPERTIES) {
  vants[i + 0] = Math.floor(Math.random() * buffer_size[0])
  vants[i + 1] = Math.floor(Math.random() * buffer_size[1])
  vants[i + 2] = Math.floor(Math.random() * 4)
  vants[i + 3] = Math.floor(Math.random() * 3) + 1
}

for (let i = 0; i < pheramone.length; ++i) {
  pheramone[i] = Math.random() > 0.95 ? 1 : 0
}

const PARAMS = {
}

const pane = new Pane()
Object.keys(PARAMS).map(x=>{pane.addBinding(PARAMS, x)})

let frame = 0
sg.buffers({vants, pheramone, gfx_data})
  .backbuffer(false)
  .onframe(()=> {
    sg.buffers.gfx_data.clear()
  })
  .compute(compute, 1)
  .render(render)
  .run(1, 60)
