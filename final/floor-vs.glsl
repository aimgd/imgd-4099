#version 430

uniform mat4 _proj;
uniform mat4 _view;

uniform vec3 _bbox_size;

void main() {
	vec3 pos = vec3(0);

	switch (gl_VertexID) {
		case 0: pos = vec3(0, 0, 0); break;
		case 1: pos = vec3(_bbox_size.x, 0, 0); break;
		case 2: pos = vec3(_bbox_size.x, 0, _bbox_size.z); break;
		case 3: pos = vec3(0, 0, 0); break;
		case 4: pos = vec3(_bbox_size.x, 0, _bbox_size.z); break;
		case 5: pos = vec3(0, 0, _bbox_size.z); break;
	}

	gl_Position = ((vec4(pos,1)) * _view) * _proj;
}