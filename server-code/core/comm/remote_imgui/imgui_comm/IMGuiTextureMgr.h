#ifndef IMGUI_TEXTURE_MGR_H
#define IMGUI_TEXTURE_MGR_H

#include "imgui.pb.h"

#include <map>

class IMGuiTextureMgr
{
public:
    void store_texture(uint32_t tex_id, int32_t width, int32_t height, int32_t format, int32_t type, unsigned char * pixels)
    {
        auto& texture_data = m_texture_data[tex_id];
        texture_data.set_tex_id(tex_id);
        texture_data.set_width(width);
        texture_data.set_height(height);
        texture_data.set_format(format);
        texture_data.set_type(type);
        texture_data.set_pixels(pixels, width*height);
        
    }
    
    template<class Func>
    void foreach(Func&& func) const
    {
        for(const auto& pair_v : m_texture_data)
        {
            func(pair_v);
        }
    }
    std::map<uint32_t, imgui::TextureData> m_texture_data;
};



#endif /* IMGUI_TEXTURE_MGR_H */
