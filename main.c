#include "purpl/purpl.h"

#include "common/alloc.h"
#include "common/common.h"

#include "platform/platform.h"
#include "platform/video.h"

INT32 PurplMain(_In_ PCHAR* arguments, _In_ UINT32 argumentCount)
{
    CmnInitialize(arguments, argumentCount);
    VidInitialize(TRUE);

    while (VidUpdate())
    {
        glClearColor(0.6, 0.0, 1.0, 1.0);
        glClearDepth(0.0);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    VidShutdown();
    CmnShutdown();

    return 0;
}

