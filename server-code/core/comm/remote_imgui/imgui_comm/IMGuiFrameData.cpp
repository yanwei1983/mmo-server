#include "IMGuiFrameData.h"

#include <algorithm>        // for copy, copy_backward, max
#include <utility>          // for move, exchange
#include "IMDataToProto.h"  // for deserialize
#include "ProtobuffUtil.h"  // for LoadFromBinaryFile, SaveToBinaryFile

IMGuiFrameGutter::IMGuiFrameGutter() {}

void IMGuiFrameGutter::push(imgui::FrameData&& frameproto)
{
    m_frameproto.push_back(std::move(frameproto));
}

bool IMGuiFrameGutter::pop(FrameDataWarp& framedata, const ImDrawListSharedData* drawListSharedData)
{
    if(m_frameproto.empty())
        return false;

    imgui::FrameData frameproto(std::move(m_frameproto.front()));
    m_frameproto.pop_front();
    imdata2proto::deserialize(framedata, frameproto, drawListSharedData);
    
    return true;
}

size_t IMGuiFrameGutter::GetFrames() const
{
    return m_frameproto.size();
}


void IMGuiFrameGutter::dump(const std::string& filename)
{
    auto count = m_frameproto.size();
    imgui::FrameDataPack pack;
    pack.mutable_framebuffer()->Reserve(count);
    
    for(int i = 0; i < count; i++)
    {
        auto ptr = pack.add_framebuffer();
        *ptr = m_frameproto.front();
        m_frameproto.pop_front();
    }
    
    pb_util::SaveToBinaryFile(pack, filename);
}


void IMGuiFrameGutter::loadFromFile(const std::string& filename)
{
    imgui::FrameDataPack pack;
    pb_util::LoadFromBinaryFile(filename, pack);
    for(const auto& v : pack.framebuffer())
    {
        m_frameproto.push_back(v);
    }
}

namespace
{
void XORDiff(std::string& dst, const std::string& src)
{
    dst.resize(src.size());
    for(int i = 0; i < dst.size();i++)
    {
        dst[i] ^= src[i];
    }
}

void XORDiff(std::string& dst, const std::string_view& src)
{
    dst.resize(src.size());
    for(int i = 0; i < dst.size();i++)
    {
        dst[i] ^= src[i];
    }
}

}

std::string IMGuiFrameGutter::diff_last_string(imgui::FrameData&& frameproto)
{
    std::string output = frameproto.SerializeAsString();

    XORDiff(m_last_frame_data, output);
    return std::exchange(m_last_frame_data, output);
}

imgui::FrameData IMGuiFrameGutter::diff_last_string(const std::string_view& xor_data)
{
    XORDiff(m_last_frame_data, xor_data);
    imgui::FrameData result;
    result.ParseFromString(m_last_frame_data);
    return result;
}
