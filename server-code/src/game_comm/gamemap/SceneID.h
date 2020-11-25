#ifndef SCENEID_H
#define SCENEID_H

#include "BaseCode.h"

export_lua struct SceneIdx
{
    export_lua SceneIdx(uint16_t _idZone, uint16_t _idMap, uint32_t _idxPhase)
        : m_Data{_idZone, _idMap, _idxPhase}
    {
        if(m_Data._idZone == 0 || m_Data._idZone > 10)
        {
            throw std::runtime_error("zoneid overflow");
        }
    }
    export_lua SceneIdx(uint64_t _data64 = 0)
        : data64(_data64)
    {
    }
    export_lua SceneIdx(const SceneIdx& rht)
        : data64(rht.data64)
    {
    }

    struct SceneIdxData
    {
        uint16_t _idZone;
        uint16_t _idMap;
        uint32_t _idxPhase;
    };

    union {
        SceneIdxData m_Data;
        uint64_t     data64;
    };

                    operator uint64_t() const { return data64; }
    export_lua bool operator==(const SceneIdx& rht) const { return data64 == rht.data64; }
    export_lua bool operator<(const SceneIdx& rht) const { return data64 < rht.data64; }

    export_lua int32_t GetPhaseIdx() const { return m_Data._idxPhase; }

    export_lua uint16_t GetZoneID() const { return m_Data._idZone; }
    export_lua uint16_t GetMapID() const { return m_Data._idMap; }
    SceneIdx            GetStaticPhaseSceneIdx() const { return SceneIdx(m_Data._idZone, m_Data._idMap, 0); }
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<>
    struct hash<SceneIdx>
    {
        typedef SceneIdx    argument_type;
        typedef std::size_t result_type;
        result_type         operator()(argument_type const& s) const
        {
            std::hash<uint64_t> hasher;
            return hasher(s.data64);
        }
    };
} // namespace std

template<>
struct fmt::formatter<SceneIdx> : public fmt::formatter<uint64_t>
{
    template<typename FormatContext>
    auto format(const SceneIdx& sceneid, FormatContext& ctx)
    {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "({}:{}:{})", sceneid.GetZoneID(), sceneid.GetMapID(), sceneid.GetPhaseIdx());
    }
};

export_lua enum PhaseType {
    GlobalPhase = 0,
    SelfPhase   = 1,
    TeamPhase   = 2,
    GuildPhase  = 3,
};

export_lua struct TargetSceneID
{
    export_lua TargetSceneID(uint16_t _idZone, uint16_t _idMap, uint16_t _idPhaseTypeID, uint16_t _nPhaseType)
        : m_Data{_idZone, _idMap, _idPhaseTypeID, _nPhaseType}
    {
        if(m_Data._idZone == 0 || m_Data._idZone > 10)
        {
            throw std::runtime_error("zoneid overflow");
        }
    }
    export_lua TargetSceneID(uint64_t _data64 = 0)
        : data64(_data64)
    {
    }
    export_lua TargetSceneID(const TargetSceneID& rht)
        : data64(rht.data64)
    {
    }

    struct TargetSceneIDData
    {
        uint16_t _idZone;
        uint16_t _idMap;
        uint16_t _idPhaseTypeID;
        uint16_t _nPhaseType;
    };

    union {
        TargetSceneIDData m_Data;
        uint64_t          data64;
    };

    operator uint64_t() const { return data64; }

    export_lua bool operator==(const TargetSceneID& rht) const { return data64 == rht.data64; }
    export_lua bool operator<(const TargetSceneID& rht) const { return data64 < rht.data64; }

    export_lua uint16_t GetPhaseTypeID() const { return m_Data._idPhaseTypeID; }
    export_lua uint16_t GetPhaseType() const { return m_Data._nPhaseType; }

    export_lua bool IsSelfPhaseID() const { return m_Data._nPhaseType == TeamPhase; }
    export_lua bool IsTeamPhaseID() const { return m_Data._nPhaseType == TeamPhase; }
    export_lua bool IsGuildPhaseID() const { return m_Data._nPhaseType == GuildPhase; }

    export_lua uint16_t GetZoneID() const { return m_Data._idZone; }
    export_lua uint16_t GetMapID() const { return m_Data._idMap; }
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<>
    struct hash<TargetSceneID>
    {
        typedef TargetSceneID argument_type;
        typedef std::size_t   result_type;
        result_type           operator()(argument_type const& s) const
        {
            std::hash<uint64_t> hasher;
            return hasher(s.data64);
        }
    };
} // namespace std

template<>
struct fmt::formatter<TargetSceneID> : public fmt::formatter<uint64_t>
{
    template<typename FormatContext>
    auto format(const TargetSceneID& sceneid, FormatContext& ctx)
    {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "({}:{}:{}:{})", sceneid.GetZoneID(), sceneid.GetMapID(), sceneid.GetPhaseTypeID(), sceneid.GetPhaseType());
    }
};

#endif /* SCENEID_H */
