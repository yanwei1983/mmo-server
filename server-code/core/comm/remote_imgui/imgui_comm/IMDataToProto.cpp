#include "IMDataToProto.h"
#include <stdint.h>              // for int32_t, uint64_t
#include <algorithm>             // for fill, max
#include <utility>               // for move
#include <vector>                // for vector
#include "IMGuiFrameDataWarp.h"  // for FrameDataWarp
#include "imgui.pb.h"            // for DrawList, FrameDataInfo, DrawCmd, Vec4
#include "imgui/imgui.h"         // for ImDrawList, ImDrawData, ImDrawCmd


namespace imdata2proto
{
    void serialize(const ImVec4&  vec, imgui::Vec4& proto)
    {
        proto.set_x(vec.x);
        proto.set_y(vec.y);
        proto.set_z(vec.z);
        proto.set_w(vec.w);
    }

    void deserialize(ImVec4&  vec, const imgui::Vec4& proto)
    {
        vec.x = proto.x();
        vec.y = proto.y();
        vec.z = proto.z();
        vec.w = proto.w();
    }

    void serialize(const ImVec2&  vec, imgui::Vec2& proto)
    {
        proto.set_x(vec.x);
        proto.set_y(vec.y);
    }

    void deserialize(ImVec2&  vec, const imgui::Vec2& proto)
    {
        vec.x = proto.x();
        vec.y = proto.y();
    }
    

    void serialize(const ImDrawCmd&  drawCmd, imgui::DrawCmd& proto)
    {
        serialize(drawCmd.ClipRect, *proto.mutable_cliprect());
        proto.set_textureid((uint64_t)drawCmd.TextureId);
        proto.set_vtxoffset(drawCmd.VtxOffset);
        proto.set_idxoffset(drawCmd.IdxOffset);
        proto.set_elemcount(drawCmd.ElemCount);
    }

    void deserialize(ImDrawCmd&  drawCmd, const imgui::DrawCmd& proto)
    {
        deserialize(drawCmd.ClipRect, proto.cliprect());
        drawCmd.TextureId = (void*)proto.textureid();
        drawCmd.VtxOffset = proto.vtxoffset();
        drawCmd.IdxOffset = proto.idxoffset();
        drawCmd.ElemCount = proto.elemcount();
    }

    void serialize(const ImDrawVert&  drawvert, imgui::DrawVert& proto)
    {
        serialize(drawvert.pos, *proto.mutable_pos());
        serialize(drawvert.uv, *proto.mutable_uv());
        proto.set_col(drawvert.col);
    }

    void deserialize(ImDrawVert&  drawvert, const imgui::DrawVert& proto)
    {
        deserialize(drawvert.pos, proto.pos());
        deserialize(drawvert.uv, proto.uv());
        drawvert.col = proto.col();
    }

     void serialize(const ImDrawData* drawData, imgui::FrameData& frame)
    {
        auto& info = *frame.mutable_info();
        info.set_valid(drawData->Valid);
        info.set_totalidxcount(drawData->TotalIdxCount);
        info.set_totalvtxcount(drawData->TotalVtxCount);
        serialize(drawData->DisplayPos, *info.mutable_displaypos());
        serialize(drawData->DisplaySize, *info.mutable_displaysize());
        serialize(drawData->FramebufferScale, *info.mutable_framebufferscale());
        
        for(int32_t iList = 0; iList < drawData->CmdListsCount; ++iList)
        {
            auto& drawlist = drawData->CmdLists[iList];
            auto& cmdlist = *frame.add_cmdlists();
            cmdlist.mutable_cmdbuffer()->Reserve(drawlist->CmdBuffer.size());
            for(const auto& v: drawlist->CmdBuffer)
            {
                imgui::DrawCmd& drawcmd = *cmdlist.add_cmdbuffer();
                serialize(v, drawcmd);
                
            }
            cmdlist.mutable_vtxbuffer()->Reserve(drawlist->VtxBuffer.size());
            for(const auto& v: drawlist->VtxBuffer)
            {
                imgui::DrawVert& drawvert = *cmdlist.add_vtxbuffer();
                serialize(v, drawvert);
            }
            cmdlist.mutable_idxbuffer()->Reserve(drawlist->IdxBuffer.size());
            for(const auto& v: drawlist->IdxBuffer)
            {
                cmdlist.add_idxbuffer(v);
            }
            cmdlist.set_flags(drawlist->Flags);
        }
    }

    void deserialize(FrameDataWarp& drawDataWarp, const imgui::FrameData& frame, const ImDrawListSharedData* drawListSharedData)
    {
        auto drawData = &drawDataWarp.drawData;
        const auto& info = frame.info();
        drawData->Valid = info.valid();
        drawData->CmdListsCount = frame.cmdlists_size();
        drawData->TotalIdxCount = info.totalidxcount();
        drawData->TotalVtxCount = info.totalvtxcount();
        deserialize(drawData->DisplayPos, info.displaypos());
        deserialize(drawData->DisplaySize, info.displaysize());
        deserialize(drawData->FramebufferScale, info.framebufferscale());
        if((int)drawDataWarp.drawLists.size() < drawData->CmdListsCount)
        {
            drawDataWarp.drawLists.resize(drawData->CmdListsCount, ImDrawList(drawListSharedData));
            drawDataWarp.cmdLists.resize(drawDataWarp.drawLists.size());
            drawData->CmdLists = &drawDataWarp.cmdLists[0];
        }

        for(int32_t i = 0; i < drawData->CmdListsCount; ++i)
        {
            drawDataWarp.cmdLists[i] = &drawDataWarp.drawLists[i];
            auto& drawlist = drawDataWarp.drawLists[i];
            const auto& drawlistdata = frame.cmdlists(i);
            drawlist.CmdBuffer.reserve(drawlistdata.cmdbuffer_size());
            for(const auto& v: drawlistdata.cmdbuffer())
            {
                ImDrawCmd drawcmd;
                deserialize(drawcmd, v);
                drawlist.CmdBuffer.push_back(std::move(drawcmd));
            }
            drawlist.VtxBuffer.reserve(drawlistdata.vtxbuffer_size());
            for(const auto& v: drawlistdata.vtxbuffer())
            {
                ImDrawVert drawvert;
                deserialize(drawvert, v);
                drawlist.VtxBuffer.push_back(std::move(drawvert));
            }
            drawlist.IdxBuffer.reserve(drawlistdata.idxbuffer_size());
            for(const auto& v: drawlistdata.idxbuffer())
            {
                drawlist.IdxBuffer.push_back(v);
            }
            drawlist.Flags = drawlistdata.flags();
        }
    }
    

} // namespace