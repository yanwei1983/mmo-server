#ifndef IMGUI_FRAMEDATA_WARP_H
#define IMGUI_FRAMEDATA_WARP_H

#include "imgui/imgui.h"
struct FrameDataWarp
{
    ImDrawData drawData;
    std::vector<ImDrawList*> cmdLists;
    std::vector<ImDrawList> drawLists;
};


#endif /* IMGUI_FRAMEDATA_WARP_H */
