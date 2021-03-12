#include "sql_code_generator.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include <fmt/format.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/stubs/scoped_ptr.h>
#include <google/protobuf/stubs/stl_util.h>
#include <google/protobuf/stubs/strutil.h>

#include "sql_options/sql_options.pb.h"
#include "AttempUtil.h"

std::string StripProto(const std::string& filename)
{
    using namespace google::protobuf;
    if(HasSuffixString(filename, ".protodevel"))
    {
        return StripSuffixString(filename, ".protodevel");
    }
    else
    {
        return StripSuffixString(filename, ".proto");
    }
}

inline std::vector<std::string> split_string(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> v;
    std::string::size_type   start = 0;
    auto                     pos   = str.find_first_of(delimiters, start);
    while(pos != std::string::npos)
    {
        if(pos != start) // ignore empty tokens
            v.emplace_back(str, start, pos - start);
        start = pos + 1;
        pos   = str.find_first_of(delimiters, start);
    }
    if(start < str.length())                              // ignore trailing delimiter
        v.emplace_back(str, start, str.length() - start); // add what's left of the string
    return v;
}

template<class T>
std::string string_concat(const T& vecStr, const std::string& delimiters, const std::string& pre, const std::string& post)
{
    std::string result;
    int32_t     i = 0;
    for(const auto& v: vecStr)
    {
        if(result.empty() == false)
            result += delimiters;

        result += pre;
        result += v;
        result += post;
    }
    return result;
}

SQLCodeGenerator::SQLCodeGenerator(const std::string& name) {}

SQLCodeGenerator::~SQLCodeGenerator() {}

const char* PrimitiveTypeName(const google::protobuf::FieldDescriptor* field_desc)
{
    using namespace google::protobuf;
    switch(field_desc->type())
    {
        case FieldDescriptor::TYPE_DOUBLE:
            return "double";
        case FieldDescriptor::TYPE_FLOAT:
            return "float";
        case FieldDescriptor::TYPE_INT64:
        case FieldDescriptor::TYPE_SINT64:
        case FieldDescriptor::TYPE_UINT64:
        case FieldDescriptor::TYPE_FIXED64:
        case FieldDescriptor::TYPE_SFIXED64:
            return "bigint";
        case FieldDescriptor::TYPE_INT32:
        case FieldDescriptor::TYPE_SINT32:
        case FieldDescriptor::TYPE_UINT32:
        case FieldDescriptor::TYPE_FIXED32:
        case FieldDescriptor::TYPE_SFIXED32:
        {
            const auto& options   = field_desc->options();
            const auto& extension = options.GetExtension(sql);
            if(extension.int_size() > 0 && extension.int_size() < 16)
            {
                return "tinyint";
            }
            else if(extension.int_size() >= 16 && extension.int_size() < 32)
            {
                return "smallint";
            }
            return "int";
        }
        case FieldDescriptor::TYPE_BOOL:
            return "tinyint";
        case FieldDescriptor::TYPE_STRING:
            return "varchar";
        case FieldDescriptor::TYPE_GROUP:
            return NULL;
        case FieldDescriptor::TYPE_MESSAGE:
            return NULL;
        case FieldDescriptor::TYPE_BYTES:
        {
            const auto& options   = field_desc->options();
            const auto& extension = options.GetExtension(sql);
            if(extension.size() > 0 && extension.size() <= 65535)
            {
                return "blob";
            }
            else if(extension.size() > 16*1024*1024)
            {
                return "longblob";
            }
            else
            {
                return "mediumblob";
            }
        }
        case FieldDescriptor::TYPE_ENUM:
            return "int";

            // No default because we want the compiler to complain if any new
            // CppTypes are added.
    }

    GOOGLE_LOG(FATAL) << "Can't get here.";
    return NULL;
}

std::string FieldUnsigned(google::protobuf::FieldDescriptor::Type type)
{
    using namespace google::protobuf;
    switch(type)
    {

        case FieldDescriptor::TYPE_UINT64:
        case FieldDescriptor::TYPE_UINT32:
        case FieldDescriptor::TYPE_FIXED64:
        case FieldDescriptor::TYPE_FIXED32:
            return " unsigned";
            break;
        default:
            return "";
            break;
    }

    return "";
}

std::string FieldName(const google::protobuf::FieldDescriptor* field)
{
    std::string result = field->lowercase_name();
    return result;
}

