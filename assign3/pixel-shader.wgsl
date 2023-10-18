@group(0) @binding(0) var <uniform> res : vec2f;
@group(0) @binding(8) var<storage> state: array<vec2f>;

@group(0) @binding(5) var <uniform> light_color_a : vec3f;
@group(0) @binding(6) var <uniform> light_color_b : vec3f;
@group(0) @binding(7) var <uniform> light_angles  : vec2f;


fn index( at: vec2i ) -> u32 {
  return u32(at.y) * u32(res.x) + u32(at.x);
}

@fragment
fn fs( @builtin(position) pos : vec4f ) -> @location(0) vec4f {
  let idx : u32 = index(vec2i(pos.xy));
  
  let v = state[idx];

  let v10 = state[index(vec2i(pos.xy) + vec2i(1,0))];
  let v01 = state[index(vec2i(pos.xy) + vec2i(0,1))];

  let gradient = vec4f(v10 - v, 
                       v01 - v);

  let norms = vec4((gradient.xz) / (1. / res.x),
                   (gradient.yw) / (1. / res.y));

  let light_dir_a = vec2f(cos(light_angles.x), sin(light_angles.x));
  let light_dir_b = vec2f(cos(light_angles.y), sin(light_angles.y));

  // let light_color_a = vec3f(1.,0.,0.);
  // let light_color_b = vec3f(0.,0.,1.);

  return vec4f(mix(vec3f(1.),
                 0.2 * max(0., dot(light_dir_a, norms.xy)) * light_color_a +
                 0.2 * max(0., dot(light_dir_b, norms.zw)) * light_color_b,
                 v.x * v.x),
               1.);
}
