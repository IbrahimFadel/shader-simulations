#include <glBoilerplate.h>
#include <shader.h>
#include <texture.h>
#include <quad.h>
#include <string.h>

#include "Fluid.h"
#include "mouse.h"

void updateFluidChunkBuffer(float *buf, Fluid *fluid)
{
    // memcpy(buf, fluid->density, sizeof(fluid->density));
    // std::copy(std::begin(fluid->density), std::end(fluid->density), std::begin(buf));
}

int main()
{
    initGLFW();

    float windowWidth = 1024, windowHeight = 1024;

    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Fluid Simulation", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        exit(1);
    }
    initGLEW(window);

    GLuint cs = createComputeShader("../fluid/shaders/fluidCompute.glsl");
    GLuint renderShader = createVertexFragmentShaders("../fluid/shaders/vs.glsl", "../fluid/shaders/fs.glsl");

    int textureWidth = 64, textureHeight = 64;
    GLuint texture = createTexture(textureWidth, textureHeight);

    GLuint quad_vao;
    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // float *densities = new float[textureWidth * textureHeight]();
    // for (int i = 0; i < textureWidth * textureHeight; i++)
    // {
    //     FluidChunk chunk = {
    //         // .density = (float)(rand() / double(RAND_MAX)),
    //         .density = 0,
    //         .directionX = 0,
    //         .directionY = 0};
    //     chunks[i] = chunk;
    // }

    // chunks[10] = {
    //     .density = 1,
    //     .directionX = 0.2,
    //     .directionY = 0};

    // chunks[11] = {
    //     .density = 1,
    //     .directionX = 0.2,
    //     .directionY = 0};

    Fluid fluid(textureWidth, 1, 1, 0.015);
    // // fluid->density[470] = 1;

    GLuint ssbo = 0;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(fluid), &fluid, GL_DYNAMIC_COPY);

    // GLuint densitiesBufferIndex = 0;

    // float currentFrame = glfwGetTime();
    // float lastFrame = currentFrame;
    // float dt;

    // double mouseX;
    // double mouseY;

    // int textureX;
    // int textureY;

    // bool mouseDown = false;

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        // currentFrame = glfwGetTime();
        // dt = currentFrame - lastFrame;
        // lastFrame = currentFrame;

        // int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        // if (mouseState == GLFW_PRESS)
        // {
        //     glfwGetCursorPos(window, &mouseX, &mouseY);
        //     mouseY = windowHeight - mouseY;

        //     int textureX = (mouseX / windowWidth) * textureWidth;
        //     int textureY = (mouseY / windowHeight) * textureHeight;

        //     printf("%d/%d\n", textureX, textureY);

        //     // fluid->addDensity(textureX, textureY, 1);
        //     mouseDown = true;
        // }
        // else
        // {
        //     mouseDown = false;
        // }

        // for (int i = 0; i < textureWidth * textureWidth; i++)
        // {
        //     if (fluid->density[i] > 0)
        //     {
        //         printf("Density at %d: %f\n", i, fluid->density[i]);
        //     }
        // }

        // // fluid->step();
        // // updateFluidChunkBuffer(densities, fluid);
        // // densities = fluid->density;

        glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUseProgram(cs);
        glUniform1i(glGetUniformLocation(cs, "texture"), 0);
        glUniform2f(glGetUniformLocation(cs, "textureSize"), textureWidth, textureHeight);
        // glUniform1i(glGetUniformLocation(cs, "mouseDown"), mouseDown);
        // glUniform2f(glGetUniformLocation(cs, "mousePos"), textureX, textureY);
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, densitiesBufferIndex, ssbo);

        glDispatchCompute((GLuint)textureWidth, (GLuint)textureHeight, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // // Render texture

        glUseProgram(renderShader);

        glUniform2f(glGetUniformLocation(renderShader, "windowSize"), windowWidth, windowHeight);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}