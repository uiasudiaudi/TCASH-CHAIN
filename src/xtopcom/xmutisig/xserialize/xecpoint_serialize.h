#pragma once

#include "xbase/xns_macro.h"

#include "openssl/ec.h"

#include <string>

NS_BEG2(tcash, xmutisig)

class xecpointserialize {
public:
    /*
     * deserialize to ec_point_ptr_t from src
     * success: not nullptr, failed nullptr
     */

    static EC_POINT * deserialize(const std::string & src);

    /*
     * serialize ec_point_ptr_t to str
     * 0: success, other failed
     */
    static uint32_t serialize(std::string & bn_str, EC_POINT * ec_point);
};

NS_END2
