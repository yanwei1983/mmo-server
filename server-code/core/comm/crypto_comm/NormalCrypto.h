#ifndef SQLURLCRYPTO_H
#define SQLURLCRYPTO_H

#include <memory>
#include <string>

struct NormalCryptoData;
class NormalCrypto
{
public:
    NormalCrypto(const std::string& key_txt);
    NormalCrypto(NormalCrypto&& rht) noexcept;
    ~NormalCrypto();

    std::string Encode(const std::string& plain);
    std::string Decode(const std::string& cipher);

    static NormalCrypto default_instance();

    std::unique_ptr<NormalCryptoData> m_data;
};

#endif /* SQLURLCRYPTO_H */
