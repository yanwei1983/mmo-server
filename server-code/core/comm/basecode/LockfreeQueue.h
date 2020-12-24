#ifndef LockfreeQueue_h__
#define LockfreeQueue_h__

#include <atomic>
#include <memory>
#include <optional>

// Lock-free multiple-producer single-consumer
template<class T>
class MPSCQueue
{
public:
    MPSCQueue();
    ~MPSCQueue();
    size_t size() const { return m_nCount; }
    bool   empty() const { return size() == 0; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class _T>
    void put(_T&& item)
    {
        Node* new_node = new Node(std::forward<_T>(item));
        Node* old      = m_pHead.load(std::memory_order_relaxed);
        do
        {
            new_node->next = old;
        } while(!m_pHead.compare_exchange_weak(old, new_node, std::memory_order_release));
        m_nCount++;
    }
    template<class _T>
    inline void push(_T&& item)
    {
        put(std::forward<_T>(item));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class Func>
    bool get_by_func(Func&& func)
    {
        if(m_pPollList)
        {
            Node* result = m_pPollList;
            m_pPollList  = m_pPollList->next;
            func(std::move(result->value));
            m_nCount--;
            delete result;
            return true;
        }
        else if(m_nCount == 0)
        {
            return false;
        }
        else
        {
            Node* head = m_pHead.exchange(nullptr, std::memory_order_acquire);
            if(!head)
            {
                return false;
            }
            // Reverse list
            do
            {
                Node* temp  = head;
                head        = head->next;
                temp->next  = m_pPollList;
                m_pPollList = temp;
            } while(head != nullptr);

            head        = m_pPollList;
            func(std::move(head->value));
            m_pPollList = head->next;
            m_nCount--;
            delete head;
            return true;
        }
    }
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool get(T& item)
    {
        if(m_pPollList)
        {
            Node* result = m_pPollList;
            m_pPollList  = m_pPollList->next;
            item         = std::move(result->value);
            m_nCount--;
            delete result;
            return true;
        }
        else if(m_nCount == 0)
        {
            return false;
        }
        else
        {
            Node* head = m_pHead.exchange(nullptr, std::memory_order_acquire);
            if(!head)
            {
                return false;
            }
            // Reverse list
            do
            {
                Node* temp  = head;
                head        = head->next;
                temp->next  = m_pPollList;
                m_pPollList = temp;
            } while(head != nullptr);

            head        = m_pPollList;
            item        = std::move(head->value);
            m_pPollList = head->next;
            m_nCount--;
            delete head;
            return true;
        }
    }
    inline bool pop(T& item) { return get(item); }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::optional<T> get()
    {
        if(m_pPollList)
        {
            Node* node = m_pPollList;
            m_pPollList  = m_pPollList->next;
            std::optional<T> result {std::move(result->value)};
            m_nCount--;
            delete node;
            return result;
        }
        else if(m_nCount == 0)
        {
            return {};
        }
        else
        {
            Node* head = m_pHead.exchange(nullptr, std::memory_order_acquire);
            if(!head)
            {
                return {};
            }
            // Reverse list
            do
            {
                Node* temp  = head;
                head        = head->next;
                temp->next  = m_pPollList;
                m_pPollList = temp;
            } while(head != nullptr);

            head        = m_pPollList;
            std::optional<T> result {std::move(head->value)};
            m_pPollList = head->next;
            m_nCount--;
            delete head;
            return result;
        }
    }
    inline std::optional<T> pop() { return get(); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void drop_all()
    {
        auto do_nothing = [](T&&){};
        while(get_by_func(do_nothing) == true)
        {
            
        }
    }
    
private:
    struct Node
    {
        template<class _T>
        Node(_T&& val)
            : value(std::forward<_T>(val))
            , next(nullptr)
        {
        }

        Node* next;
        T     value;
    };

    std::atomic<Node*>  m_pHead     = nullptr;
    std::atomic<size_t> m_nCount    = 0;
    Node*               m_pPollList = nullptr; // for consumer only
};

template<class T>
MPSCQueue<T>::MPSCQueue()
{
}

template<class T>
MPSCQueue<T>::~MPSCQueue()
{
    while(true)
    {
        while(m_pPollList)
        {
            Node* result = m_pPollList;
            m_pPollList  = m_pPollList->next;
            delete result;
        }
        Node* head = m_pHead.exchange(nullptr, std::memory_order_acquire);
        if(!head)
        {
            break;
        }
        m_pPollList = head->next;
        delete head;
    }
    m_nCount = 0;
}

#endif // LockfreeQueue_h__