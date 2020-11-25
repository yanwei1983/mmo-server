
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include <fmt/format.h>
#include <fmt/printf.h>

#include "StringAlgo.h"
#include "get_opt.h"

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "ddl2cpp [in_file_name] [out_dir] [out_file_name]" << std::endl;
        return 0;
    }
    get_opt opt(argc, (const char**)argv);

    std::string in_file_name  = opt["--input"];
    std::string out_dir       = opt["--outdir"];
    std::string out_file_name = opt["--output"];
    bool        bDebug        = opt.has("--debug");

    std::ifstream input_file(in_file_name);
    if(input_file.is_open() == false)
    {
        std::cout << in_file_name << " open fail" << std::endl;
        return -1;
    }
    std::string input_string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

    //将输入按;进行分割
    std::string output_header;
    std::string output_cpp;

    std::vector<std::string> table_name_list;
    auto                     vecString = split_string(input_string, ";");
    for(auto& create_sql: vecString)
    {
        if(bDebug)
        {
            std::cout << "string1:" << create_sql << std::endl;
        }

        std::string regextxt = R"(CREATE TABLE `(.*)` \(([\s\S]*)\) ENGINE=InnoDB (.*))";
        std::smatch base_match;
        if(std::regex_search(create_sql, base_match, std::regex(regextxt)))
        {
            std::string table_name    = base_match[1];
            std::string content       = base_match[2];
            auto        vec_field_sql = split_string(content, "\n");

            struct field_type_t
            {
                std::string field_name;
                std::string field_type;
                std::string field_comment;
            };
            std::vector<field_type_t>                    vecFieldType;
            std::unordered_set<std::string>              PriKeys;
            std::vector<std::string>                     vec_match_field_sql;
            std::unordered_map<std::string, std::string> Keys;

            for(auto& field_sql: vec_field_sql)
            {
                rtrim(field_sql);
                std::smatch field_match;
                if(std::regex_search(field_sql, field_match, std::regex{R"(.*PRIMARY KEY \((.*)\).*)"}))
                {
                    std::string prikey_str = field_match[1];
                    replace_str(prikey_str, "`", "");
                    auto prikeys = split_string(prikey_str, ",");
                    for(const auto& v: prikeys)
                    {
                        PriKeys.insert(v);
                    }
                    Keys["PRIMARY"] = prikey_str;
                }
                else if(std::regex_search(field_sql, field_match, std::regex{R"(.*UNIQUE KEY `(.*)` \((.*)\).*)"}))
                {
                    std::string keyname_str  = field_match[1];
                    std::string keyfield_str = field_match[2];
                    replace_str(keyfield_str, "`", "");
                    Keys[keyname_str] = keyfield_str;
                }
                else if(std::regex_search(field_sql, field_match, std::regex{R"(.*KEY `(.*)` \((.*)\).*)"}))
                {
                    std::string keyname_str  = field_match[1];
                    std::string keyfield_str = field_match[2];
                    replace_str(keyfield_str, "`", "");
                    Keys[keyname_str] = keyfield_str;
                }
                else if(std::regex_search(field_sql, field_match, std::regex{R"(.*`(.*)` (.*) NOT NULL.*COMMENT ('.*').*)"}))
                {
                    std::string field_name    = field_match[1];
                    std::string field_type    = field_match[2];
                    std::string field_comment = field_match[3];
                    vecFieldType.push_back({field_name, field_type, field_comment});
                    vec_match_field_sql.push_back(replace_str(field_sql, ",", ""));
                }
                else if(std::regex_search(field_sql, field_match, std::regex{R"(.*`(.*)` (.*) COMMENT ('.*').*)"}))
                {
                    std::string field_name    = field_match[1];
                    std::string field_type    = field_match[2];
                    std::string field_comment = field_match[3];
                    vecFieldType.push_back({field_name, field_type, field_comment});
                    vec_match_field_sql.push_back(replace_str(field_sql, ",", ""));
                }
                else if(std::regex_search(field_sql, field_match, std::regex{R"(.*`(.*)` (.*) NOT NULL.*)"}))
                {
                    std::string field_name = field_match[1];
                    std::string field_type = field_match[2];
                    vecFieldType.push_back({field_name, field_type, ""});
                    vec_match_field_sql.push_back(replace_str(field_sql, ",", ""));
                }
            }

            std::string              fields_enum_list;
            std::vector<std::string> fields_tuple;
            std::vector<std::string> field_type_cpp_list;
            fields_tuple.resize(vecFieldType.size());

            for(uint32_t i = 0; i < vecFieldType.size(); i++)
            {
                const auto& field_type_data = vecFieldType[i];
                std::smatch field_match;
                if(field_type_data.field_name.empty() == false)
                {
                    std::string& field_tuple = fields_tuple[i];

                    if(fields_enum_list.empty() == false)
                        fields_enum_list += "\t\t";

                    field_tuple = "\"" + field_type_data.field_name + "\"";

                    std::string field_name_UP = upper_cast_copy(field_type_data.field_name);
                    fields_enum_list += field_name_UP + ",//" + field_type_data.field_comment + "\n";
                    //  `id` bigint unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
                    if(std::regex_search(field_type_data.field_type, field_match, std::regex{R"((.*)\((.*)\)(.*))"}))
                    {
                        std::string field_type = field_match[1];
                        std::string field_bits = field_match[2];
                        bool        bUnsigned  = field_match.size() > 2 && lower_cast_copy(trim_copy(field_match[3])) == "unsigned";
                        std::string field_type_cpp;
                        std::string field_type_enum;
                        if(field_type == "float")
                        {
                            field_type_cpp  = "float ";
                            field_type_enum = "DB_FIELD_TYPE_FLOAT";
                        }
                        else if(field_type == "double")
                        {
                            field_type_cpp  = "double ";
                            field_type_enum = "DB_FIELD_TYPE_DOUBLE";
                        }
                        else if(field_type == "varchar")
                        {
                            if(field_bits.empty())
                            {
                                field_type_cpp  = "std::string ";
                                field_type_enum = "DB_FIELD_TYPE_VARCHAR";
                            }
                            else
                            {
                                field_type_cpp  = "char[" + field_bits + "] ";
                                field_type_enum = "DB_FIELD_TYPE_VARCHAR";
                            }
                        }
                        else if(field_type == "blob")
                        {
                            field_type_cpp  = "std::string ";
                            field_type_enum = "DB_FIELD_TYPE_BLOB";
                        }

                        field_type_cpp_list.push_back(field_type_cpp);
                        field_tuple += ",\"" + vec_match_field_sql[i] + "\"";
                        field_tuple += "," + field_type_enum;

                        if(PriKeys.find(field_type_data.field_name) != PriKeys.end())
                            field_tuple += ",true";
                        else
                            field_tuple += ",false";
                    }
                    else if(std::regex_search(field_type_data.field_type, field_match, std::regex{R"((.*) (.*))"}))
                    {
                        std::string field_type = field_match[1];
                        bool        bUnsigned  = field_match.size() > 1 && lower_cast_copy(trim_copy(field_match[2])) == "unsigned";
                        std::string field_type_cpp;
                        std::string field_type_enum;
                        if(field_type == "bigint")
                        {
                            if(bUnsigned)
                            {
                                field_type_cpp  = "uint64_t ";
                                field_type_enum = "DB_FIELD_TYPE_LONGLONG_UNSIGNED";
                            }
                            else
                            {
                                field_type_cpp  = "int64_t ";
                                field_type_enum = "DB_FIELD_TYPE_LONGLONG";
                            }
                        }
                        else if(field_type == "int")
                        {
                            if(bUnsigned)
                            {
                                field_type_cpp  = "uint32_t ";
                                field_type_enum = "DB_FIELD_TYPE_LONG_UNSIGNED";
                            }
                            else
                            {
                                field_type_cpp  = "int32_t ";
                                field_type_enum = "DB_FIELD_TYPE_LONG";
                            }
                        }
                        else if(field_type == "smallint")
                        {
                            if(bUnsigned)
                            {
                                field_type_cpp  = "uint16_t ";
                                field_type_enum = "DB_FIELD_TYPE_SHORT_UNSIGNED";
                            }
                            else
                            {
                                field_type_cpp  = "int16_t ";
                                field_type_enum = "DB_FIELD_TYPE_SHORT";
                            }
                        }
                        else if(field_type == "tinyint")
                        {
                            if(bUnsigned)
                            {
                                field_type_cpp  = "uint8_t ";
                                field_type_enum = "DB_FIELD_TYPE_TINY_UNSIGNED";
                            }
                            else
                            {
                                field_type_cpp  = "int8_t ";
                                field_type_enum = "DB_FIELD_TYPE_TINY";
                            }
                        }
                        else if(field_type == "blob")
                        {
                            field_type_cpp  = "std::string ";
                            field_type_enum = "DB_FIELD_TYPE_BLOB";
                        }

                        field_type_cpp_list.push_back(field_type_cpp);
                        field_tuple += ",\"" + vec_match_field_sql[i] + "\"";
                        field_tuple += "," + field_type_enum;

                        if(PriKeys.find(field_type_data.field_name) != PriKeys.end())
                            field_tuple += ",true";
                        else
                            field_tuple += ",false";
                    }
                }
            }

            std::string field_output_format = R"---(
struct {0}
{{
	static constexpr const char* table_name() {{ return "{1}";}} 
	enum FIELD_ENUMS
	{{
		{2}
	}};

	static constexpr auto field_info()
    {{ 
        return std::make_tuple({3});
    }}

	using field_type_t = type_list<{4}>;

    static constexpr size_t field_count() {{return {5};}}

    static constexpr auto keys_info()
    {{ 
        return std::make_tuple({6});
    }}

    static constexpr size_t keys_size() {{ return {7}; }}

    static constexpr const char* create_sql() 
    {{ 
        return R"##({8})##";
    }};

}};
    

		)---";

            std::string table_name_UP = upper_cast_copy(table_name);

            std::string              field_tuple_str = string_concat(fields_tuple, ",", "std::make_tuple(", ")");
            std::string              field_types_str = string_concat(field_type_cpp_list, ",", "", "");
            std::string              field_sql_str   = string_concat(vec_match_field_sql, ",", "\"", "\"");
            std::vector<std::string> vec_key_typle_str;
            vec_key_typle_str.reserve(Keys.size());
            for(const auto& [key, value]: Keys)
            {
                vec_key_typle_str.push_back(fmt::format("std::make_tuple(\"{}\", \"{}\")", key, value));
            }

            std::string szBuf = fmt::format(field_output_format,
                                            table_name_UP,
                                            table_name,
                                            fields_enum_list,
                                            field_tuple_str,
                                            field_types_str,
                                            field_type_cpp_list.size(),
                                            vec_key_typle_str.empty() ? "\"\"" : string_concat(vec_key_typle_str, ",", "", ""),
                                            Keys.size(),
                                            trim_copy(create_sql));
            table_name_list.push_back(table_name_UP);
            output_header += szBuf;
            if(bDebug)
            {
                fmt::printf("%s", szBuf);
            }
        }
    }

    std::string table_list_str;
    {
        std::string out_file_name_UP     = upper_cast_copy(out_file_name);
        std::string table_name_tuple_str = string_concat(table_name_list, ",", "", "");
        table_list_str                   = fmt::format("\nusing {}_TABLE_LIST = type_list<{}>;\n", out_file_name_UP, table_name_tuple_str);
    }
    {
        static std::string output_format    = R"(
            #ifndef {0}_H
            #define {0}_H
            #include <string>

            #include "BaseCode.h"
            #include "DBField.h"
            {1}
            {2}
            #endif
            )";
        std::string        output_file_name = out_dir + out_file_name + ".h";
        std::ofstream      output_file(output_file_name);
        output_file << fmt::format(output_format, out_file_name, output_header, table_list_str);
        output_file.close();
        std::cout << output_file_name << " write succ." << std::endl;
        if(opt.has("--format"))
        {
            system(fmt::format("{} -i {}", opt["--format"], out_dir + out_file_name + ".h").c_str());
        }
    }
}