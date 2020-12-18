#ifndef IMGUI_IO_DATA_H
#define IMGUI_IO_DATA_H

#include "imgui.pb.h"
#include "LockfreeQueue.h"

struct ImGuiIO;

class IMGuiIOGutter
{
public:
    IMGuiIOGutter();
    bool push(const ImGuiIO& io);
    void push(imgui::IOData&& io_data);
    bool pop(ImGuiIO& io);

    size_t Count() const;

private:
    MPSCQueue<imgui::IOData> m_io_data;
};

#endif /* IMGUI_IO_DATA_H */
