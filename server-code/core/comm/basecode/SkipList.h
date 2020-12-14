#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "BaseCode.h"
#include "RandomGet.h"

constexpr int32_t SKIPLIST_MAXLEVEL = 32;
constexpr double  SKIPLIST_P        = 0.25;

class CSkipList
{
public:
    struct CSkipListNode
    {
        uint64_t              m_member   = 0;
        uint64_t              m_score    = 0.0;
        struct CSkipListNode* m_backward = nullptr;
        struct CSkipListLevel
        {
            struct CSkipListNode* m_forward = nullptr;
            uint32_t              m_span    = 0;
        };
        std::vector<CSkipListLevel> m_level;

        CSkipListNode* getNext(bool reverse) const { return reverse ? m_backward : m_level[0].m_forward; }
    };

    struct CSkipListNode* m_header = nullptr;
    struct CSkipListNode* m_tail   = nullptr;
    uint32_t              m_length = 0;
    int32_t               m_level  = 1;

public:
    CSkipList()
    {
        m_header             = CreateNode(SKIPLIST_MAXLEVEL, 0.0, 0);
        m_header->m_backward = NULL;
        m_tail               = NULL;
    }

    ~CSkipList()
    {
        CSkipListNode *node = m_header->m_level[0].m_forward, *next;

        delete m_header;
        while(node)
        {
            next = node->m_level[0].m_forward;
            FreeNode(node);
            node = next;
        }
    }

    void Insert(uint64_t score, uint64_t member)
    {
        std::array<CSkipListNode*, SKIPLIST_MAXLEVEL> update;
        update.fill(nullptr);
        std::array<uint32_t, SKIPLIST_MAXLEVEL> rank;
        rank.fill(0);

        CSkipListNode* x = m_header;
        for(int32_t i = m_level - 1; i >= 0; i--)
        {
            /* store rank that is crossed to reach the insert position */
            rank[i] = i == (m_level - 1) ? 0 : rank[i + 1];
            while(x->m_level[i].m_forward &&
                  (x->m_level[i].m_forward->m_score > score ||
                   (x->m_level[i].m_forward->m_score == score && compareslObj(x->m_level[i].m_forward->m_member, member) > 0)))
            {
                rank[i] += x->m_level[i].m_span;
                x = x->m_level[i].m_forward;
            }
            update[i] = x;
        }
        /* we assume the key is not already inside, since we allow duplicated
         * scores, and the re-insertion of score and redis object should never
         * happen since the caller of slInsert() should test in the hash table
         * if the element is already inside or not. */
        int32_t level = RandomLevel();
        if(level > m_level)
        {
            for(int32_t i = m_level; i < level; i++)
            {
                rank[i]                      = 0;
                update[i]                    = m_header;
                update[i]->m_level[i].m_span = m_length;
            }
            m_level = level;
        }
        x = CreateNode(level, score, member);
        for(int32_t i = 0; i < level; i++)
        {
            x->m_level[i].m_forward         = update[i]->m_level[i].m_forward;
            update[i]->m_level[i].m_forward = x;

            /* update span covered by update[i] as x is inserted here */
            x->m_level[i].m_span         = update[i]->m_level[i].m_span - (rank[0] - rank[i]);
            update[i]->m_level[i].m_span = (rank[0] - rank[i]) + 1;
        }

        /* increment span for untouched levels */
        for(int32_t i = level; i < m_level; i++)
        {
            update[i]->m_level[i].m_span++;
        }

        x->m_backward = (update[0] == m_header) ? NULL : update[0];
        if(x->m_level[0].m_forward)
            x->m_level[0].m_forward->m_backward = x;
        else
            m_tail = x;
        m_length++;
    }

    /* Delete an element with matching score/object from the skiplist. */
    int32_t Delete(uint64_t score, uint64_t member)
    {
        CSkipListNode* update[SKIPLIST_MAXLEVEL];
        CSkipListNode* x = m_header;
        for(int32_t i = m_level - 1; i >= 0; i--)
        {
            while(x->m_level[i].m_forward &&
                  (x->m_level[i].m_forward->m_score > score ||
                   (x->m_level[i].m_forward->m_score == score && compareslObj(x->m_level[i].m_forward->m_member, member) > 0)))
                x = x->m_level[i].m_forward;
            update[i] = x;
        }
        /* We may have multiple elements with the same score, what we need
         * is to find the element with both the right score and object. */
        x = x->m_level[0].m_forward;
        if(x && score == x->m_score && x->m_member == member)
        {
            DeleteNode(x, update);
            FreeNode(x);
            return 1;
        }
        else
        {
            return 0; /* not found */
        }
        return 0; /* not found */
    }

    /* Delete all the elements with rank between start and end from the skiplist.
     * Start and end are inclusive. Note that start and end need to be 1-based */
    uint32_t DeleteByRank(uint32_t start, uint32_t end, const std::function<void(uint64_t member, uint64_t score)>& cb)
    {
        CSkipListNode* update[SKIPLIST_MAXLEVEL];
        uint32_t       traversed = 0, removed = 0;
        CSkipListNode* x = m_header;
        for(int32_t i = m_level - 1; i >= 0; i--)
        {
            while(x->m_level[i].m_forward && (traversed + x->m_level[i].m_span) < start)
            {
                traversed += x->m_level[i].m_span;
                x = x->m_level[i].m_forward;
            }
            update[i] = x;
        }

        traversed++;
        x = x->m_level[0].m_forward;
        while(x && traversed <= end)
        {
            CSkipListNode* next = x->m_level[0].m_forward;
            DeleteNode(x, update);
            cb(x->m_member, x->m_score);
            FreeNode(x);
            removed++;
            traversed++;
            x = next;
        }
        return removed;
    }

