#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

#include "CheckUtil.h"


template<class Type>
struct CreateNewImpl
{
public:
    template<typename... Args>                                  
    static inline Type* CreateNew(Args&&... args)               
    {                                                           
        Type* newT = nullptr;                                   
        __ENTER_FUNCTION                                        
        {                                                       
            newT = new Type();                                  
            if(newT && newT->Init(std::forward<Args>(args)...)) 
            {                                                   
                return newT;                                    
            }                                                   
        }                                                       
        __LEAVE_FUNCTION                                        
        SAFE_DELETE(newT);                                     
        return nullptr;                                         
    }

    template<typename... Args>                                  
    static inline Type* CreateNewRelease(Args&&... args)               
    {                                                           
        Type* newT = nullptr;                                   
        __ENTER_FUNCTION                                        
        {                                                       
            newT = new Type();                                  
            if(newT && newT->Init(std::forward<Args>(args)...)) 
            {                                                   
                return newT;                                    
            }                                                   
        }                                                       
        __LEAVE_FUNCTION                                        
        SAFE_RELEASE(newT);                                     
        return nullptr;                                         
    }
};

template<typename Type, typename... Args>                                              
static inline Type* CreateNew(Args&&... args)                           
{                                                                       
    return CreateNewImpl<Type>::CreateNew(std::forward<Args>(args)...); 
}

template<typename Type, typename... Args>                                                     
static inline Type* CreateNewRelease(Args&&... args)                                  
{                                                                              
    return CreateNewImpl<Type>::CreateNewRelease(std::forward<Args>(args)...); 
}

#define CreateNewImpl(Type) friend struct CreateNewImpl<Type>;

#define CreateNewRealeaseImpl(Type) friend struct CreateNewImpl<Type>;

template<class Type>
class NoncopyableT
{
protected:
    NoncopyableT() {}
    virtual ~NoncopyableT() {}

public:
    NoncopyableT(const NoncopyableT&) = delete;
    const NoncopyableT& operator=(const NoncopyableT&) = delete;
};

class Noncopyable
{
protected:
    Noncopyable() {}
    virtual ~Noncopyable() {}

public:
    Noncopyable(const Noncopyable&) = delete;
    const Noncopyable& operator=(const Noncopyable&) = delete;
};

#endif /* NONCOPYABLE_H */
