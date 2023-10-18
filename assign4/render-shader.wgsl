struct VertexInput {
  @location(0) pos: vec2f,
  @builtin(instance_index) instance: u32,
};

struct Particle {
  pos: vec2f,
  vel: vec2f,
  age: f32,
};

@group(0) @binding(0) var<uniform> res:   vec2f;

@group(0) @binding(4) var<uniform> velocity_colouring_mul: f32;
@group(0) @binding(5) var<uniform> velocity_shaping_mul:   f32;

@group(0) @binding(6) var<storage> state: array<Particle>;

struct VertexIO {
  @builtin(position) pos: vec4f,
  @location(1) centre: vec2f,
  @location(2) age: f32,
  @location(3) vel: vec2f,
};

@vertex 
fn vs( input: VertexInput ) -> VertexIO {
  let size = input.pos * .03;
  let aspect = res.y / res.x;
  let p = state[ input.instance ];
  return VertexIO(
    vec4f( p.pos.x - size.x * aspect, p.pos.y + size.y, 0., 1.),
    p.pos,
    p.age,
    p.vel); 
}

@fragment 
fn fs( v: VertexIO ) -> @location(0) vec4f {;
  var velocity_colouring = length(v.vel) * 150.0;

  let trans: vec2f = v.pos.xy / res * vec2f(2.,-2.) + vec2f(-1.,1.);
  let diff: vec2f = (trans - v.centre) / .03;
  let aspect = res.y / res.x;
  var uv = diff / vec2f(aspect, 1.);

  var uv_xy = vec2f(
                  dot(uv, 
                      normalize(v.vel)
                  ),
                  dot(uv,
                      normalize(vec2f(-v.vel.y,v.vel.x))
                  )
              );

  uv_xy.y *= max(1.0, velocity_colouring * velocity_shaping_mul * 2.0);

  uv = uv_xy.x * normalize(v.vel)
     + uv_xy.y * normalize(vec2f(-v.vel.y,v.vel.x));

  if (length(uv) > 1.0) {
    discard;
  }

  let kernel = 1.0 - pow(length(uv), 3.);

  var age_coloring = min(v.age/100.0,1.0);
  if (v.age > 200.) {
    age_coloring = min(1.0-(v.age - 300.0)/100.0,1.0);
  }

  // return vec4f( , velocity_colouring, .1 );
  return vec4f( velocity_colouring * velocity_colouring_mul, 
                vec2f( abs(v.pos.x * 2. - 1.) / res.x, 
                abs(v.pos.y * 2. - 1.) / res.y) * 0.5
                  + v.vel * 150.0, 
                .2 * kernel * age_coloring );
  // return vec4f( v.centre.x, v.centre.y, velocity_colouring , .1 );
}