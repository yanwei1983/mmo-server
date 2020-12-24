#ifndef IMGUI_FRAME_DATA_H
#define IMGUI_FRAME_DATA_H

#include <deque>        // for deque
#include <string>       // for string
#include <string_view>  // for string_view
#include "imgui.pb.h"

struct FrameDataWarp;  // lines 12-12
struct ImDrawListSharedData;  // lines 9-9

struct FrameDataWarp;

class IMGuiFrameGutter
{
public:
    IMGuiFrameGutter();


    void push(imgui::FrameData&& frameproto);
    bool pop(FrameDataWarp& framedata, const ImDrawListSharedData* drawListSharedData);
    size_t GetFrames() const;

    void dump(const std::string& filename);
    void loadFromFile(const std::string& filename);


public:
    std::string diff_last_string(imgui::FrameData&& frameproto);
    imgui::FrameData diff_last_string(const std::string_view& xor_data);

private:
    std::deque<imgui::FrameData> m_frameproto;
    std::string m_last_frame_data;

};

#endif /* IMGUI_FRAME_DATA_H */
