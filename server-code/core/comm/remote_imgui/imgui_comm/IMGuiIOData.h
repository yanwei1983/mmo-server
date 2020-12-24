#ifndef IMGUI_IO_DATA_H
#define IMGUI_IO_DATA_H

#include "imgui.pb.h"
#include <deque>

struct ImGuiIO;

class IMGuiIOGutter
{
public:
    IMGuiIOGutter();
    static imgui::IOData make_data(const ImGuiIO& io);
    void push(imgui::IOData&& io_data);
    bool pop(ImGuiIO& io);

    size_t Count() const;
    void drop_all();
private:
    std::deque<imgui::IOData> m_io_data;
};

#endif /* IMGUI_IO_DATA_H */
