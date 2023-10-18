#include <stdint.h>

#include <SDL2/SDL.h>
#include "ext/glad/glad.h"

#include "ext/glad.c"
#include <assert.h>

#include "maths.h"

#define GL(x) do { { x; } assert(glGetError() == 0); } while(0);

char* loadTextFile(const char* path) {
	FILE* f = fopen(path, "rb");
	if (!f) {
		printf("%s doesnt exist!\n", path);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* mem = (char*)malloc(len + 1);
	fread(mem, 1, len, f);
	fclose(f);
	mem[len] = 0;
	return mem;
}


template<GLuint Type>
struct Buffer {
	static_assert(Type == GL_ARRAY_BUFFER ||
							  Type == GL_SHADER_STORAGE_BUFFER);
	u32 id;
	size_t size;

	static Buffer<Type> make(void* data, size_t size) {
		Buffer<Type> buf;
		buf.size = size;
		glCreateBuffers(1, &buf.id);
		glNamedBufferStorage(buf.id, size, data, GL_DYNAMIC_STORAGE_BIT);
		return buf;
	}

	void bind() {
		GL(glBindBuffer(Type, id));
	}

	void bindSsbo(uint32_t index) {
		static_assert(Type == GL_SHADER_STORAGE_BUFFER);
		GL(glBindBufferBase(Type, index, id));
	}

	void destroy() {
		GL(glDeleteBuffers(1, &id));
	}
};



struct Shader {
	u32 id;

	static Shader make() {
		Shader shader;
		GL(shader.id = glCreateProgram());
		return shader;
	}

	template<GLuint ShaderType>
	Shader& addStage(const char* path) {
		static_assert(ShaderType == GL_VERTEX_SHADER ||
									ShaderType == GL_FRAGMENT_SHADER ||
									ShaderType == GL_COMPUTE_SHADER);
		char* shader_code = loadTextFile(path);
		if (!shader_code) {
			printf("Failed to add shader!\n");
			free(shader_code);
			return *this;
		}

		u32 shader_part;
		GL(shader_part = glCreateShader(ShaderType));
		GL(glShaderSource(shader_part, 1, &shader_code, NULL));
		GL(glCompileShader(shader_part));
		free(shader_code);

		{
			char msg_buf[1024];
			GL(glGetShaderInfoLog(shader_part, sizeof(msg_buf), NULL, msg_buf));
			puts(msg_buf);
		}

		GL(glAttachShader(id, shader_part));
		GL(glDeleteShader(shader_part));

		return *this;
	}

	Shader& link() {
		GL(glLinkProgram(id));
		{
			char msg_buf[1024];
			GL(glGetProgramInfoLog(id, sizeof(msg_buf), NULL, msg_buf));
			puts(msg_buf);
		}

		return *this;
	}

	void setUniform(const char* uniform, f32 x) {
		GL(glUseProgram(id));
		GL(glUniform1f(glGetUniformLocation(id, uniform), x));
	}

	void setUniform(const char* uniform, Vec2 v) {
		GL(glUseProgram(id));
		GL(glUniform2f(glGetUniformLocation(id, uniform), v.x, v.y));
	}
	void setUniform(const char* uniform, Vec3 v) {
		GL(glUseProgram(id));
		GL(glUniform3f(glGetUniformLocation(id, uniform), v.x, v.y, v.z));
	}
	void setUniform(const char* uniform, Vec4 v) {
		GL(glUseProgram(id));
		GL(glUniform4f(glGetUniformLocation(id, uniform), v.x, v.y, v.z, v.w));
	}

	void setUniform(const char* uniform, Mat4 m) {
		GL(glUseProgram(id));
		GL(glUniformMatrix4fv(glGetUniformLocation(id, uniform), 1, false, m.m));
	}


	void destroy() {
		GL(glDeleteProgram(id));
	}

	void execute(u32 x_groups, u32 y_groups, u32 z_groups) {
		GL(glUseProgram(id));
		GL(glDispatchCompute(x_groups, y_groups, z_groups));
	}
};


struct MeshVertex {
	Vec3 pos;
};

struct Mesh {
	u32 num_verts;
	MeshVertex* verts;


	static Mesh make(u32 num_verts) {
		Mesh mesh;
		mesh.num_verts = num_verts;
		mesh.verts = (MeshVertex*)calloc(num_verts, sizeof(MeshVertex));
		return mesh;
	}

	static Mesh makeSphere(f32 radius, u32 theta_segments, u32 rho_segments) {
		Mesh mesh = Mesh::make(theta_segments * rho_segments * 6);

		u32 builder_idx = 0;

		const f32 theta_step = 2 * PI / theta_segments;
		const f32 rho_step = PI / theta_segments;

		auto sphericalToCartesian = [](f32 theta, f32 rho) -> Vec3 {
			return Vec3 {
				sinf(theta) * sinf(rho),
				cosf(rho),
				cosf(theta) * sinf(rho)
			};
		};

		for (u32 theta_i = 0; theta_i < theta_segments; ++theta_i) {
			const f32 theta[2] = { theta_i * theta_step,
														 theta_i * theta_step + theta_step };

			for (u32 rho_i = 0; rho_i < theta_segments; ++rho_i) {
				const f32 rho[2] = { rho_i * rho_step,
														 rho_i * rho_step + rho_step };
				
				Vec3 surf[4];
				for (u32 i = 0; i < 2; ++i)
					for (u32 j = 0; j < 2; ++j) {
						surf[i+j*2] = sphericalToCartesian(theta[i], rho[j]) * v3(radius);
					}

				mesh.verts[builder_idx++] = { surf[0] };
				mesh.verts[builder_idx++] = { surf[1] };
				mesh.verts[builder_idx++] = { surf[3] };

				mesh.verts[builder_idx++] = { surf[0] };
				mesh.verts[builder_idx++] = { surf[3] };
				mesh.verts[builder_idx++] = { surf[2] };
			}
		}

		assert(builder_idx == mesh.num_verts);

		return mesh;
	}

	Buffer<GL_ARRAY_BUFFER> buildVbo() {
		auto buf = Buffer<GL_ARRAY_BUFFER>::make(verts,
																						 sizeof(MeshVertex) * num_verts);
		return buf;
	}
};




void draw(Buffer<GL_ARRAY_BUFFER> vbo, Shader shader, u32 count, u32 instances) {
	GL(glUseProgram(shader.id));
	vbo.bind();
	GL(glDrawArraysInstanced(GL_TRIANGLES, 0, count, instances));
}

constexpr u32 PARTICLE_COUNT = 1024;

struct SphParticle {
	Vec3 pos;
	float _pad1;
	Vec3 vel;
	float _pad2;
};

struct Camera {
	Vec3 at;
	Vec2 rot;

	Mat4 viewMat() {
		Mat4 m = mul(
							mul(axisAngleMat(v3(0,1,0), rot.x),	
								 axisAngleMat(v3(1,0,0), rot.y)),
					  	transMat(at));

		return inverse(m);
	}
};


Camera camera;

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_Window* window = SDL_CreateWindow("imgd 4099 final",
																				SDL_WINDOWPOS_UNDEFINED,
																				SDL_WINDOWPOS_UNDEFINED,
																				1280, 720,
																				SDL_WINDOW_OPENGL);
	if (!window) {
		printf("Failed to create window!");
		return 1;
	}

	SDL_GLContext glctx = SDL_GL_CreateContext(window);
	if (!glctx) {
		printf("Failed to create OpenGL context!\n");
		return 1;
	}
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		printf("Failed to load OpenGL!\n");
		return 1;
	}


	GLuint vao;
	GL(glCreateVertexArrays(1, &vao));
	GL(glBindVertexArray(vao));


	Shader compute_shader = Shader::make()
					 											 .addStage<GL_COMPUTE_SHADER>("compute.glsl")
					 											 .link();

  Shader render_shader = Shader::make()
  															.addStage<GL_VERTEX_SHADER>("vertex.glsl")
  															.addStage<GL_FRAGMENT_SHADER>("pixel.glsl")
  															.link();


	Mesh m = Mesh::makeSphere(0.1f, 8.0f, 8.0f);


	Buffer<GL_SHADER_STORAGE_BUFFER> state_bufs[2];

	{ // initial state
		SphParticle particles[PARTICLE_COUNT];

		for (u32 i=0; i<ARRAY_SIZE(particles); ++i) {
			auto& p = particles[i];
			p.pos = v3(i,0,0);
			p.vel = v3(rand()%2-1,rand()%2-1,rand()%2-1);
		}

		for (u32 i=0; i<ARRAY_SIZE(state_bufs); ++i)
			state_bufs[i] = Buffer<GL_SHADER_STORAGE_BUFFER>::make(particles, sizeof(particles));
	}
	



	auto vbo = m.buildVbo();
	bool running = true;
	while (running) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				running = false;
			}
		}

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		render_shader.setUniform("_proj", perspMat(0.25, 1920.0/1080.0, .1, 1000.0));
		render_shader.setUniform("_view", camera.viewMat());

		GL(glEnableVertexAttribArray(0));
		GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0));
		draw(vbo, render_shader, vbo.size / sizeof(MeshVertex), PARTICLE_COUNT);

		compute_shader.execute(PARTICLE_COUNT, 1, 1);

		{
			for (u32 i=0; i<ARRAY_SIZE(state_bufs); ++i)
				state_bufs[i].bindSsbo(i);

			// pingpong
			auto c = state_bufs[1];
			state_bufs[1] = state_bufs[0];
			state_bufs[0] = c;
		}



		u32 err = glGetError();
		if (err != 0)
			printf("GL error: %u\n", err);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glctx);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}