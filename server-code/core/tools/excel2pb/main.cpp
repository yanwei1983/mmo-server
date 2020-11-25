
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/text_format.h>

#include "BaseCode.h"
#include "ProtobuffUtil.h"
#include "StringAlgo.h"
#include "datapack/CfgDataPack.pb.h"
#include "get_opt.h"
#include "xlnt/xlnt.hpp"

void ProtobufLogHandler(google::protobuf::LogLevel level, const char* file, int32_t line, const std::string& msg)
{
    std::cerr << msg << std::endl;
}
struct PB_Initer
{
public:
    PB_Initer()
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        google::protobuf::SetLogHandler(ProtobufLogHandler);
    }
    ~PB_Initer() {}
} const s_PB_Initer;

struct ProcessData
{
    std::string                                title;
    size_t                                     y;
    size_t                                     x;
    std::unique_ptr<google::protobuf::Message> pRow;
};

void process_excel(const get_opt& opt, const xlnt::workbook& wb, const google::protobuf::Message* message_const, std::vector<ProcessData>& vecMessage)
{
    for(const auto& ws: wb)
    {
        auto rows     = ws.rows(false);
        auto row_name = rows.vector(0);

        size_t x = 0;
        size_t y = 0;
        for(auto row: rows)
        {
            if(y < 2)
            {
                y++;
                continue;
            }
            google::protobuf::Message* pPBRow  = message_const->New();
            bool                       bUpdate = false;
            for(auto cell: row)
            {
                if(x >= row_name.length())
                    continue;
                auto cell_name = row_name[x];
                if(cell_name.has_value() == false)
                    continue;
                std::string name = cell_name.to_string();
                if(name.empty() || name[0] == '#')
                {
                    x++;
                    continue;
                }

                std::string data = cell.to_string();
                if(data.empty())
                {
                    x++;
                    continue;
                }

                bool bError = false;
                try
                {
                    if(name.find('|') != std::string::npos)
                    {
                        //特殊处理
                        auto vecData = split_string(name, "|");
                        if(data == "1")
                        {
                            __attempt_call_printerr_begin()
                            {
                                if(pb_util::JoinMessageData(pPBRow, trim_copy(vecData[0]), trim_copy(vecData[1])) == false)
                                {
                                    bError = true;
                                }
                            }
                            __attempt_call_printerr_end()
                        }
                    }
                    else
                    {
                        __attempt_call_printerr_begin()
                        {

                            if(pb_util::SetMessageData(pPBRow, trim_copy(name), trim_copy(data)) == false)
                            {
                                bError = true;
                            }
                        }
                        __attempt_call_printerr_end()
                    }
                }
                catch(...)
                {
                    bError = true;
                }

                if(bError)
                {
                    fmt::print("process fail: sheet={} y={} x={} cell:{} data:{} \n", ws.title(), y + 1, x + 1, name, data);
                }
                else
                {
                    bUpdate = true;
                }

                x++;
            }

            if(bUpdate)
            {
                ProcessData data;
                data.title = ws.title();
                data.y     = y;
                data.x     = x;
                data.pRow.reset(pPBRow);
                vecMessage.emplace_back(std::move(data));
            }
            else
            {
                SAFE_DELETE(pPBRow);
            }

            x = 0;
            y++;
        }
    }
}

int main(int argc, char** argv)
{
    get_opt opt(argc, (const char**)argv);

    if(opt.has("--excel") == false || opt.has("--inputpbdir") == false || opt.has("--inputpb") == false || opt.has("--out") == false ||
       opt.has("--help") == true)
    {
        std::cout << "execl2pb [--excel=xxx.xlsx] [--inputpbdir=input.pb] [--inputpb=input.pb] [--out=output.txt]" << std::endl;
        return 0;
    }

    std::string execl_full_path = opt["--excel"];
    std::string execl_file_name = get_filename_from_fullpath(execl_full_path);
    std::string execl_name      = get_filename_without_ext(execl_file_name);
    std::string pb_file_name    = opt["--inputpb"];
    std::string pb_dir_name     = opt["--inputpbdir"];
    std::string out_file_name   = opt["--out"];

    using namespace google::protobuf;
    using namespace google::protobuf::compiler;

    DiskSourceTree sourceTree;
    sourceTree.MapPath("", pb_dir_name);
    Importer              importer(&sourceTree, nullptr);
    const FileDescriptor* fdes = importer.Import(pb_file_name);
    if(fdes == nullptr)
    {
        fmt::print("importer fail");
        return -1;
    }

    const Descriptor* desc = importer.pool()->FindMessageTypeByName(execl_name);
    if(desc == nullptr)
    {
        fmt::print("find cfgname fail");
        return -1;
    }

    DynamicMessageFactory factory;

    const Message* message_const = factory.GetPrototype(desc);

    // auto                  pPBRowFieldDesc = message_const->GetDescriptor();

    std::vector<ProcessData> vecMessage;
    std::string              debug_txt;
    xlnt::workbook           wb;
    wb.load(execl_full_path);

    if(opt.has("--showexecl"))
    {
        for(const auto& ws: wb)
        {
            auto        rows     = ws.rows(false);
            const auto& row_name = rows.vector(0);
            int32_t     x = 0, y = 0;
            for(auto row: rows)
            {
                for(auto cell: row)
                {
                    fmt::print("[{},{}]{} \t", y + 1, x + 1, cell.to_string().c_str());
                    x++;
                }
                y++;
                x = 0;
                std::cout << std::endl;
            }
        }
    }
    process_excel(opt, wb, message_const, vecMessage);

    {
        CfgDataPack output;
        output.set_size(vecMessage.size());
        for(const auto& [title, y, x, pRow]: vecMessage)
        {
            auto data = output.add_data_set();
            pRow->SerializeToString(data);
            std::string json_txt;
            pb_util::SaveToJsonTxt(*pRow, json_txt);

            if(debug_txt.empty() == false)
                debug_txt += ",\n";
            debug_txt += std::string_view{json_txt.c_str(), json_txt.size() - 2};
            debug_txt += fmt::format(",\"__debug\":\"file:{} sheet:{} line:{}  \"\n}} ", execl_file_name, title, y + 1);
        }

        pb_util::SaveToBinaryFile(output, out_file_name.c_str());
    }

    if(opt.has("--debug"))
    {
        std::string   debug_file_name = out_file_name + ".debug.json";
        std::ofstream ofs(debug_file_name.c_str(), std::ios::out | std::ios::trunc);
        if(ofs.is_open())
        {
            ofs << "[";
            ofs << debug_txt;
            ofs << "]";
            ofs.close();
        }
    }

    fmt::printf("write to file succ.\n");

    return 0;
}