#include "gl.h"

static VOID GlDebugOutput(INT32 source, INT32 type, UINT32 id, INT32 severity, INT32 length, PCSTR message,
                          CONST PVOID userParam)
{
#define X(out, type, name)                                                                                             \
    case GL_DEBUG_##type##_##name:                                                                                     \
        (out) = #name;                                                                                                 \
        break;

    PCSTR sourceText;
    switch (source)
    {
        X(sourceText, SOURCE, API);
        X(sourceText, SOURCE, WINDOW_SYSTEM);
        X(sourceText, SOURCE, SHADER_COMPILER);
        X(sourceText, SOURCE, THIRD_PARTY);
        X(sourceText, SOURCE, APPLICATION);
    default:
        X(sourceText, SOURCE, OTHER);
    }

    PCSTR typeText;
    switch (source)
    {
        X(typeText, TYPE, ERROR);
        X(typeText, TYPE, DEPRECATED_BEHAVIOR);
        X(typeText, TYPE, UNDEFINED_BEHAVIOR);
        X(typeText, TYPE, PORTABILITY);
        X(typeText, TYPE, PERFORMANCE);
        X(typeText, TYPE, MARKER);
        X(typeText, TYPE, PUSH_GROUP);
        X(typeText, TYPE, POP_GROUP);
    default:
        X(typeText, TYPE, OTHER);
    }

    LOG_LEVEL severityLevel;
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        severityLevel = LogLevelError;
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severityLevel = LogLevelWarning;
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severityLevel = LogLevelInfo;
        break;
    default:
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severityLevel = LogLevelDebug;
        break;
    }

    LogMessage(severityLevel, "OpenGL", (UINT64)PlatGetReturnAddress(), TRUE, "%s message from %s: %s", typeText, sourceText,
               message);

#undef X
}

VOID GlSetDebugCallback(VOID)
{
#ifdef PURPL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback(GlDebugOutput, NULL);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
}

VOID GlClear(vec4 color, DOUBLE depth, UINT8 stencil)
{
    glClearColor(color[0], color[1], color[2], color[3]);
    glClearDepth(depth);
    glClearStencil(stencil);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

UINT32 GlCreateVertex2dBuffer(PVERTEX_2D vertices, UINT32 vertexCount)
{
    UINT32 buffer = GL_INVALID_VALUE;
    glGenBuffers(1, &buffer);
    if (buffer == GL_INVALID_VALUE)
    {
        LogError("Failed to create buffer: %d", glGetError());
        return GL_INVALID_VALUE;
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(VERTEX_2D), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glObjectLabel(GL_BUFFER, buffer, 13, "Vertex buffer");

    return buffer;
}

UINT32 GlCreateIndexBuffer(ivec3 *indices, UINT32 indexCount)
{
    UINT32 buffer = GL_INVALID_VALUE;
    glGenBuffers(1, &buffer);
    if (buffer == GL_INVALID_VALUE)
    {
        LogError("Failed to create buffer: %d", glGetError());
        return GL_INVALID_VALUE;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(ivec3), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glObjectLabel(GL_BUFFER, buffer, 12, "Index buffer");

    return buffer;
}

UINT32 GlCreateVertexArray(UINT32 vertexBuffer, UINT32 indexBuffer, PGL_VERTEX_ATTRIBUTE vertexAttributes,
                           UINT32 vertexAttributeCount)
{
    UINT32 vertexArray = GL_INVALID_VALUE;
    glGenVertexArrays(1, &vertexArray);
    if (vertexArray == GL_INVALID_VALUE)
    {
        LogError("Failed to create vertex array object: %d", glGetError());
        return GL_INVALID_VALUE;
    }

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    for (SIZE_T i = 0; i < vertexAttributeCount; i++)
    {
        glVertexAttribPointer(vertexAttributes[i].index, vertexAttributes[i].size, vertexAttributes[i].type,
                              vertexAttributes[i].normalize, vertexAttributes[i].stride,
                              (PVOID)(UINT_PTR)vertexAttributes[i].offset);
        glEnableVertexAttribArray(vertexAttributes[i].index);
    }

    glBindVertexArray(0);

    return vertexArray;
}

static UINT32 LoadShader(UINT32 type, PCSTR entryPoint, PCSTR pathFormat, ...)
{
    CHAR buffer[512] = {0};
    INT32 success = 0;
    UINT32 shader = GL_INVALID_VALUE;

    va_list arguments;
    va_start(arguments, pathFormat);
    PCHAR path = CmnFormatStringVarArgs(pathFormat, arguments);
    va_end(arguments);

    LogInfo("Loading shader binary %s with entry point %s", path, entryPoint);

    UINT64 binarySize = 0;
    PBYTE binary = FsReadFile(path, 0, 0, &binarySize, 0);
    if (!binary || !binarySize)
    {
        LogError("Failed to load shader %s", path);
        goto Done;
    }

    shader = glCreateShader(type);
    glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, binary, (INT32)binarySize);
    glSpecializeShader(shader, entryPoint, 0, NULL, NULL);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, PURPL_ARRAYSIZE(buffer), NULL, buffer);
        glDeleteShader(shader);
        LogError("Failed to load shader binary %s: %s", path, buffer);
        goto Done;
    }

Done:
    if (binary)
    {
        CmnFree(binary);
    }
    if (path)
    {
        CmnFree(path);
    }
    return shader;
}

