#include <functional>
#include <tuple>

#include "BaseCode.h"
#include "StaticReflection.h"
#include "StaticReflectionV2.h"
#include "tinyxml2/tinyxml2.h"

std::string xml_txt =
    R"(
<?xml version="1.0" encoding="UTF-8"?>
<Root>
    <Nodes>
        <AllNode>
            <TestNodeA name="AllNode_a">
                <int name="TestNodeA_a" value="1"/>
                <int name="TestNodeA_b" value="2"/>
                <int name="TestNodeA_c" value="3"/>
            </TestNodeA>

            <TestNodeB name="AllNode_b">
                <int name="TestNodeB_a" value="4"/>
                <TestNodeA name="TestNodeB_b">
                    <int name="TestNodeA_a" value="5"/>
                    <int name="TestNodeA_b" value="6"/>
                    <int name="TestNodeA_c" value="7"/>
                </TestNodeA>
                <float name="TestNodeB_c" value="37"/>
            </TestNodeB>
        </AllNode>

    </Nodes>
</Root>
)";

struct string_to_hash_tag
{
};
void after_process_TestNodeB_c(tinyxml2::XMLElement*, float* val);

struct TestNodeA
{
    int32_t TestNodeA_a;
    int32_t TestNodeA_b;
    int32_t TestNodeA_c;
};
DEFINE_STRUCT_SCHEMA(TestNodeA,
                     DEFINE_STRUCT_FIELD(TestNodeA_a, "TestNodeA_a"),
                     DEFINE_STRUCT_FIELD(TestNodeA_b, "TestNodeA_b"),
                     DEFINE_STRUCT_FIELD(TestNodeA_c, "TestNodeA_c"));

DEFINE_META(TestNodeA,
            DEFINE_MEMBER(META_MEMBER(TestNodeA_a, "TestNodeA_a"), META_MEMBER(TestNodeA_b, "TestNodeA_b"), META_MEMBER(TestNodeA_c, "TestNodeA_c")));

struct TestNodeB
{
    uint32_t  TestNodeB_a;
    TestNodeA TestNodeB_b;
    float     TestNodeB_c;
};

DEFINE_STRUCT_SCHEMA(TestNodeB,
                     DEFINE_STRUCT_FIELD_TAG(TestNodeB_a, "TestNodeB_a", string_to_hash_tag()),
                     DEFINE_STRUCT_FIELD(TestNodeB_b, "TestNodeB_b"),
                     DEFINE_STRUCT_FIELD_FUNC(TestNodeB_c, "TestNodeB_c", after_process_TestNodeB_c));

DEFINE_META(TestNodeB,
            DEFINE_MEMBER(META_MEMBER_TAG(TestNodeB_a, "TestNodeB_a", string_to_hash_tag),
                          META_MEMBER_FUNC(TestNodeB_c, "TestNodeB_c", after_process_TestNodeB_c),
                          META_MEMBER(TestNodeB_b, "TestNodeB_b")));

struct AllNode
{
    TestNodeA AllNode_a;
    TestNodeB AllNode_b;
};

DEFINE_STRUCT_SCHEMA(AllNode, DEFINE_STRUCT_FIELD(AllNode_a, "AllNode_a"), DEFINE_STRUCT_FIELD(AllNode_b, "AllNode_b"));
DEFINE_META(AllNode, DEFINE_MEMBER(META_MEMBER(AllNode_a, "AllNode_a"), META_MEMBER(AllNode_b, "AllNode_b")));

// forward decal
template<class T>
inline void xmlElement_to_struct(tinyxml2::XMLElement* pE, T& refStruct);

template<class FieldType>
inline void xml_value_to_field(tinyxml2::XMLElement* pVarE, FieldType* field)
{
    if constexpr(std::is_integral<FieldType>::value || std::is_floating_point<FieldType>::value || std::is_same<bool, FieldType>::value)
    {
        pVarE->QueryAttribute("val", field);
    }
    else if constexpr(std::is_same<std::string, FieldType>::value)
    {
        const char* pVal = pVarE->Attribute("val");
        if(pVal)
        {
            *field = pVal;
        }
    }
    else
    {
        xmlElement_to_struct(pVarE, *field);
    }
}

template<class FieldType>
inline void xml_value_to_field(tinyxml2::XMLElement* pVarE, FieldType* field, void (*after_func)(tinyxml2::XMLElement*, FieldType*))
{
    xml_value_to_field(pVarE, field);
    after_func(pVarE, field);
}

