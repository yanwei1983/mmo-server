#include "IMGuiIOData.h"
#include "imgui/imgui.h"
#include "IMDataToProto.h"
#include <algorithm>        // for copy, copy_backward, max
#include <utility>          // for move

IMGuiIOGutter::IMGuiIOGutter()
{
    
}
imgui::IOData IMGuiIOGutter::make_data(const ImGuiIO& io)
{
    imgui::IOData io_data;
    imdata2proto::serialize(io.DisplaySize, *io_data.mutable_displaysize());
    imdata2proto::serialize(io.MousePos, *io_data.mutable_mousepos());
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        io_data.add_mousedown(io.MouseDown[i]);
    }
    io_data.set_mousewheelh(io.MouseWheelH);
    io_data.set_mousewheel(io.MouseWheel);
    io_data.set_keyctrl(io.KeyCtrl);
    io_data.set_keyshift(io.KeyShift);
    io_data.set_keyalt(io.KeyAlt);
    io_data.set_keysuper(io.KeySuper);

    for(int i = 0; i < 512; i++)
    {
        if(io.KeysDown[i] == true)
        {
            io_data.add_keysdown(i);
        }
    }

    for(const auto& v : io.KeyMap)
    {
        io_data.add_keymap(v);
    }


    for(const auto& v: io.InputQueueCharacters)
    {
        io_data.add_inputqueuecharacters(v);
    }

   
    io_data.set_inputqueuesurrogate(io.InputQueueSurrogate);

    return io_data;
}

void IMGuiIOGutter::push(imgui::IOData&& io_data)
{
    m_io_data.push_back(std::move(io_data));
}

bool IMGuiIOGutter::pop(ImGuiIO& io)
{
    if(m_io_data.empty())
        return false;
    
    imgui::IOData io_data(std::move(m_io_data.front()));
    m_io_data.pop_front();
    
    if(io_data.displaysize().x() != 0 && io_data.displaysize().y() != 0)
    {
        imdata2proto::deserialize(io.DisplaySize, io_data.displaysize());
    }
    imdata2proto::deserialize(io.MousePos, io_data.mousepos());
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        io.MouseDown[i] = io_data.mousedown(i);
    }
    io.MouseWheelH = io_data.mousewheelh();
    io.MouseWheel = io_data.mousewheel();
    io.KeyCtrl = io_data.keyctrl();
    io.KeyShift = io_data.keyshift();
    io.KeyAlt = io_data.keyalt();
    io.KeySuper = io_data.keysuper();
    for(int i = 0; i < 512; i++)
    {
        io.KeysDown[i] = false;
    }
    for(const auto& k : io_data.keysdown())
    {
        io.KeysDown[k] = true;
    }

    for(int i = 0 ; i < io_data.keymap_size();i++)
    {
        io.KeyMap[i] = io_data.keymap(i);
    }
    
    io.InputQueueSurrogate = io_data.inputqueuesurrogate();
    
    io.InputQueueCharacters.resize(io_data.inputqueuecharacters_size());
    for(int i = 0; i < io_data.inputqueuecharacters_size(); i++)
    {
        io.InputQueueCharacters[i] = io_data.inputqueuecharacters(i);
    }
    return true;
}

void IMGuiIOGutter::drop_all()
{
    m_io_data.clear();
}

size_t IMGuiIOGutter::Count() const { return m_io_data.size(); }

