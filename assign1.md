
## Assignment 1:

The aesthetic intent of this shader was to create a fractal zoom that would react to and match with music playing at the same time. The shader shows a Mandelbrot fractal that zooms while rotating and applying other linear transformations to get different visual behaviours. These linear transformations applied in screen-space are shears and looked the nicest while not interfering with the visuals of the fractal like music based translation would have. The fractal is also scaled in different axes by the music, with each axis having a different frequency response, but in a different transformation space as doing it in screen space would not match the orientation of the fractal and get weird scaling behaviour. The other thing I experimented with was having the color respond to the music, and after trying a few different configurations, having each component of the color sync up with a different range of frequencies in the music had the most interesting effect, which was achieved through taking the Hadamard product of the color and the audio vector.



```wgsl
@fragment 
fn fs( @builtin(position) pos : vec4f ) -> @location(0) vec4f {
  // get normalized texture coordinates (aka uv) in range 0-1
  let npos  = uvN( pos.xy ); // + mouse.xy - 0.5;
  let co = (npos * 2.0 - vec2f(1.0)) * vec2f(res.x / res.y, 1.0);

  let t = frame / 600.0; 
  var c = vec2f(cos(t), sin(t)) * co.x + vec2f(-sin(t), cos(t)) * co.y;


  var A = vec2f(0.4,0.15);
  c = (c - A);
  

  c.x += (audio.x + audio.y) * c.x * 0.2;
  c.y += audio.z * c.y * 0.5;


c = c / exp2(f32((frame - 600.0) / 1000.0)) + A;

  
  var z : vec2f = c;
  var it: f32 = 0.0;
  for (var i = 0; i <132; i++)
  {
    // z = (a+bi)^2
    // (a+bi)(a+bi) = a^2 + 2abi - b^2
    z = vec2f(dot(z,vec2f(z.x,-z.y)), 2.0 * z.x * z.y) + c;
    it += 1.0;
    if (length(z) > 2.0)
    {
      break;
    }
  }

  
  var v: f32 =  pow(it / 132.0, 0.6 - 0.3 * dot(audio, vec3f(0.3,1.0,1.0)));
  var col = normalize(vec4f(0.5 + abs(cos(frame/232.0 + co.x * 3.0)),0.1 + sin(frame / 50.0 + co.y*3.),0.3,1.0));
  col.x *= audio.x;
  col.y *= audio.y;
  col.z *= audio.z;

  return mix( mix(vec4f(0.1), col, vec4f(smoothstep(v,0.0,0.25))), vec4f(1.), saturate(vec4f(pow(v*0.7,2.0))));
}
```
