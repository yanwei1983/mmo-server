#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <memory>

#include "BaseType.h"
class CEncryptorImpl;
class CEncryptor
{
public:
    CEncryptor();
    ~CEncryptor();

    void   Init(uint32_t key);
    size_t Encryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len);

private:
    std::unique_ptr<CEncryptorImpl> m_pImpl;
};
#endif /* ENCRYPTOR_H */
