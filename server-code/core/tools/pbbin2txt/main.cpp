
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include "ProtobuffParse.h"
#include "ProtobuffUtil.h"
#include "StringAlgo.h"
#include "T_GameDataMap.h"
#include "datapack/CfgDataPack.pb.h"
#include "get_opt.h"

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

int main(int argc, char** argv)
{
    get_opt opt(argc, (const char**)argv);
    if(opt.has("--input") == false || opt.has("--pbdir") == false || opt.has("--pb") == false || opt.has("--help") == true)
    {
        std::cout << "pbbin2txt [--input=xxx.bytes] [--pbdir=xxxxx] [--pb=xxx.proto] [--output=output.txt]" << std::endl;
        return 0;
    }

    std::string in_file_name = opt["--input"];
    std::string pbdirname    = opt["--pbdir"];
    std::string pbname       = opt["--pb"];
    std::string cfgname      = get_filename_without_ext(get_filename_from_fullpath(pbname));
    if(opt.has("--cfg"))
        cfgname = opt["--cfg"];

    ProtoBufParser parser;
    parser.Init(pbdirname);
    if(parser.ParsePBFile(pbname) == false)
    {
        std::cerr << "importer fail:" << pbname;
        return -1;
    }

    const google::protobuf::Descriptor* desc = parser.FindDescByName(cfgname);
    if(desc == nullptr)
    {
        std::cerr << "find cfgname fail:" << cfgname;
        return -1;
    }
    google::protobuf::DynamicMessageFactory factory;
    const google::protobuf::Message*        message_const = factory.GetPrototype(desc);

    if(opt.has("--debug"))
    {
        std::cout << desc->DebugString() << std::endl;
        std::cout << "prass any key to start convert" << std::endl;
        if(opt.has("-i"))
        {
            getchar();
        }
    }

    CfgDataPack input;
    pb_util::LoadFromBinaryFile(in_file_name, input);
    std::string json_txt_all;
    for(const auto& data: input.data_set())
    {
        google::protobuf::Message* pRow = message_const->New();
        pRow->ParseFromString(data);
        std::string json_txt;
        pb_util::SaveToJsonTxt(*pRow, json_txt);
        delete(pRow);
        json_txt_all += json_txt;
    }

    if(opt.has("--debug"))
    {
        std::cout << json_txt_all << std::endl;
    }

    if(opt.has("--output"))
    {
        std::ofstream ofs(opt["--output"], std::ios::out | std::ios::trunc);
        if(ofs.is_open())
        {
            ofs << json_txt_all;
            ofs.close();
        }
    }
}