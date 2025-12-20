#include "graphics/wm/wm.h"
#include "graphics/renderer/opengl.h"

int main()
{
    MentalWindowManagerInfo wmInfo = {
        .aSizes = {800, 600},
        .eRenderer = MENTAL_RENDERER_OPENGL,
    };
    MentalWindowManager* wm = mentalWindowManagerCreate(&wmInfo);
    if (wm) {
        mentalWindowManagerRun(wm);
    }
    mentalWindowManagerDestroy(wm);
    
    return 0;
}