    /* Find the rank for an element by both score and key.
     * Returns 0 when the element cannot be found, rank otherwise.
     * Note that the rank is 1-based due to the span of header to the
     * first element. */
    uint32_t GetRank(uint64_t score, uint64_t member) const
    {
        CSkipListNode* x    = m_header;
        uint32_t       rank = 0;
        for(int32_t i = m_level - 1; i >= 0; i--)
        {
            while(x->m_level[i].m_forward &&
                  (x->m_level[i].m_forward->m_score > score ||
                   (x->m_level[i].m_forward->m_score == score && compareslObj(x->m_level[i].m_forward->m_member, member) >= 0)))
            {
                rank += x->m_level[i].m_span;
                x = x->m_level[i].m_forward;
            }

            /* x might be equal to header, so test if obj is non-NULL */
            if(x->m_member && x->m_member == member)
            {
                return rank;
            }
        }
        return 0;
    }

    /* Finds an element by its rank. The rank argument needs to be 1-based. */
    CSkipListNode* GetNodeByRank(uint32_t rank) const
    {
        if(rank == 0 || rank > m_length)
        {
            return NULL;
        }

        CSkipListNode* x         = m_header;
        uint32_t       traversed = 0;
        for(int32_t i = m_level - 1; i >= 0; i--)
        {
            while(x->m_level[i].m_forward && (traversed + x->m_level[i].m_span) <= rank)
            {
                traversed += x->m_level[i].m_span;
                x = x->m_level[i].m_forward;
            }
            if(traversed == rank)
            {
                return x;
            }
        }

        return NULL;
    }

    /* range [min, max], left & right both include */
    /* Returns if there is a part of the zset is in range. */
    int32_t IsInRange(uint64_t min, uint64_t max) const
    {
        CSkipListNode* x;

        /* Test for ranges that will always be empty. */
        if(min > max)
        {
            return 0;
        }
        x = m_tail;
        if(x == NULL || x->m_score < min)
            return 0;

        x = m_header->m_level[0].m_forward;
        if(x == NULL || x->m_score > max)
            return 0;
        return 1;
    }

    /* Find the first node that is contained in the specified range.
     * Returns NULL when no element is contained in the range. */
    CSkipListNode* FirstInRange(uint64_t min, uint64_t max) const
    {
        CSkipListNode* x = m_header;
        /* If everything is out of range, return early. */
        if(!IsInRange(min, max))
            return NULL;
        for(int32_t i = m_level - 1; i >= 0; i--)
        {
            /* Go forward while *OUT* of range. */
            while(x->m_level[i].m_forward && x->m_level[i].m_forward->m_score < min)
                x = x->m_level[i].m_forward;
        }

        /* This is an inner range, so the next node cannot be NULL. */
        x = x->m_level[0].m_forward;
        return x;
    }

    /* Find the last node that is contained in the specified range.
     * Returns NULL when no element is contained in the range. */
    CSkipListNode* LastInRange(uint64_t min, uint64_t max) const
    {
        CSkipListNode* x = m_header;
        /* If everything is out of range, return early. */
        if(!IsInRange(min, max))
            return NULL;

        for(int32_t i = m_level - 1; i >= 0; i--)
        {
            /* Go forward while *IN* range. */
            while(x->m_level[i].m_forward && x->m_level[i].m_forward->m_score <= max)
                x = x->m_level[i].m_forward;
        }

        /* This is an inner range, so this node cannot be NULL. */
        return x;
    }

    void for_each(const std::function<void(uint32_t, uint64_t, uint64_t)>& func) const
    {
        CSkipListNode* x     = m_header;
        uint32_t       nRank = 1;
        while(x->m_level[0].m_forward)
        {
            x = x->m_level[0].m_forward;
            func(nRank, x->m_member, x->m_score);
            nRank++;
        }
    }

private:
    static int32_t compareslObj(uint64_t a, uint64_t b)
    {
        if(a == b)
            return 0;
        if(a < b)
            return -1;
        return 1;
    }

    CSkipListNode* CreateNode(int32_t level, uint64_t score, uint64_t member)
    {
        CSkipListNode* n = new CSkipListNode;
        n->m_score       = score;
        n->m_member      = member;
        n->m_level.resize(level);
        return n;
    }

    void FreeNode(CSkipListNode* node) { delete node; }

    int32_t RandomLevel(void) const
    {
        int32_t level = 1;
        while((random_uint32() & 0xffff) < (SKIPLIST_P * 0xffff))
            level += 1;
        return (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
    }

    /* Internal function used by slDelete, slDeleteByScore */
    void DeleteNode(CSkipListNode* x, CSkipListNode** update)
    {
        for(int32_t i = 0; i < m_level; i++)
        {
            if(update[i]->m_level[i].m_forward == x)
            {
                update[i]->m_level[i].m_span += x->m_level[i].m_span - 1;
                update[i]->m_level[i].m_forward = x->m_level[i].m_forward;
            }
            else
            {
                update[i]->m_level[i].m_span -= 1;
            }
        }
        if(x->m_level[0].m_forward)
        {
            x->m_level[0].m_forward->m_backward = x->m_backward;
        }
        else
        {
            m_tail = x->m_backward;
        }

        while(m_level > 1 && m_header->m_level[m_level - 1].m_forward == NULL)
            m_level--;
        m_length--;
    }
};

#endif /* SKIPLIST_H */
