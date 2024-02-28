#include "purpl/purpl.h"

#include "common/alloc.h"
#include "common/common.h"

#include "platform/platform.h"
#include "platform/video.h"

#include "gl.h"

INT32 PurplMain(_In_ PCHAR *arguments, _In_ UINT32 argumentCount)
{
    CmnInitialize(arguments, argumentCount);
#ifdef PURPL_DEBUG
    LogAddFile(fopen("demo.log", "ab"), LogLevelDebug);
#else
    LogAddFile(fopen("demo.log", "ab"), LogLevelInfo);
#endif
    VidInitialize(TRUE);

    GlSetDebugCallback();

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0xFF);

    UINT32 vertexBuffer = GlCreateVertex2dBuffer(
        (VERTEX_2D[]){
            {{0.5, 0.5}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5, -0.5}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
            {{-0.5, -0.5}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5, 0.5}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        },
        4);
    UINT32 indexBuffer = GlCreateIndexBuffer((ivec3[]){{0, 1, 2}, {0, 2, 3}}, 2);
    UINT32 vertexArray = GlCreateVertexArray(vertexBuffer, indexBuffer, GL_VERTEX_2D_ATTRIBUTES,
                                             PURPL_ARRAYSIZE(GL_VERTEX_2D_ATTRIBUTES));

    UINT32 shaderProgram = GlLoadShaders("main");

    UINT32 texture = GlLoadTexture("test");
    GlSetUniform(shaderProgram, "textureSampler", (UINT32[]){0}, GL_INT, 1);

    mat4 projectionMatrix = {0};
    UINT32 width = 0;
    UINT32 height = 0;
    VidGetSize(&width, &height);
    glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, projectionMatrix);

    mat4 modelMatrix = {0};
    glm_mat4_identity(modelMatrix);

    UINT32 globalUniformBuffer = GlCreateUniformBuffer(sizeof(mat4));
    GlWriteUniformBuffer(globalUniformBuffer, 0, projectionMatrix, sizeof(mat4));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, globalUniformBuffer);

    UINT32 objectUniformBuffer = GlCreateUniformBuffer(sizeof(mat4));
    GlWriteUniformBuffer(objectUniformBuffer, 0, modelMatrix, sizeof(mat4));
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, objectUniformBuffer);

    while (VidUpdate())
    {
        GlSetViewport();
        GlClear((vec4){1.0f, 1.0f, 1.0f, 1.0f}, 0.0, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);
        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, NULL);
    }

    //glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteBuffers(4, (UINT32[]){vertexBuffer, indexBuffer, globalUniformBuffer, objectUniformBuffer});

    VidShutdown();
    CmnShutdown();

    return 0;
}
