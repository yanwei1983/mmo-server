#include "Encryptor.h"


#include <cryptopp/aes.h>      // for AES
#include <cryptopp/config.h>   // for CryptoPP
#include <cryptopp/filters.h>  // for ArraySink, Redirector, StreamTransform...
#include <cryptopp/modes.h>    // for OFB_Mode<>::Encryption, OFB_Mode
#include <cryptopp/rng.h>      // for LC_RNG

using namespace CryptoPP;
class CEncryptorImpl
{
public:
    CEncryptorImpl() {}
    ~CEncryptorImpl() {}

    void Init(uint32_t key)
    {
        LC_RNG prng(key);
        prng.GenerateBlock(m_key, AES::DEFAULT_KEYLENGTH);
        prng.GenerateBlock(m_iv, AES::BLOCKSIZE);
        m_enc.SetKeyWithIV(m_key, AES::DEFAULT_KEYLENGTH, m_iv);
    }

    size_t Encryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len)
    {
        // m_encryptor.ProcessData(out_buffer, in_buffer, in_len);
        ArraySink   cs(out_buffer, out_len);
        ArraySource as(in_buffer,
                       in_len,
                       true,
                       new StreamTransformationFilter(m_enc, new Redirector(cs), CryptoPP::BlockPaddingSchemeDef::NO_PADDING));
        return cs.TotalPutLength();
    }

private:
    byte                      m_key[AES::DEFAULT_KEYLENGTH];
    byte                      m_iv[AES::BLOCKSIZE] = {};
    OFB_Mode<AES>::Encryption m_enc;
};

CEncryptor::CEncryptor()
    : m_pImpl{std::make_unique<CEncryptorImpl>()}
{
}

CEncryptor::~CEncryptor() {}

void CEncryptor::Init(uint32_t key)
{
    m_pImpl->Init(key);
}

size_t CEncryptor::Encryptor(byte* in_buffer, size_t in_len, byte* out_buffer, size_t out_len)
{
    return m_pImpl->Encryptor(in_buffer, in_len, out_buffer, out_len);
}
