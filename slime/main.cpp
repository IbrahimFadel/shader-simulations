#include <glBoilerplate.h>
#include <shader.h>
#include <texture.h>
#include <quad.h>
#include <glm/glm.hpp>
using namespace glm;

#include <random>

#define PI 3.14159265

struct Agent
{
	float positionX;
	float positionY;
	float angle;
};

enum SpawningMode
{
	_random,
	inward_circle
};

int main()
{
	initGLFW();

	float window_w = 1024, window_h = 1024;

	GLFWwindow *window = glfwCreateWindow(window_w, window_h, "Compute Shader Test", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		exit(1);
	}
	initGLEW(window);

	int tex_w = 1024, tex_h = 1024;
	GLuint tex = createTexture(tex_w, tex_h);

	GLuint cs_program = createComputeShader("../slime/shaders/compute.glsl");
	GLuint cs_sub_program = createComputeShader("../slime/shaders/compute_subtract.glsl");
	GLuint quad_program = createVertexFragmentShaders("../slime/shaders/quad_vs.glsl", "../slime/shaders/quad_fs.glsl");

	GLuint quad_vao;
	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	float centerX = tex_w / 2;
	float centerY = tex_h / 2;

	std::random_device seeder;
	std::mt19937 engine(seeder());
	std::uniform_real_distribution<float> width_distr(0, tex_w);
	std::uniform_real_distribution<float> height_distr(0, tex_h);
	std::uniform_real_distribution<float> zero_one_distr(0, 1);

	SpawningMode spawning_mode = SpawningMode::_random;
	vec2 center = {tex_w / 2, tex_h / 2};
	vec2 start_pos = {0, 0};
	float angle = 0;
	float radius = 500;
	const int NUM_AGENTS = 50000;
	Agent agents[NUM_AGENTS];
	for (int i = 0; i < NUM_AGENTS; i++)
	{
		if (spawning_mode == SpawningMode::inward_circle)
		{
			float theta = 2 * PI * ((float)rand() / RAND_MAX);
			double r = sqrt(((float)rand() / RAND_MAX));
			vec2 randomInsideCircle = {r * radius * cos(theta), r * radius * sin(theta)};
			start_pos = center + randomInsideCircle;
			angle = atan2(normalize(center - start_pos).y, normalize(center - start_pos).x);
		}
		else if (spawning_mode == SpawningMode::_random)
		{
			start_pos = {width_distr(engine), height_distr(engine)};
			angle = PI * 2 * zero_one_distr(engine);
		}

		Agent agent = {
				.positionX = start_pos.x,
				.positionY = start_pos.y,
				.angle = angle};

		agents[i] = agent;
	}

	GLuint ssbo = 0;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(agents), &agents, GL_DYNAMIC_COPY);

	GLuint index_buffer_binding = 0;

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{
		// First Compute Shader

		glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glUseProgram(cs_program);
		glUniform1i(glGetUniformLocation(cs_program, "texture"), 0);
		glUniform2f(glGetUniformLocation(cs_program, "textureSize"), tex_w, tex_h);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index_buffer_binding, ssbo);

		glDispatchCompute((GLuint)sizeof(agents) / sizeof(agents[0]), 1, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Second Compute Shader

		glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glUseProgram(cs_sub_program);
		glUniform1i(glGetUniformLocation(cs_sub_program, "texture"), 0);
		glUniform2f(glGetUniformLocation(cs_sub_program, "textureSize"), tex_w, tex_h);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index_buffer_binding, ssbo);

		glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Render quad
		glUseProgram(quad_program);

		glUniform2f(glGetUniformLocation(quad_program, "window_size"), window_w, window_h);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(
				0,				// attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,				// size
				GL_FLOAT, // type
				GL_FALSE, // normalized?
				0,				// stride
				(void *)0 // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}