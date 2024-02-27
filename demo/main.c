#include "purpl/purpl.h"

#include "common/alloc.h"
#include "common/common.h"

#include "platform/platform.h"
#include "platform/video.h"

#include "gl.h"

INT32 PurplMain(_In_ PCHAR *arguments, _In_ UINT32 argumentCount)
{
    CmnInitialize(arguments, argumentCount);
    VidInitialize(TRUE);

    INT32 vertexBuffer = GlMakeVertexBuffer(
        (VERTEX[]){{{-0.5, -0.5, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {-1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
                   {{-0.5, 0.5, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {-1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                   {{0.5, -0.5, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
                   {{0.5, 0.5, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}},
        4);
    INT32 indexBuffer = GlMakeIndexBuffer((ivec3[]){{0, 1, 2}, {1, 2, 3}}, 2);
    INT32 vertexArrayObject = GlMakeVertexArrayObject(vertexBuffer, indexBuffer, GL_MESH_VERTEX_ATTRIBUTES,
                                                      PURPL_ARRAYSIZE(GL_MESH_VERTEX_ATTRIBUTES));

    while (VidUpdate())
    {
        GlClear((vec4){0.6f, 0.0f, 1.0f, 0.0f}, 0.0f, 0);
    }

    VidShutdown();
    CmnShutdown();

    return 0;
}
