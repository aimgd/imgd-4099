@group(0) @binding(1) var<storage> pheromones: array<f32>;
@group(0) @binding(2) var<storage> render: array<f32>;

@fragment 
fn fs( @builtin(position) pos : vec4f ) -> @location(0) vec4f {
  let id = floor(pos.xy / vec2f(${CELL_SIZE}.,${CELL_SIZE}.));
  let idlinear = u32(id.x + id.y * ${buffer_size[0]}.);

  var color = vec3f();

  color = vec3f(select(0.1, 1.0, pheromones[idlinear] == 1.0));

  if (render[idlinear] == 1.0) {
    color = vec3(1.,0.4,0.4);
  }
  if (render[idlinear] == 2.0) {
    color = vec3(0.4,1.,0.4);
  }
  if (render[idlinear] == 3.0) {
    color = vec3(0.4,0.4,1.);
  }

  return vec4f(color,1.);
}



