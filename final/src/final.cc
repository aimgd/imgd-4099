#include <stdint.h>

#include <SDL2/SDL.h>
#include "ext/glad/glad.h"

#include "ext/glad.c"
#include <assert.h>

#include "maths.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ext/imgui/imgui.h"

#include "ext/imgui/imgui.cpp"
#include "ext/imgui/imgui_draw.cpp"
#include "ext/imgui/imgui_tables.cpp"
#include "ext/imgui/imgui_widgets.cpp"

#include "ext/imgui-backends/imgui_impl_sdl2.h"


#include "ext/imgui/backends/imgui_impl_opengl3.h"


#define GL(x) do { { x; } assert(glGetError() == 0); } while(0);
#define WORKGROUP_SIZE 8

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

	void read(void* data) {
		glGetNamedBufferSubData(id, 0, size, data);
	}

	void write(void* data) {
		glNamedBufferSubData(id, 0, size, data);
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

constexpr u32 PARTICLE_COUNT = 5000;

struct SphParticle {
	Vec3 pos;
	float density;
	Vec3 vel;
	float _pad2;
};

struct Camera {
	Vec3 at;
	Vec2 rot;

	Mat4 viewMat() {
		Mat4 m = mul(
							transMat(at),
					  	mul(axisAngleMat(v3(0,1,0), rot.x),	
								 axisAngleMat(v3(1,0,0), rot.y)));

		return inverse(m);
	}
};


Camera camera;
const float CAMERA_SPEED = 0.2;
char key_state[512];

struct {
	f32 _sph_mass = 0.5;
	f32 _sph_radius = 1.2;
	f32 _target_density = 4.5;
	f32 _pressure_mul = 1000.0f;  // was 500.0f; // was 250.0;
} config;

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

	Shader compute_shader = Shader::make()
					 											 .addStage<GL_COMPUTE_SHADER>("compute.glsl")
					 											 .link();

	Shader compute_shader2 = Shader::make()
					 											 .addStage<GL_COMPUTE_SHADER>("compute-density.glsl")
					 											 .link();


  Shader render_shader = Shader::make()
  															.addStage<GL_VERTEX_SHADER>("vertex.glsl")
  															.addStage<GL_FRAGMENT_SHADER>("pixel.glsl")
  															.link();

  Shader floor_shader  = Shader::make()
  															.addStage<GL_VERTEX_SHADER>("floor-vs.glsl")
  															.addStage<GL_FRAGMENT_SHADER>("floor-ps.glsl")
  															.link();

	Mesh m = Mesh::makeSphere(0.3f, 16.0f, 16.0f);


	Vec3 box_size = v3(10, 10, 10);

	Buffer<GL_SHADER_STORAGE_BUFFER> state_bufs[2];

	SphParticle particles[PARTICLE_COUNT];
	{ // initial state

		auto rand01 =[]() {
			return (rand()/float(RAND_MAX));
		};

		for (u32 idx=0; idx<PARTICLE_COUNT; ++idx) {
			particles[idx] = {
				.pos = v3(rand01(),rand01(),rand01()) * v3(4,4,4), // + v3(rand()%10-5,rand()%10-5,rand()%10-5)/v3(25.0),
				.vel = v3(0,0,0),
			};
		}
		for (u32 i=0; i<ARRAY_SIZE(state_bufs); ++i)
			state_bufs[i] = Buffer<GL_SHADER_STORAGE_BUFFER>::make(particles, sizeof(particles));
	}

	glEnable(GL_DEPTH_TEST);


	// Initialize IMGUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	// Initialize IMGUI backends
	ImGui_ImplSDL2_InitForOpenGL(window, glctx);
	ImGui_ImplOpenGL3_Init();

	auto vbo = m.buildVbo();
	bool running = true;
	while (running) {

		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					if (ev.key.keysym.sym < ARRAY_SIZE(key_state))
						key_state[ev.key.keysym.sym] = ev.type == SDL_KEYDOWN;
				} break;
				case SDL_MOUSEMOTION: {
					if (SDL_GetRelativeMouseMode())
						camera.rot += v2(ev.motion.xrel, ev.motion.yrel) * v2(0.001f); 
				} break;
			}

			if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == 'h') {
				SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
			}

      ImGui_ImplSDL2_ProcessEvent(&ev);
		}


		// Render
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Make the entire window a dock space
		// ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());



		if (ImGui::Begin("Config!")) {
			ImGui::SliderFloat("_sph_mass", &config._sph_mass, 0.1f, 5.0f);
			ImGui::SliderFloat("_sph_radius", &config._sph_radius, 0.1f, 2.0f);
			ImGui::SliderFloat("_target_density", &config._target_density, 0.1f, 5.0f);
			ImGui::SliderFloat("_pressure_mul", &config._pressure_mul, 0.1f, 1000.f);

			ImGui::SliderFloat("_box_size_x", &box_size.x, 1.0f, 40.f);
			ImGui::SliderFloat("_box_size_y", &box_size.y, 1.0f, 40.f);
			ImGui::SliderFloat("_box_size_z", &box_size.z, 1.0f, 40.f);
		}
		ImGui::End();

		// input
		camera.at = camera.at 
							+ forward(inverse(camera.viewMat())) * v3((key_state['w'] - key_state['s']) * CAMERA_SPEED)
							+ right(inverse(camera.viewMat()))   * v3((key_state['d'] - key_state['a']) * CAMERA_SPEED)
							+ up(inverse(camera.viewMat()))      * v3((key_state['e'] - key_state['q']) * CAMERA_SPEED);



		auto swapBuf = [&]() {
			for (u32 i=0; i<ARRAY_SIZE(state_bufs); ++i)
				state_bufs[i].bindSsbo(i);

			// pingpong
			auto c = state_bufs[1];
			state_bufs[1] = state_bufs[0];
			state_bufs[0] = c;
		};


		if (key_state['t']) {
			state_bufs[0].read(particles);
			for (int i = 0; i < PARTICLE_COUNT; i++) {
				particles[i].vel += (particles[i].pos - camera.at) / v3(dot((particles[i].pos - camera.at),(particles[i].pos - camera.at))) * v3(3.0);
			}
			state_bufs[0].write(particles);				
		}

		swapBuf();

		if (key_state['x']) box_size.x += 0.1f;
		if (key_state['u']) box_size.x -= 0.1f;


		if (key_state['z']) box_size.z += 0.1f;
		if (key_state['v']) box_size.z -= 0.1f;


		compute_shader2.setUniform("_sph_mass", config._sph_mass);
		compute_shader2.setUniform("_sph_radius", config._sph_radius);
		compute_shader2.setUniform("_target_density", config._target_density);
		compute_shader2.setUniform("_pressure_mul", config._pressure_mul);
		compute_shader2.setUniform("_particle_count", PARTICLE_COUNT);


		compute_shader2.execute(PARTICLE_COUNT, 1, 1);


		swapBuf();


		compute_shader.setUniform("_sph_mass", config._sph_mass);
		compute_shader.setUniform("_sph_radius", config._sph_radius);
		compute_shader.setUniform("_target_density", config._target_density);
		compute_shader.setUniform("_pressure_mul", config._pressure_mul);

		compute_shader.setUniform("_bbox_size", box_size);
		compute_shader.setUniform("_particle_count", PARTICLE_COUNT);
		// compute_shader.execute(PARTICLE_COUNT / (WORKGROUP_SIZE * WORKGROUP_SIZE) + (PARTICLE_COUNT % (WORKGROUP_SIZE * WORKGROUP_SIZE) > 0) , 1, 1);
	
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		compute_shader.execute(PARTICLE_COUNT, 1, 1);


		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_shader.setUniform("_sph_mass", config._sph_mass);
		render_shader.setUniform("_sph_radius", config._sph_radius);
		render_shader.setUniform("_target_density", config._target_density);
		render_shader.setUniform("_pressure_mul", config._pressure_mul);

		render_shader.setUniform("_proj", perspMat(0.25, 1920.0/1080.0, .1, 1000.0));
		render_shader.setUniform("_view", camera.viewMat());


		GL(glBindVertexArray(vao));

		GL(glEnableVertexAttribArray(0));
		GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0));
		draw(vbo, render_shader, vbo.size / sizeof(MeshVertex), PARTICLE_COUNT);

		floor_shader.setUniform("_proj", perspMat(0.25, 1920.0/1080.0, .1, 1000.0));
		floor_shader.setUniform("_view", camera.viewMat());
		floor_shader.setUniform("_bbox_size", box_size);


		GL(glEnableVertexAttribArray(0));
		GL(glUseProgram(floor_shader.id));
		GL(glDrawArrays(GL_TRIANGLES, 0, 6));

		u32 err = glGetError();
		if (err != 0)
			printf("GL error: %u\n", err);



		// Graphics
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);
	}


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	ImGui::DestroyContext();

	SDL_GL_DeleteContext(glctx);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}