#ifndef SINGLETON_H
#define SINGLETON_H

template<class T>
class SingletonNoRelease
{
protected:
    SingletonNoRelease() {}

public:
    virtual ~SingletonNoRelease() {}

    static T* getPtr()
    {
        static T s_ptr;
        return &s_ptr;
    }
};

template<class T>
class SingletonNeedCreate
{
protected:
    SingletonNeedCreate() {}

public:
    virtual ~SingletonNeedCreate() {}

    static T* Instance() { return m_ptr; }

    static void CreateInstance()
    {
        if(m_ptr == nullptr)
        {
            m_ptr = new T();
        }
    }

    static void Release()
    {
        if(m_ptr)
        {
            T* p = nullptr;
            if(m_ptr.exchange(p) == true)
            {
                delete p;
            }
        }
    }

private:
    static std::atomic<T*> m_ptr = nullptr;
};

#endif /* SINGLETON_H */
