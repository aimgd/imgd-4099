@group(0) @binding(0) var <uniform> res : vec2f;

@group(0) @binding(1) var <uniform> u_Da : f32;
@group(0) @binding(2) var <uniform> u_Db : f32;
@group(0) @binding(3) var <uniform> u_k  : f32;
@group(0) @binding(4) var <uniform> u_f  : f32;

@group(0) @binding(8) var<storage, read_write> statein: array<vec2f>;
@group(0) @binding(9) var<storage, read_write> stateout: array<vec2f>;


fn index( x:i32, y:i32 ) -> u32 {
  let _res = vec2i(res);
  return u32( (y % _res.y) * _res.x + ( x % _res.x ) );
}

@compute @workgroup_size(8,8)
fn cs(@builtin(global_invocation_id) _cell:vec3u) {
  let cell = vec3i(_cell);

  let old_state: vec2f = statein[index(cell.x, cell.y)];

  var laplacian: vec2f = vec2f(0.f);
  for (var y=-1; y<=1; y++) {
    for (var x=-1; x<=1; x++) {
      let val = abs(x) + abs(y);
      var coef: f32 = 0.;
      if (val == 0) {
        coef = -1.;
      }
      else if (val == 1) {
        coef = .2;
      }
      else if (val == 2) {
        coef = .05;
      }

      var _x: i32 = cell.x + x;
      var _y: i32 = cell.y + y;
      laplacian += coef * statein[index(_x, _y)];
    }
  }

  let dt  : f32 = 1.0;

  let new_state: vec2f = vec2f(
      clamp(old_state.x + (u_Da * laplacian.x 
                     - old_state.x * old_state.y * old_state.y
                     + u_f * (1. - old_state.x)) * dt, 0., 1.),
      clamp(old_state.y + (u_Db * laplacian.y 
                     + old_state.x * old_state.y * old_state.y
                     - (u_k + u_f) * old_state.y) * dt, 0., 1.)
      );

  if (cell.x >= 1 
   && cell.y >= 1 
   && cell.x < i32(res.x) - 1 
   && cell.y < i32(res.y) - 1) {
    stateout[index(cell.x, cell.y)] = new_state;
  }
}