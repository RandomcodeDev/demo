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

    UINT32 vertexBuffer = GlCreateVertex2dBuffer(
        (VERTEX_2D[]){
            {{0.5, 0.5}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{0.5, -0.5}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
            {{-0.5, -0.5}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5, 0.5}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        },
        4);
    UINT32 indexBuffer = GlCreateIndexBuffer((ivec3[]){{0, 1, 2}, {1, 2, 3}}, 2);
    UINT32 vertexArray = GlCreateVertexArray(vertexBuffer, indexBuffer, GL_VERTEX_2D_ATTRIBUTES,
                                             PURPL_ARRAYSIZE(GL_VERTEX_2D_ATTRIBUTES));

    UINT32 shaderProgram = GlLoadShaders("main");

    UINT32 texture = GlLoadTexture("test");
    GlSetUniform(shaderProgram, "textureSampler", (UINT32[]){0}, GL_INT, 1);

    GL_UNIFORM_DATA uniform = {0};
    UINT32 Width = 0;
    UINT32 Height = 0;
    VidGetSize(&Width, &Height);
    glm_ortho(0.0f, Width, Height, 0.0f, -1.0f, 1.0f, uniform.projection);
    glm_mat4_identity(uniform.model);

    UINT32 uniformBuffer = GlCreateUniformBuffer(sizeof(GL_UNIFORM_DATA));
    GlWriteUniformBuffer(uniformBuffer, 0, &uniform, sizeof(GL_UNIFORM_DATA));

    while (VidUpdate())
    {
        GlClear((vec4){0.0f, 0.0f, 0.0f, 0.0f}, 0.0f, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBuffer, offsetof(GL_UNIFORM_DATA, projection), sizeof(mat4));
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, uniformBuffer, offsetof(GL_UNIFORM_DATA, model), sizeof(mat4));
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);
        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    }

    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteBuffers(3, (UINT32[]){vertexBuffer, indexBuffer, uniformBuffer});

    VidShutdown();
    CmnShutdown();

    return 0;
}
