/// @file gl.h
///
/// @brief This file defines some functions for making OpenGL more convenient. UINT32 is used for sizes because it's
/// unlikely a full SIZE_T will ever be needed.
///
/// @copyright (c) Randomcode Developers 2024

#pragma once

#include "purpl/purpl.h"

#include "common/alloc.h"
#include "common/common.h"
#include "common/filesystem.h"

#include "platform/video.h"

#include "util/mesh.h"
#include "util/texture.h"

/// @brief Set up the debug callback
extern VOID GlSetDebugCallback(VOID);

/// @brief Set the viewport to the window size
extern VOID GlSetViewport(VOID);

/// @brief Clear all three buffers
extern VOID GlClear(vec4 color, DOUBLE depth, UINT8 stencil);

/// @brief A 2D vertex
typedef struct VERTEX_2D
{
    vec2 position;
    vec2 textureCoordinates;
    vec4 colour;
} VERTEX_2D, *PVERTEX_2D;

/// @brief Make a vertex buffer
extern UINT32 GlCreateVertex2dBuffer(PVERTEX_2D vertices, UINT32 vertexCount);

/// @brief Make an index buffer
extern UINT32 GlCreateIndexBuffer(ivec3 *indices, UINT32 indexCount);

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

static CONST GL_VERTEX_ATTRIBUTE GL_VERTEX_2D_ATTRIBUTES[] = {
    {0, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX_2D), 0},
    {1, 2, GL_FLOAT, GL_TRUE, sizeof(VERTEX_2D), 2 * sizeof(FLOAT)},
    {2, 4, GL_FLOAT, GL_TRUE, sizeof(VERTEX_2D), 4 * sizeof(FLOAT)},
};

/// @brief Make a vertex array object
extern UINT32 GlCreateVertexArray(UINT32 vertexBuffer, UINT32 indexBuffer, PGL_VERTEX_ATTRIBUTE vertexAttributes,
                                  UINT32 vertexAttributeCount);

/// @brief Load a vertex and fragment shader with the name given
extern UINT32 GlLoadShaders(PCSTR name);

/// @brief Set a uniform, minus overloads
extern VOID GlSetUniform(UINT32 shader, PCSTR name, PVOID value, UINT32 type, UINT32 count);

/// @brief Set a matrix uniform, minus overloads
extern VOID GlSetMatrixUniform(UINT32 shader, PCSTR name, mat4 value);

/// @brief Uniform data
typedef struct GL_UNIFORM_DATA
{
    mat4 projection;
    mat4 model;
} GL_UNIFORM_DATA, *PGL_UNIFORM_DATA;

/// @brief Create a uniform buffer
extern UINT32 GlCreateUniformBuffer(UINT32 size);

/// @brief Set data in a uniform buffer
extern UINT32 GlWriteUniformBuffer(UINT32 uniformBuffer, UINT32 offset, PVOID data, UINT32 size);

/// @brief Load a texture into OpenGL
extern UINT32 GlLoadTexture(PCSTR name);