UINT32 GlLoadShaders(PCSTR name)
{
    CHAR buffer[512] = {0};
    INT32 success;

    if (!name)
    {
        return FALSE;
    }

    LogInfo("Loading shader %s", name);

    UINT32 vertexShader = LoadShader(GL_VERTEX_SHADER, "VertexMain", "assets/shaders/vulkan/%s.vs.spv", name);
    UINT32 fragmentShader = LoadShader(GL_FRAGMENT_SHADER, "PixelMain", "assets/shaders/vulkan/%s.ps.spv", name);

    UINT32 program = glCreateProgram();
    if (program == GL_INVALID_VALUE)
    {
        LogError("Failed to create shader program: %d", glGetError());
        return GL_INVALID_VALUE;
    }

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, PURPL_ARRAYSIZE(buffer), NULL, buffer);
        LogError("Failed to link shader program %s: %s", name, buffer);
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    glObjectLabel(GL_PROGRAM, program, (INT32)strlen(name), name);

    return program;
}

VOID GlSetUniform(UINT32 shader, PCSTR name, PVOID value, UINT32 type, UINT32 count)
{
#define XX1(location, type, suffix, value)                                                                             \
    case GL_##type:                                                                                                    \
        glUniform1##suffix(location, (value)[0]);                                                                      \
        break;
#define XX2(location, type, suffix, values)                                                                            \
    case GL_##type:                                                                                                    \
        glUniform2##suffix(location, (values)[0], (values)[1]);                                                        \
        break;
#define XX3(location, type, suffix, values)                                                                            \
    case GL_##type:                                                                                                    \
        glUniform3##suffix(location, (values)[0], (values)[1], (values)[2]);                                           \
        break;
#define XX4(location, type, suffix, values)                                                                            \
    case GL_##type:                                                                                                    \
        glUniform4##suffix(location, (values)[0], (values)[1], (values)[2], (values)[3]);                              \
        break;
#define X(location, type, count, values)                                                                               \
    case (count):                                                                                                      \
        switch ((type))                                                                                                \
        {                                                                                                              \
            XX##count(location, INT, i, (PINT32)values);                                                               \
            XX##count(location, FLOAT, f, (PFLOAT)values);                                                             \
        }                                                                                                              \
        break;

    glUseProgram(shader);

    INT32 uniformLocation = glGetUniformLocation(shader, name);
    if (uniformLocation < 0 || uniformLocation == GL_INVALID_VALUE)
    {
        LogError("Failed to set uniform %s", name);
        return;
    }

    switch (count)
    {
    default: // default is 1
        X(uniformLocation, type, 1, value);
        X(uniformLocation, type, 2, value);
        X(uniformLocation, type, 3, value);
        X(uniformLocation, type, 4, value);
    }

    glUseProgram(0);

#undef X
#undef XX1
#undef XX2
#undef XX3
#undef XX4
}

VOID GlSetMatrixUniform(UINT32 shader, PCSTR name, mat4 value)
{
    glUseProgram(shader);

    INT32 uniformLocation = glGetUniformLocation(shader, name);
    if (uniformLocation < 0 || uniformLocation == GL_INVALID_VALUE)
    {
        LogError("Failed to set uniform %s", name);
        return;
    }

    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (PFLOAT)value);

    glUseProgram(0);
}

UINT32 GlCreateUniformBuffer(UINT32 size)
{
    UINT32 buffer = GL_INVALID_VALUE;
    glGenBuffers(1, &buffer);
    if (buffer == GL_INVALID_VALUE)
    {
        LogError("Failed to create uniform buffer: %d", glGetError());
    }

    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glObjectLabel(GL_BUFFER, buffer, 14, "Uniform buffer");

    return buffer;
}

UINT32 GlWriteUniformBuffer(UINT32 uniformBuffer, UINT32 offset, PVOID data, UINT32 size)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

UINT32 GlLoadTexture(PCSTR name)
{
    UINT32 texture = GL_INVALID_VALUE;
    glGenTextures(1, &texture);
    if (texture == GL_INVALID_VALUE)
    {
        LogError("Failed to create texture: %d", glGetError());
        return GL_INVALID_VALUE;
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    PTEXTURE sourceTexture = LoadTexture(CmnFormatTempString("assets/textures/%s.ptex", name));
    if (!sourceTexture || sourceTexture->Format != TextureFormatRgba8)
    {
        return GL_INVALID_VALUE;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sourceTexture->Width, sourceTexture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 sourceTexture->Pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glObjectLabel(GL_TEXTURE, texture, (INT32)strlen(name), name);

    // can i just appreciate for a minute how convenient it is that i can call free on this instead of needing a
    // special function? people should design functions that heap allocate structures to make it all one block.
    CmnFree(sourceTexture);

    return texture;
}
