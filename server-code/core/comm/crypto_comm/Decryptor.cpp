#include "Decryptor.h"


#include <cryptopp/aes.h>      // for AES
#include <cryptopp/config.h>   // for CryptoPP
#include <cryptopp/filters.h>  // for ArraySink, Redirector, StreamTransform...
#include <cryptopp/modes.h>    // for OFB_Mode<>::Decryption, OFB_Mode
#include <cryptopp/rng.h>      // for LC_RNG

using namespace CryptoPP;
class CDecryptorImpl
{
public:
    CDecryptorImpl() {}
    ~CDecryptorImpl() {}

    void Init(uint32_t key)
    {
        LC_RNG prng(key);
        prng.GenerateBlock(m_key, AES::DEFAULT_KEYLENGTH);
        prng.GenerateBlock(m_iv, AES::BLOCKSIZE);
        m_dec.SetKeyWithIV(m_key, AES::DEFAULT_KEYLENGTH, m_iv);
    }

    size_t Decryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len)
    {
        ArraySink   cs(out_buffer, out_len);
        ArraySource as(in_buffer,
                       in_len,
                       true,
                       new StreamTransformationFilter(m_dec, new Redirector(cs), CryptoPP::BlockPaddingSchemeDef::NO_PADDING));
        return cs.TotalPutLength();
    }

private:
    byte                      m_key[AES::DEFAULT_KEYLENGTH] = {};
    byte                      m_iv[AES::BLOCKSIZE] = {};
    OFB_Mode<AES>::Decryption m_dec;
};

CDecryptor::CDecryptor()
    : m_pImpl{std::make_unique<CDecryptorImpl>()}
{
}

CDecryptor::~CDecryptor() {}

void CDecryptor::Init(uint32_t key)
{
    m_pImpl->Init(key);
}

size_t CDecryptor::Decryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len)
{
    return m_pImpl->Decryptor(in_buffer, in_len, out_buffer, out_len);
}