inline void xml_value_to_field(tinyxml2::XMLElement* pVarE, uint32_t* field, const string_to_hash_tag&)
{
    const char* pVal = pVarE->Attribute("val");
    if(pVal)
    {
        *field = hash::MurmurHash3::shash(pVal, strlen(pVal), 0);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ForEachXMLLambda
{
    tinyxml2::XMLElement* pVarE;
    const char*           field_name;
    std::size_t           field_name_hash;
    template<typename FieldInfo, typename Field>
    bool operator()(FieldInfo&& this_field_info, Field&& this_field) const
    {
        printf("%s test:%s\n", field_name, std::get<0>(this_field_info));
        if(field_name_hash != std::get<1>(this_field_info))
            return false;
        printf("%s vist:%s\n", field_name, std::get<0>(this_field_info));
        xml_value_to_field(pVarE, &this_field);
        return true;
    }

    template<typename FieldInfo, typename Field, typename Tag>
    bool operator()(FieldInfo&& this_field_info, Field&& this_field, Tag&& tag) const
    {
        printf("%s test:%s\n", field_name, std::get<0>(this_field_info));
        if(field_name_hash != std::get<1>(this_field_info))
            return false;
        printf("%s vist:%s\n", field_name, std::get<0>(this_field_info));
        xml_value_to_field(pVarE, &this_field, std::forward<Tag>(tag));
        return true;
    }
};

template<class T>
inline void xmlElement_to_struct(tinyxml2::XMLElement* pE, T& refStruct)
{
    tinyxml2::XMLElement* pVarE = pE->FirstChildElement();
    while(pVarE != NULL)
    {
        const char* pStrName = pVarE->Attribute("name");
        if(pStrName != NULL)
        {
            std::string field_name      = pStrName;
            std::size_t field_name_hash = hash::MurmurHash3::shash(field_name.c_str(), field_name.size(), 0);
            static_reflection::FindInField_Index(refStruct, field_name_hash, ForEachXMLLambda{pVarE, field_name.c_str(), field_name_hash});
        }

        pVarE = pVarE->NextSiblingElement();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// forward decal
template<class T>
inline void xmlElement_to_struct_v2(tinyxml2::XMLElement* pE, T& refStruct);

template<class FieldType>
inline void xml_value_to_field_v2(tinyxml2::XMLElement* pVarE, FieldType* field)
{
    if constexpr(std::is_integral<FieldType>::value || std::is_floating_point<FieldType>::value || std::is_same<bool, FieldType>::value)
    {
        pVarE->QueryAttribute("val", field);
    }
    else if constexpr(std::is_same<std::string, FieldType>::value)
    {
        const char* pVal = pVarE->Attribute("val");
        if(pVal)
        {
            *field = pVal;
        }
    }
    else
    {
        xmlElement_to_struct_v2(pVarE, *field);
    }
}

inline void xml_value_to_field_v2(tinyxml2::XMLElement* pVarE, uint32_t* field, const string_to_hash_tag&)
{
    const char* pVal = pVarE->Attribute("val");
    if(pVal)
    {
        *field = hash::MurmurHash3::shash(pVal, strlen(pVal), 0);
    }
}

template<class FieldType, class Func>
inline void xml_value_to_field_v2(tinyxml2::XMLElement* pVarE, FieldType* field, Func&& after_func)
{
    xml_value_to_field_v2(pVarE, field);
    after_func(pVarE, field);
}

struct ForEachXMLLambda_V2
{
    tinyxml2::XMLElement* pVarE;
    const char*           field_name;
    std::size_t           field_name_hash;
    template<typename FieldInfo, typename Field>
    bool operator()(FieldInfo&& this_field_info, Field&& this_field) const
    {
        printf("v2 %s test:%s\n", field_name, this_field_info.field_name);
        if(field_name_hash != this_field_info.field_name_hash)
            return false;
        printf("v2 %s vist:%s\n", field_name, this_field_info.field_name);
        xml_value_to_field_v2(pVarE, &this_field);
        return true;
    }

    template<typename FieldInfo, typename Field, typename TagOfFunc>
    bool operator()(FieldInfo&& this_field_info, Field&& this_field, TagOfFunc&& tag) const
    {
        printf("v2 %s test:%s\n", field_name, this_field_info.field_name);
        if(field_name_hash != this_field_info.field_name_hash)
            return false;
        printf("v2 %s vist:%s\n", field_name, this_field_info.field_name);
        xml_value_to_field_v2(pVarE, &this_field, std::forward<TagOfFunc>(tag));
        return true;
    }
};

template<class T>
inline void xmlElement_to_struct_v2(tinyxml2::XMLElement* pE, T& refStruct)
{
    tinyxml2::XMLElement* pVarE = pE->FirstChildElement();
    while(pVarE != NULL)
    {
        const char* pStrName = pVarE->Attribute("name");
        if(pStrName != NULL)
        {
            std::string field_name      = pStrName;
            auto        field_name_hash = hash::MurmurHash3::shash(field_name.c_str(), field_name.size(), 0);
            static_reflection_v2::FindInField(refStruct, field_name_hash, ForEachXMLLambda_V2{pVarE, field_name.c_str(), field_name_hash});
        }

        pVarE = pVarE->NextSiblingElement();
    }
}

void after_process_TestNodeB_c(tinyxml2::XMLElement*, float* val)
{
    *val *= 100;
}

int main()
{

    tinyxml2::XMLDocument doc;
    auto                  errZ_code = doc.Parse(xml_txt.c_str(), xml_txt.size());
    if(errZ_code != tinyxml2::XMLError::XML_SUCCESS)
    {
        return -1;
    }

    auto pRootE = doc.FirstChildElement("Root");
    if(pRootE == NULL)
    {
        return -1;
    }

    auto pNodesE = pRootE->FirstChildElement("Nodes");
    if(pNodesE == NULL)
    {
        return -1;
    }
    std::vector<AllNode> testNodeList;
    auto                 pNodeE = pNodesE->FirstChildElement();
    while(pNodeE)
    {

        std::string struct_name = pNodeE->Name();
        AllNode     testNode;
        xmlElement_to_struct_v2(pNodeE, testNode);
        testNodeList.emplace_back(std::move(testNode));
        pNodeE = pNodeE->NextSiblingElement();
    }

    return 0;
}