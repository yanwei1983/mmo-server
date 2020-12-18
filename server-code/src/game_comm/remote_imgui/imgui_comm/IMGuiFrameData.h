#ifndef IMGUI_FRAME_DATA_H
#define IMGUI_FRAME_DATA_H


#include "imgui.pb.h"
#include "LockfreeQueue.h"
struct ImDrawData;
struct ImDrawListSharedData;
struct ImDrawList;

struct FrameDataWarp;

class IMGuiFrameGutter
{
public:
    IMGuiFrameGutter();

    void push(const imgui::FrameData& frameproto);
    bool pop(FrameDataWarp& framedata, const ImDrawListSharedData* drawListSharedData);
    size_t GetFrames() const;

    
private:
    MPSCQueue<imgui::FrameData> m_frameproto;
};

#endif /* IMGUI_FRAME_DATA_H */
