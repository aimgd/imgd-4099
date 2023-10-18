struct Particle {
  pos: vec2f,
  vel: vec2f,
  age: f32,
};

@group(0) @binding(0) var<uniform> res:     vec2f;
@group(0) @binding(1) var<uniform> frame:   f32;

@group(0) @binding(2) var<uniform> curl_weight:   f32;
@group(0) @binding(3) var<uniform> wave_strength: f32;

@group(0) @binding(6) var<storage, read_write> state: array<Particle>;

fn _smoothstep(edge0: vec2f, edge1: vec2f, x: vec2f) -> vec2f {
    let t: vec2f = clamp((x - edge0) / (edge1 - edge0), vec2f(0.0), vec2f(1.0));
    return t * t * (3.0 - 2.0 * t);
}

fn rand22(n: vec2f) -> f32 { return fract(sin(dot(n, vec2f(12.9898, 4.1414))) * 43758.5453); }
fn noise2(n: vec2f) -> f32 {
  let d = vec2f(0., 1.);
  let b = floor(n);
  let f = _smoothstep(vec2f(0.), vec2f(1.), fract(n));
  return mix(mix(rand22(b), rand22(b + d.yx), f.x), mix(rand22(b + d.xy), rand22(b + d.yy), f.x), f.y);
}


fn _2noise2(n: vec2f) -> vec2f {
  return vec2f(noise2(n), noise2((n.yx) + vec2f(12.9898, .0)));
}

fn noise_drv(n: vec2f) -> vec2f {
  let v00 = noise2(n);
  let v10 = noise2(n + vec2f(0.0001,0.));
  let v01 = noise2(n + vec2f(0.,0.0001));
  return vec2f(v10 - v00, v01 - v00);
}

fn cellindex( cell:vec3u ) -> u32 {
  let size = 8u;
  return cell.x + (cell.y * size) + (cell.z * size * size);
}

@compute
@workgroup_size(8,8)

fn cs(@builtin(global_invocation_id) cell:vec3u)  {
  let i = cellindex( cell );
  var p = state[i];
  p.vel += (_2noise2(p.pos + vec2f(frame * 0.01, frame * 0.02)) * 2. - 1.) * 0.00005
           * wave_strength;
  let deriv = noise_drv(p.pos * 2. + vec2f(frame * 0.01, frame * 0.01));
  p.vel += vec2f(-deriv.y, deriv.x) * 0.5 * curl_weight;
  p.vel *= 0.99;
  p.pos += p.vel;
  p.age += 1.0;
  if (p.age > 400. || abs(p.pos.x)>=1. || abs(p.pos.y)>=1.) {
    p.pos=vec2f(rand22(p.vel), rand22(p.vel.yx))*2.-vec2f(1.);
    p.vel=vec2f(0.,0.);
    p.age = 0.;
  }
  state[i] = p;
}