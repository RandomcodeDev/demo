/// @file gl.h
///
/// @brief This file defines some functions for making OpenGL more convenient
///
/// @copyright (c) Randomcode Developers 2024

#pragma once

#include "purpl/purpl.h"

#include "common/alloc.h"
#include "common/common.h"

#include "util/mesh.h"
#include "util/texture.h"

/// @brief Clear all three buffers
extern VOID GlClear(vec4 color, DOUBLE depth, UINT8 stencil);

/// @brief Make a vertex buffer
extern INT32 GlMakeVertexBuffer(PVERTEX vertices, UINT32 vertexCount);

/// @brief Make an index buffer
extern INT32 GlMakeIndexBuffer(ivec3 *indices, UINT32 indexCount);

/// @brief A vertex attribute
typedef struct GL_VERTEX_ATTRIBUTE
{
    UINT32 index;
    UINT32 size;
    GLenum type;
    GLboolean normalize;
    UINT32 stride;
    UINT32 offset;
} GL_VERTEX_ATTRIBUTE, *PGL_VERTEX_ATTRIBUTE;

static CONST GL_VERTEX_ATTRIBUTE GL_MESH_VERTEX_ATTRIBUTES[] = {
    {0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(FLOAT), 0},
    {1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(FLOAT), 3 * sizeof(FLOAT)},
    {2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(FLOAT), 7 * sizeof(FLOAT)},
    {3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(FLOAT), 9 * sizeof(FLOAT)}
};

/// @brief Make a vertex array object
extern INT32 GlMakeVertexArrayObject(INT32 VertexBuffer, INT32 IndexBuffer, PGL_VERTEX_ATTRIBUTE vertexAttributes,
                                     SIZE_T vertexAttributeCount);