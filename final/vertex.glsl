#version 430

uniform mat4 _proj;
uniform mat4 _view;

in vec3 pos;

struct SphParticle {
	vec3 pos;
	float density;
	vec3 vel;
	float _pad2;
};


layout(std140, binding = 1) buffer Ssbo {
  SphParticle particle[];
} ssbo;

out vec3 v_color;

uniform float _target_density;

void main() {
	float density = ssbo.particle[gl_InstanceID].density - _target_density; // - _target_density;
	v_color = max(dot(normalize(pos),normalize(vec3(1))),0.1) * (vec3(0,0,1) + vec3(1,0,0) * length(ssbo.particle[gl_InstanceID].vel) / 5.0);
	// v_color = vec3(density, 0, -density) * max(dot(normalize(pos),normalize(vec3(1))),0.3);
	gl_Position = ((vec4(pos + ssbo.particle[gl_InstanceID].pos,1)) * _view) * _proj;
}
