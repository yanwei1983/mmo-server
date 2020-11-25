#ifndef DECRYPTOR_H
#define DECRYPTOR_H

#include <memory>

#include "BaseType.h"
class CDecryptorImpl;
class CDecryptor
{
public:
    CDecryptor();
    ~CDecryptor();

    void   Init(uint32_t key);
    size_t Decryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len);

private:
    std::unique_ptr<CDecryptorImpl> m_pImpl;
};
#endif /* DECRYPTOR_H */
