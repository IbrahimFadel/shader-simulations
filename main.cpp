#include <stdio.h>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;
// #include <glm/gtc/matrix_transform.hpp>

#include "quad.h"

void initGLFW()
{
    glewExperimental = true;
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
}

void initGLEW(GLFWwindow *window)
{
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental = true;        // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(1);
    }
}

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

std::string get_file_content(std::string path)
{
    std::string shader_code;
    std::ifstream shader_stream(path, std::ios::in);
    if (shader_stream.is_open())
    {
        std::stringstream sstr;
        sstr << shader_stream.rdbuf();
        shader_code = sstr.str();
        shader_stream.close();
    }
    else
    {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", path);
        getchar();
        exit(1);
    }

    return shader_code;
}

struct Agent
{
    float positionX;
    float positionY;
    float angle;
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
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    int work_grp_cnt[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n",
           work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
           work_grp_size[0], work_grp_size[1], work_grp_size[2]);

    auto cs_content = get_file_content("../compute.glsl");
    GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
    const char *cs_source = cs_content.c_str();
    glShaderSource(cs, 1, &cs_source, NULL);
    glCompileShader(cs);
    int rvalue;
    GLint info_log_length = GL_FALSE;
    glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
    glGetShaderiv(cs, GL_INFO_LOG_LENGTH, &info_log_length);
    char *log;

    if (!rvalue)
    {
        glGetShaderInfoLog(cs, 1000, &info_log_length, log);
        printf("Error: Compiler log:\n%s\n", log);
        return false;
    }
    // check for compilation errors as per normal here

    GLuint cs_program = glCreateProgram();
    glAttachShader(cs_program, cs);
    glLinkProgram(cs_program);

    GLint result = GL_FALSE;
    info_log_length = GL_FALSE;
    glGetProgramiv(cs_program, GL_LINK_STATUS, &result);
    glGetProgramiv(cs_program, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        std::vector<char> ProgramErrorMessage(info_log_length + 1);
        glGetProgramInfoLog(cs_program, info_log_length, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    result = GL_FALSE;
    info_log_length = GL_FALSE;
    glGetProgramiv(cs_program, GL_LINK_STATUS, &result);
    glGetProgramiv(cs_program, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        std::vector<char> ProgramErrorMessage(info_log_length + 1);
        glGetProgramInfoLog(cs_program, info_log_length, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    auto cs_sub_content = get_file_content("../compute_subtract.glsl");
    GLuint cs_sub = glCreateShader(GL_COMPUTE_SHADER);
    const char *cs_sub_source = cs_sub_content.c_str();
    glShaderSource(cs_sub, 1, &cs_sub_source, NULL);
    glCompileShader(cs_sub);
    rvalue;
    info_log_length = GL_FALSE;
    glGetShaderiv(cs_sub, GL_COMPILE_STATUS, &rvalue);
    glGetShaderiv(cs_sub, GL_INFO_LOG_LENGTH, &info_log_length);

    // if (!rvalue)
    // {
    //     glGetShaderInfoLog(cs_sub, 1000, &info_log_length, log);
    //     printf("Error: Compiler log:\n%s\n", log);
    //     return false;
    // }
    // check for compilation errors as per normal here

    GLuint cs_sub_program = glCreateProgram();
    glAttachShader(cs_sub_program, cs_sub);
    glLinkProgram(cs_sub_program);

    result = GL_FALSE;
    info_log_length = GL_FALSE;
    glGetProgramiv(cs_sub_program, GL_LINK_STATUS, &result);
    glGetProgramiv(cs_sub_program, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        std::vector<char> ProgramErrorMessage(info_log_length + 1);
        glGetProgramInfoLog(cs_sub_program, info_log_length, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    result = GL_FALSE;
    info_log_length = GL_FALSE;
    glGetProgramiv(cs_sub_program, GL_LINK_STATUS, &result);
    glGetProgramiv(cs_sub_program, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        std::vector<char> ProgramErrorMessage(info_log_length + 1);
        glGetProgramInfoLog(cs_sub_program, info_log_length, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    // check for linking errors and validate program as per normal here

    auto quad_vs_str = get_file_content("../quad_vs.glsl");
    auto quad_vs_source = quad_vs_str.c_str();
    // printf("%s\n", quad_vs_str);
    // std::cout << quad_vs_source << '\n';
    GLuint quad_vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(quad_vs, 1, &quad_vs_source, NULL);
    glCompileShader(quad_vs);

    auto quad_fs_str = get_file_content("../quad_fs.glsl");
    auto quad_fs_source = quad_fs_str.c_str();
    GLuint quad_fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(quad_fs, 1, &quad_fs_source, NULL);
    glCompileShader(quad_fs);

    GLuint quad_program = glCreateProgram();
    glAttachShader(quad_program, quad_vs);
    glAttachShader(quad_program, quad_fs);
    glLinkProgram(quad_program);

    result = GL_FALSE;
    info_log_length = GL_FALSE;
    glGetProgramiv(quad_program, GL_LINK_STATUS, &result);
    glGetProgramiv(quad_program, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        std::vector<char> ProgramErrorMessage(info_log_length + 1);
        glGetProgramInfoLog(quad_program, info_log_length, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    GLuint quad_vao;
    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    float spawn_radius = tex_w / 4;
    float centerX = tex_w / 2;
    float centerY = tex_h / 2;
    std::random_device seeder;
    std::mt19937 engine(seeder());
    std::uniform_int_distribution<int> x_position_dist(0, tex_w);
    std::uniform_int_distribution<int> y_position_dist(0, tex_h);
    std::uniform_int_distribution<int> angle_dist(0, 360);

    const int NUM_AGENTS = 100000;
    // const int NUM_AGENTS = 10000;
    Agent agents[NUM_AGENTS];
    for (int i = 0; i < NUM_AGENTS; i++)
    {
        float x = x_position_dist(engine);
        float y = y_position_dist(engine);
        float angle = angle_dist(engine);
        // float r = spawn_radius * sqrt(rand() / double(RAND_MAX));
        // float theta = rand() / double(RAND_MAX) * 2 * 3.1416925;
        // float x = centerX + r * cos(theta);
        // float y = centerY + r * sin(theta);

        // float dot = x * centerX + y * centerY; // dot product between [x1, y1] and [x2, y2]
        // float det = x * centerY - y * centerX; // determinant
        // float angle = atan2(det, dot);         // atan2(y, x) or atan2(sin, cos)

        // // float angle = atan(y / x) * 180 / 3.1416925 + 90;
        // angle = (angle * 180) / 3.1416925;

        Agent agent = {
            .positionX = x,
            .positionY = y,
            .angle = angle};

        agents[i] = agent;
    }

    GLuint ssbo = 0;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(agents), &agents, GL_DYNAMIC_COPY);
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // Second Compute Shader

        glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(cs_sub_program);
        glUniform1i(glGetUniformLocation(cs_sub_program, "texture"), 0);
        glUniform2f(glGetUniformLocation(cs_sub_program, "textureSize"), tex_w, tex_h);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index_buffer_binding, ssbo);

        glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);

        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // Render quad
        glUseProgram(quad_program);

        glUniform2f(glGetUniformLocation(quad_program, "window_size"), window_w, window_h);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(
            0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}