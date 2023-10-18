#version 430

#define WORKGROUP_SIZE 2
layout (local_size_x = WORKGROUP_SIZE, local_size_y = WORKGROUP_SIZE, local_size_z = WORKGROUP_SIZE) in;



struct SphParticle {
	vec3 pos;
	float density;
	vec3 vel;
	float _pad2;
};

uniform float _particle_count;
uniform vec3  _bbox_size;

layout(std140, binding = 0) buffer Ssbo0 {
  SphParticle particle[];
} state_in;

layout(std140, binding = 1) buffer Ssbo1 {
  SphParticle particle[];
} state_out;

// int linearId(uvec3 v) {
// 	return v.x +
// }

#define DT (1.0/120.0)
#define PI 3.14159265358979

uniform float _sph_mass;
uniform float _sph_radius;
uniform float _target_density;
uniform float _pressure_mul;

float crappyDensityToPressure(float density) {
	float error = density - _target_density;
	return error * _pressure_mul;
}

float smoothingFunc(float dst) {
	if (dst >= _sph_radius) return 0.;

	float vol = (PI * pow(_sph_radius, 4.0)) / 6.0;
	return (_sph_radius - dst) * (_sph_radius - dst) / vol;
}

float smoothingFuncDer(float dst) {
	if (dst >= _sph_radius) return 0.;

	float scale = 12.0 / (pow(_sph_radius, 4.0) * PI);
	return (dst - _sph_radius) * scale;
}


float computeDensity(int i) {
	SphParticle p = state_in.particle[i];
	float density = 0.0;
	for (int x = 0; x < _particle_count; ++x) {
		// if (x == i) continue;
		SphParticle pi = state_in.particle[x];

		float dist = distance(pi.pos + pi.vel * DT, p.pos + p.vel * DT);
		density += _sph_mass * smoothingFunc(dist);
	}
	return density;
}

uint linearId() {
	return gl_GlobalInvocationID.x; // + gl_GlobalInvocationID.y * WORKGROUP_SIZE + gl_GlobalInvocationID.z * WORKGROUP_SIZE * WORKGROUP_SIZE;
}

void main() {
	uint linear_id = linearId();
	if (linear_id < _particle_count) {
		SphParticle p = state_in.particle[linear_id];
		p.density = computeDensity(int(linear_id));
		state_out.particle[linear_id] = p;
	}
}

