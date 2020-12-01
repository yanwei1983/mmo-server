#ifndef DATAPACK_H
#define DATAPACK_H

#include "ProtobuffUtil.h"
#include "datapack/CfgDataPack.pb.h"
namespace DataPack
{
    template<class cfg_message_t>
    bool LoadFromBinaryFile(const std::string& filename, std::vector<cfg_message_t>& vecData)
    {
        CfgDataPack input;
        if(pb_util::LoadFromBinaryFile(filename, input) == false)
        {
            LOGERROR("InitFromFile {} Fail.", filename);
            return false;
        }

        if(input.data_name().empty() == false && input.data_name() != cfg_message_t::descriptor()->name())
        {
            LOGERROR("InitFromFile {} Fail, dataname {} not same {}.", filename, input.data_name(),cfg_message_t::descriptor()->name() );
            return false;
        }
        for(const auto& data: input.data_set())
        {
            cfg_message_t cfg;
            cfg.ParseFromString(data);

            vecData.emplace_back(std::move(cfg));
        }
        return true;
    }

    template<class cfg_message_t>
    bool SaveToBinaryFile(const std::vector<cfg_message_t>& vecData, const std::string& filename)
    {
        CfgDataPack output;
        output.set_size(vecData.size());
        output.set_data_name(cfg_message_t::descriptor()->name());
        for(const cfg_message_t& row: vecData)
        {
            auto data = output.add_data_set();
            row.SerializeToString(data);
        }

        return pb_util::SaveToBinaryFile(output, filename.c_str());
    }
} // namespace DataPack

#endif // DATAPACK_H