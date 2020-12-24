#ifndef IMDATA_TO_PROTO_H
#define IMDATA_TO_PROTO_H

struct ImVec2;
struct ImVec4;
struct ImDrawCmd;
struct ImDrawVert;
struct ImDrawData;
struct ImDrawListSharedData;

namespace imgui
{
    class Vec2;
    class Vec4;
    class DrawCmd;
    class DrawVert;
    class FrameData;
}

struct FrameDataWarp;

namespace imdata2proto
{
    void serialize(const ImVec4&  vec, imgui::Vec4& proto);
    void deserialize(ImVec4&  vec, const imgui::Vec4& proto);
    
    void serialize(const ImVec2&  vec, imgui::Vec2& proto);
    void deserialize(ImVec2&  vec, const imgui::Vec2& proto);
    

    void serialize(const ImDrawCmd&  drawCmd, imgui::DrawCmd& proto);
    void deserialize(ImDrawCmd&  drawCmd, const imgui::DrawCmd& proto);

    void serialize(const ImDrawVert&  drawvert, imgui::DrawVert& proto);
    void deserialize(ImDrawVert&  drawvert, const imgui::DrawVert& proto);

    void serialize(const ImDrawData* drawData, imgui::FrameData& frame);
    void deserialize(FrameDataWarp& drawDataWarp, const imgui::FrameData& frame, const ImDrawListSharedData* drawListSharedData);    

} // namespace

#endif /* IMDATA_TO_PROTO_H */
