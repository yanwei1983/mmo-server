#include "IMDataToProto.h"
#include "imgui/imgui.h"
#include "imgui.pb.h"
#include "IMGuiFrameDataWarp.h"

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
        vec.y = proto.w();
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
        frame.set_valid(drawData->Valid);
        frame.set_cmdlistscount(drawData->CmdListsCount);
        frame.set_totalidxcount(drawData->TotalIdxCount);
        frame.set_totalvtxcount(drawData->TotalVtxCount);
        serialize(drawData->DisplayPos, *frame.mutable_displaypos());
        serialize(drawData->DisplaySize, *frame.mutable_displaysize());
        serialize(drawData->FramebufferScale, *frame.mutable_framebufferscale());
        
        for(int32_t iList = 0; iList < drawData->CmdListsCount; ++iList)
        {
            auto& drawlist = drawData->CmdLists[iList];
            auto cmdlist_ptr = frame.add_cmdlists();
            for(const auto& v: drawlist->CmdBuffer)
            {
                imgui::DrawCmd& drawcmd = *cmdlist_ptr->add_cmdbuffer();
                serialize(v, drawcmd);
                
            }
            for(const auto& v: drawlist->VtxBuffer)
            {
                imgui::DrawVert& drawvert = *cmdlist_ptr->add_vtxbuffer();;
                serialize(v, drawvert);
            }
            for(const auto& v: drawlist->IdxBuffer)
            {
                cmdlist_ptr->add_idxbuffer(v);
            }
            cmdlist_ptr->set_flags(drawlist->Flags);
        }
    }

    void deserialize(FrameDataWarp& drawDataWarp, const imgui::FrameData& frame, const ImDrawListSharedData* drawListSharedData)
    {
        auto drawData = &drawDataWarp.drawData;
        drawData->Valid = frame.valid();
        drawData->CmdListsCount = frame.cmdlistscount();
        drawData->TotalIdxCount = frame.totalidxcount();
        drawData->TotalVtxCount = frame.totalvtxcount();
        deserialize(drawData->DisplayPos, frame.displaypos());
        deserialize(drawData->DisplaySize, frame.displaysize());
        deserialize(drawData->FramebufferScale, frame.framebufferscale());
        if((int)drawDataWarp.drawLists.size() < drawData->CmdListsCount)
        {
            drawDataWarp.drawLists.resize(drawData->CmdListsCount, ImDrawList(drawListSharedData));
            drawDataWarp.cmdLists.resize(drawDataWarp.drawLists.size());
        }

        for(int32_t i = 0; i < drawData->CmdListsCount; ++i)
        {
            drawDataWarp.cmdLists[i] = &drawDataWarp.drawLists[i];
            auto& drawlist = drawDataWarp.drawLists[i];
            const auto& drawlistdata = frame.cmdlists(i);
            for(const auto& v: drawlistdata.cmdbuffer())
            {
                ImDrawCmd drawcmd;
                deserialize(drawcmd, v);
                drawlist.CmdBuffer.push_back(std::move(drawcmd));
            }
            for(const auto& v: drawlistdata.vtxbuffer())
            {
                ImDrawVert drawvert;
                deserialize(drawvert, v);
                drawlist.VtxBuffer.push_back(std::move(drawvert));
            }
            for(const auto& v: drawlistdata.idxbuffer())
            {
                drawlist.IdxBuffer.push_back(v);
            }
            drawlist.Flags = drawlistdata.flags();
        }
    }
    

} // namespace