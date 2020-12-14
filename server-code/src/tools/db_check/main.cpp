#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>

#include "BaseCode.h"
#include "CheckUtil.h"
#include "MysqlConnection.h"
#include "MysqlTableCheck.h"
#include "NormalCrypto.h"
#include "UIDFactory.h"
#include "gamedb.h"
#include "get_opt.h"
#include "globaldb.h"
#include "serverinfodb.h"

CMysqlConnectionPtr ConnectGameDB(CMysqlConnection* pGlobalDB, uint32_t worldid)
{
    auto result = pGlobalDB->QueryKeyLimit<TBLD_DBINFO, TBLD_DBINFO::WORLDID>(worldid, 1);
    if(result == nullptr || result->get_num_row() == 0)
    {
        LOGFATAL("CWorldService::Create fail:gamedb info error");
        return nullptr;
    }

    auto row = result->fetch_row(false);
    if(row)
    {
        std::string db_url         = row->Field(TBLD_DBINFO::DB_URL).get<std::string>();
        auto        real_mysql_url = NormalCrypto::default_instance().Decode(db_url);
        auto        pDB            = std::make_unique<CMysqlConnection>();

        if(pDB->Connect(real_mysql_url) == false)
        {
            return nullptr;
        }
        return pDB;
    }

    return nullptr;
}

int main(int argc, char** argv)
{

    __ENTER_FUNCTION
    get_opt opt(argc, (const char**)argv);

    std::string serverinfo_url = opt["--surl"];
    uint32_t    zone_id        = atoi(opt["--zone_id"].c_str());

    std::string logpath = "./log/";
    if(opt.has("--logpath"))
    {
        logpath = opt["--logpath"];
    }
    BaseCode::InitLog(logpath + "/db_check");
    BaseCode::SetNdc("db_check");

    if(opt.has("--cryptourl"))
    {
        serverinfo_url = NormalCrypto::default_instance().Decode(serverinfo_url);
    }

    if(serverinfo_url.empty() == true)
    {
        fmt::printf("example:  db_check --surl=xxx --zone_id=x --cryptourl --fix --logpath=xxx ");
        return -1;
    }

    CMysqlConnection serverinfodb;
    CHECKF(serverinfodb.Connect(serverinfo_url));
    if(opt.has("--fix"))
    {
        fmt::printf("start check and fix ServerInfoDB!\n");
        CHECKF(MysqlTableCheck::CheckAllTableAndFix<SERVERINFODB_TABLE_LIST>(&serverinfodb));
        fmt::printf("procrss finish\n");
    }
    else
    {
        fmt::printf("start check ServerInfoDB!\n");
        CHECKF(MysqlTableCheck::CheckAllTable<SERVERINFODB_TABLE_LIST>(&serverinfodb));
        fmt::printf("procrss finish\n");
    }

    auto globaldb = ConnectGameDB(&serverinfodb, 0);
    CHECKF(globaldb);

    if(opt.has("--fix"))
    {
        fmt::printf("start check and fix GlobalDB and ZoneDB!\n");
        CHECKF(MysqlTableCheck::CheckAllTableAndFix<GLOBALDB_TABLE_LIST>(globaldb.get()));
        if(zone_id != 0)
        {
            auto zonedb = ConnectGameDB(&serverinfodb, zone_id);
            CHECKF(zonedb);
            CHECKF(MysqlTableCheck::CheckAllTableAndFix<GAMEDB_TABLE_LIST>(zonedb.get()));
        }
        fmt::printf("procrss finish\n");
    }
    else
    {
        fmt::printf("start check  GlobalDB and ZoneDB!\n");
        CHECKF(MysqlTableCheck::CheckAllTable<GLOBALDB_TABLE_LIST>(globaldb.get()));
        if(zone_id != 0)
        {
            auto zonedb = ConnectGameDB(&serverinfodb, zone_id);
            CHECKF(zonedb);
            CHECKF(MysqlTableCheck::CheckAllTable<GAMEDB_TABLE_LIST>(zonedb.get()));
        }
        fmt::printf("procrss finish\n");
    }

    fmt::printf("db_check finish\n");
    return 0;
    __LEAVE_FUNCTION
    return -1;
}
