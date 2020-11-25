#include "NormalCrypto.h"

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>
#include <cryptopp/seed.h>

using byte = unsigned char;

struct NormalCryptoData
{
    NormalCryptoData(const std::string& key_txt)
        : key(reinterpret_cast<const byte*>(&key_txt[0]), key_txt.size())
    {
    }
    CryptoPP::SecByteBlock key;

    byte iv[CryptoPP::AES::BLOCKSIZE] = {};
};

NormalCrypto::NormalCrypto(const std::string& key_txt)
    : m_data(std::make_unique<NormalCryptoData>(key_txt))
{
}

NormalCrypto::NormalCrypto(NormalCrypto&& rht) noexcept
    : m_data(std::move(rht.m_data))
{
}

NormalCrypto::~NormalCrypto() {}

std::string NormalCrypto::Encode(const std::string& plain)
{
    using namespace CryptoPP;

    std::string cipher;
    try
    {
        CTR_Mode<AES>::Encryption e;
        e.SetKeyWithIV(m_data->key, m_data->key.size(), m_data->iv);

        // The StreamTransformationFilter adds padding
        //  as required. ECB and CBC Mode must be padded
        //  to the block size of the cipher. CTR does not.
        StringSource ss1(plain,
                         true,
                         new StreamTransformationFilter(e,
                                                        new HexEncoder(new StringSink(cipher))) // StreamTransformationFilter
        );                                                                                      // StringSource
    }
    catch(...)
    {
    }
    return cipher;
}

std::string NormalCrypto::Decode(const std::string& cipher)
{
    using namespace CryptoPP;

    std::string recovered;
    try
    {

        CTR_Mode<AES>::Decryption d;
        d.SetKeyWithIV(m_data->key, m_data->key.size(), m_data->iv);

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource ss3(cipher,
                         true,
                         new HexDecoder(new StreamTransformationFilter(d,
                                                                       new StringSink(recovered)) // StreamTransformationFilter
                                        ));                                                       // StringSource
    }
    catch(...)
    {
    }

    return recovered;
}

NormalCrypto NormalCrypto::default_instance()
{
    std::string  key_txt = "dPj3T2bJRHRVhE8Bc2AjkT7n";
    NormalCrypto result(key_txt);
    return result;
}