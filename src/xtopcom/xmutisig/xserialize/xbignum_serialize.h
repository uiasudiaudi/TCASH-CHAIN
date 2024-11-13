#pragma once

#include "xbase/xns_macro.h"

#include "openssl/bn.h"

#include <string>

NS_BEG2(tcash, xmutisig)

class xbignumserialize {
public:
    /*
     * deserialize to bignum from src
     * success: not nullptr, failed nullptr
     */
    static BIGNUM * deserialize(const std::string & src);

    /*
     * serialize bignum to str
     * 0: success, other failed
     */
    static uint32_t serialize(std::string & bn_str, BIGNUM * bn);
};

NS_END2
