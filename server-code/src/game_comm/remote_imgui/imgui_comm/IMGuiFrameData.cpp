#include "IMGuiFrameData.h"
#include "imgui/imgui.h"
#include "IMGuiFrameDataWarp.h"
#include "IMDataToProto.h"




void IMGuiFrameGutter::push(const imgui::FrameData& frameproto)
{
    m_frameproto.push(std::move(frameproto));
    
}

bool IMGuiFrameGutter::pop(FrameDataWarp& framedata, const ImDrawListSharedData* drawListSharedData)
{
    if(m_frameproto.empty())
        return false;
    imgui::FrameData frameproto;
    if(m_frameproto.pop(frameproto))
    {
        imdata2proto::deserialize(framedata, frameproto, drawListSharedData);
    }
    return true;
}

size_t IMGuiFrameGutter::GetFrames() const { return m_frameproto.size(); }


