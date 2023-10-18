struct Vant {
  pos:  vec2f,
  dir:  f32,
  mode: f32,
  vel:  vec2f,
};

@group(0) @binding(0) var<storage, read_write> vants:     array<Vant>;
@group(0) @binding(1) var<storage, read_write> pheramone: array<f32>;
@group(0) @binding(2) var<storage, read_write> render:    array<f32>;

fn vantIdx(cell:vec2u) -> u32 {
  let size = ${WORKGROUP_SIZE}u;
  return cell.x + (cell.y * size);
}

fn pheramoneIdx(pos:vec2f) -> u32 {  
  let width = ${buffer_size[0]}.;
  return u32( abs( pos.y % ${buffer_size[1]}. ) * width + (pos.x % ${buffer_size[0]}.) );
}

@compute
@workgroup_size(${WORKGROUP_SIZE},${WORKGROUP_SIZE},1)
fn cs(@builtin(global_invocation_id) cell:vec3u) {
  let i = vantIdx( cell.xy );

  var s = vants[i];

  let dir = vec2f( sin(s.dir * ${Math.PI / 2}), 
                   cos(s.dir * ${Math.PI / 2}) );

  s.pos += dir;

  if (s.mode == 1.0) {
    if (pheramone[pheramoneIdx(s.pos)] == 1.0) {
      s.dir += 1.0;
      pheramone[pheramoneIdx(s.pos)] = 0.0;
    }
    else {
      s.dir -= 1.0;
      pheramone[pheramoneIdx(s.pos)] = 1.0;
    }
  }
  else if (s.mode == 2.0) {
    if (pheramone[pheramoneIdx(s.pos)] == 1.0) {
      pheramone[pheramoneIdx(s.pos)] = 0.0;
      s.vel.x += 1.0;
    }

    if (s.vel.x > 32.) {
      s.mode = 3.0;
      s.vel = vec2f(0.,0.);
    }

    var nearx = -10;
    var neary = -10;

    for (var y: i32 = -10; y <= 10; y++) {
      for (var x: i32 = -10; x <= 10; x++) {
        let v = pheramone[pheramoneIdx(s.pos + vec2f(f32(x),f32(y)))];

        if (v == 1.0) {
          if (abs(nearx) + abs(neary) > abs(x) + abs(y)) { 
            nearx = x;
            neary = y;
          }
        }
      }
    }


    if (abs(nearx) > abs(neary)) {
      if (nearx > 0) {
        s.dir = 1.;
      }
      else {
        s.dir = 3.;
      }
    }
    else {
      if (neary > 0) {
        s.dir = 0.;
      }
      else {
        s.dir = 2.;
      }
    }
  }
  else if (s.mode == 3.0) {
    for (var y: i32 = -4; y <= 4; y++) {
      for (var x: i32 = -4; x <= 4; x++) {
        let v = pheramone[pheramoneIdx(s.pos + vec2f(f32(x),f32(y)))];

        if (v == 1.0) {
          s.vel += vec2f(f32(-y), f32(x));
        }
      }
    }

    s.vel *= 0.99;
    pheramone[pheramoneIdx(s.pos)] = 1.0;

    if (length(s.vel) > 250.0) {
      s.mode = 2.0;
    }

    if (abs(s.vel.x) > abs(s.vel.y)) {
      if (s.vel.x > 0.) {
        s.dir = 1.;
      }
      else {
        s.dir = 3.;
      }
    }
    else {
      if (s.vel.y > 0.) {
        s.dir = 0.;
      }
      else {
        s.dir = 2.;
      }
    }

  }

  s.pos = s.pos % vec2f(${buffer_size.map(x=>x+".").join(',')});
  vants[i] = s;


  render[pheramoneIdx(s.pos)] = s.mode;
}