std::string FieldSize(const google::protobuf::FieldDescriptor* field)
{
    const auto& options   = field->options();
    const auto& extension = options.GetExtension(sql);

    using namespace google::protobuf;
    switch(field->type())
    {
        case FieldDescriptor::TYPE_STRING:
        {
            if(extension.size() > 0)
                return attempt_format("({})", extension.size());
        }
        break;
        case FieldDescriptor::TYPE_DOUBLE:
        {
            return "(24,6)";
        }
        break;
        case FieldDescriptor::TYPE_FLOAT:
        {
            return "(24,6)";
        }
        break;
        case FieldDescriptor::TYPE_FIXED64:
        case FieldDescriptor::TYPE_SFIXED64:
        case FieldDescriptor::TYPE_INT64:
        case FieldDescriptor::TYPE_UINT64:
        case FieldDescriptor::TYPE_SINT64:
        case FieldDescriptor::TYPE_FIXED32:
        case FieldDescriptor::TYPE_SFIXED32:
        case FieldDescriptor::TYPE_INT32:
        case FieldDescriptor::TYPE_UINT32:
        case FieldDescriptor::TYPE_SINT32:
        case FieldDescriptor::TYPE_BOOL:
        case FieldDescriptor::TYPE_ENUM:
        default:
        {
            return "";
        }
        break;
    }

    return "";
}

std::string FieldUTF8(google::protobuf::FieldDescriptor::Type type)
{
    if(type == google::protobuf::FieldDescriptor::TYPE_STRING)
    {
        return " CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci";
    }
    return "";
}

std::string AutoIncrement(const google::protobuf::FieldDescriptor* field)
{
    const auto& options   = field->options();
    const auto& extension = options.GetExtension(sql);
    if(extension.auto_increment())
    {
        return " AUTO_INCREMENT";
    }
    return "";
}

std::string PrimaryKey(const google::protobuf::FieldDescriptor* field)
{
    const auto& options   = field->options();
    const auto& extension = options.GetExtension(sql);
    return extension.primary_key();
}

std::string Key(const google::protobuf::FieldDescriptor* field)
{
    const auto& options   = field->options();
    const auto& extension = options.GetExtension(sql);
    return extension.keys();
}

std::string Unique(const google::protobuf::FieldDescriptor* field)
{
    const auto& options   = field->options();
    const auto& extension = options.GetExtension(sql);
    return extension.unique();
}

std::string DefaultVal(const google::protobuf::FieldDescriptor* field)
{
    const auto& options   = field->options();
    const auto& extension = options.GetExtension(sql);
    if(extension.default_val().empty() == false)
    {
        return " DEFAULT '" + extension.default_val() + "'";
    }
    else
    {
        using namespace google::protobuf;
        switch(field->type())
        {
            case FieldDescriptor::TYPE_DOUBLE:
            case FieldDescriptor::TYPE_FLOAT:
                return " DEFAULT '0.000000'";
            case FieldDescriptor::TYPE_FIXED64:
            case FieldDescriptor::TYPE_SFIXED64:
            case FieldDescriptor::TYPE_INT64:
            case FieldDescriptor::TYPE_UINT64:
            case FieldDescriptor::TYPE_SINT64:
            case FieldDescriptor::TYPE_FIXED32:
            case FieldDescriptor::TYPE_SFIXED32:
            case FieldDescriptor::TYPE_INT32:
            case FieldDescriptor::TYPE_UINT32:
            case FieldDescriptor::TYPE_SINT32:
            case FieldDescriptor::TYPE_BOOL:
            case FieldDescriptor::TYPE_ENUM:
                return " DEFAULT '0'";
            case FieldDescriptor::TYPE_STRING:
                return " DEFAULT ''";
            case FieldDescriptor::TYPE_BYTES:
            default:
                return "";
        }

        return "";
    }
}

std::string Notnull(const google::protobuf::FieldDescriptor* field)
{
    using namespace google::protobuf;
    switch(field->type())
    {
        case FieldDescriptor::TYPE_BYTES:
            return "";
        case FieldDescriptor::TYPE_DOUBLE:
        case FieldDescriptor::TYPE_FLOAT:
        case FieldDescriptor::TYPE_FIXED64:
        case FieldDescriptor::TYPE_SFIXED64:
        case FieldDescriptor::TYPE_INT64:
        case FieldDescriptor::TYPE_UINT64:
        case FieldDescriptor::TYPE_SINT64:
        case FieldDescriptor::TYPE_FIXED32:
        case FieldDescriptor::TYPE_SFIXED32:
        case FieldDescriptor::TYPE_INT32:
        case FieldDescriptor::TYPE_UINT32:
        case FieldDescriptor::TYPE_SINT32:
        case FieldDescriptor::TYPE_BOOL:
        case FieldDescriptor::TYPE_ENUM:
        case FieldDescriptor::TYPE_STRING:
        default:
            return " NOT NULL";
    }
    return " NOT NULL";
}

