#include "IMGuiIOData.h"
#include "imgui/imgui.h"
#include "IMDataToProto.h"

bool IMGuiIOGutter::push(const ImGuiIO& io)
{
    imgui::IOData io_data;
    imdata2proto::serialize(io.MousePos, *io_data.mutable_mousepos());
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        io_data.add_mousedown(io.MouseDown[i]);
    }
    push(std::move(io_data));

    return true;
}

void IMGuiIOGutter::push(imgui::IOData&& io_data)
{
    m_io_data.push(std::move(io_data));
}

bool IMGuiIOGutter::pop(ImGuiIO& io)
{
    if(m_io_data.empty())
        return false;
    
    imgui::IOData io_data;
    if(m_io_data.pop(io_data))
    {
        imdata2proto::deserialize(io.MousePos, io_data.mousepos());
        for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        {
            io.MouseDown[i] = io_data.mousedown(i);
        }
    }
    
    return true;
}

size_t IMGuiIOGutter::Count() const { return m_io_data.size(); }

