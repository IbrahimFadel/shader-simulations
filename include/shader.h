#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>

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

GLuint createComputeShader(const char *path)
{
    auto csContent = get_file_content(path);
    const char *csSource = csContent.c_str();
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &csSource, NULL);

    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        printf("%s\n", &errorLog[0]);

        glDeleteShader(shader);
        return -1;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    return program;
}

GLuint createVertexFragmentShaders(const char *vtx_path, const char *frg_path)
{
    auto vsContent = get_file_content(vtx_path);
    const char *vsSource = vsContent.c_str();
    auto fsContent = get_file_content(frg_path);
    const char *fsSource = fsContent.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);

    GLint isCompiled = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);

        printf("%s\n", &errorLog[0]);

        glDeleteShader(vs);
        return -1;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSource, NULL);
    glCompileShader(fs);

    isCompiled = 0;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fs, maxLength, &maxLength, &errorLog[0]);

        printf("%s\n", &errorLog[0]);

        glDeleteShader(fs);
        return -1;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    return program;
}

#endif