std::string Comment(const google::protobuf::FieldDescriptor* field)
{
    google::protobuf::SourceLocation out_location;
    field->GetSourceLocation(&out_location);
    std::string comments = out_location.trailing_comments;
    google::protobuf::ReplaceCharacters(&comments, "\n", ' ');
    google::protobuf::ReplaceCharacters(&comments, "\r", ' ');
    google::protobuf::StripWhitespace(&comments);
    if(comments.empty() == false)
        return attempt_format(" COMMENT '{}'", comments);
    return comments;
}

void PrintMessage(const google::protobuf::Descriptor& message_descriptor, google::protobuf::io::Printer& printer)
{
    printer.Print("DROP TABLE IF EXISTS `$name$`;\n", "name", message_descriptor.name());
    printer.Print("CREATE TABLE `$name$` (\n", "name", message_descriptor.name());

    std::vector<std::string>                                   primary_key_list;
    std::unordered_map<std::string, std::vector<std::string> > keys_list;
    std::unordered_map<std::string, std::vector<std::string> > unique_list;

    for(int32_t i = 0; i < message_descriptor.field_count(); ++i)
    {
        auto desc = message_descriptor.field(i);

        auto type_str       = PrimitiveTypeName(desc);
        auto name_str       = FieldName(desc);
        auto unsigned_str   = FieldUnsigned(desc->type());
        auto utf8_str       = FieldUTF8(desc->type());
        auto size_str       = FieldSize(desc);
        auto notnull_str    = Notnull(desc);
        auto default_str    = DefaultVal(desc);
        auto primary_key    = PrimaryKey(desc);
        auto key_str        = Key(desc);
        auto auto_increment = AutoIncrement(desc);
        auto unique_str     = Unique(desc);
        auto comment        = Comment(desc);

        if(auto_increment.empty() == false)
            default_str.clear();

        if(primary_key.empty() == false)
        {
            primary_key_list.push_back(name_str);
        }

        auto vecKey = split_string(key_str, ",");
        for(auto k: vecKey)
        {
            google::protobuf::StripWhitespace(&k);
            keys_list[k].push_back(name_str);
        }
        auto vecUnique = split_string(unique_str, ",");
        for(auto k: vecUnique)
        {
            google::protobuf::StripWhitespace(&k);
            unique_list[k].push_back(name_str);
        }

        std::string str = attempt_format("  `{name}` {type}{size}{unsigned}{utf8}{notnull}{default}{auto_inc}{comment},\n",
                                      fmt::arg("name", name_str),
                                      fmt::arg("type", type_str),
                                      fmt::arg("size", size_str),
                                      fmt::arg("unsigned", unsigned_str),
                                      fmt::arg("utf8", utf8_str),
                                      fmt::arg("notnull", notnull_str),
                                      fmt::arg("default", default_str),
                                      fmt::arg("auto_inc", auto_increment),
                                      fmt::arg("comment", comment));
        printer.PrintRaw(str);
    }

    std::vector<std::string> key_str_list;
    {
        std::string key_str = attempt_format("  PRIMARY KEY ({})", string_concat(primary_key_list, ",", "`", "`"));
        key_str_list.emplace_back(std::move(key_str));
    }
    for(const auto& [k, vecList]: unique_list)
    {
        std::string key_str = attempt_format("  UNIQUE KEY `{}` ({})", k, string_concat(vecList, ",", "`", "`"));
        key_str_list.emplace_back(std::move(key_str));
    }
    for(const auto& [k, vecList]: keys_list)
    {
        std::string key_str = attempt_format("  KEY `{}` ({})", k, string_concat(vecList, ",", "`", "`"));
        key_str_list.emplace_back(std::move(key_str));
    }

    std::string keys_str = string_concat(key_str_list, ",\n", "", "");
    printer.PrintRaw(keys_str);

    printer.Print("\n) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;\n\n");
}

bool SQLCodeGenerator::Generate(const google::protobuf::FileDescriptor*       file,
                                const std::string&                            parameter,
                                google::protobuf::compiler::GeneratorContext* context,
                                std::string*                                  error) const
{
    using namespace google::protobuf;

    std::string basename = StripProto(file->name());
    basename.append(".pb.sql");

    std::string         file_descriptor_serialized_;
    FileDescriptorProto file_proto;
    file->CopyTo(&file_proto);
    file_proto.SerializeToString(&file_descriptor_serialized_);

    scoped_ptr<io::ZeroCopyOutputStream> output(context->Open(basename));
    GOOGLE_CHECK(output.get());
    google::protobuf::io::Printer printer(output.get(), '$');

    for(int32_t i = 0; i < file->message_type_count(); ++i)
    {
        PrintMessage(*file->message_type(i), printer);
        printer.Print("\n");
    }

    return true;
}
