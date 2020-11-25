#include "Decryptor.h"
#include "Encryptor.h"
#include "Misc.h"
int main()
{
    CEncryptor enc;
    CDecryptor dec;
    uint32_t   seed = time(0);
    enc.Init(seed);
    dec.Init(seed);

    char plain[] = "Test gffa";
    printf("[%lu]:", sizeOfArray(plain));
    for(unsigned char ch: plain)
    {
        printf("%X", ch);
    }
    printf("\n");

    enc.Encryptor((byte*)&plain[0], sizeOfArray(plain) - 1, (byte*)&plain[0], sizeOfArray(plain) - 1);
    printf("[%lu]:", sizeOfArray(plain));
    for(unsigned char ch: plain)
    {
        printf("%X", ch);
    }
    printf("\n");

    dec.Decryptor((byte*)&plain[0], sizeOfArray(plain) - 1, (byte*)&plain[0], sizeOfArray(plain) - 1);
    printf("[%lu]:", sizeOfArray(plain));
    for(unsigned char ch: plain)
    {
        printf("%X", ch);
    }
    printf("\n");
}