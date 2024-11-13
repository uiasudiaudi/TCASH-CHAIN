﻿#pragma once

#include "xbase/xns_macro.h"
#include "xmutisig/xssl_fwd.h"

#include <string>

NS_BEG2(tcash, xmutisig)

// random k or private_key
class xbn_face {
public:
    xbn_face();
    xbn_face(BIGNUM * bn);
    xbn_face(const xbn_face &);
    xbn_face(const std::string & serialize_str);

    xbn_face & operator=(const xbn_face &);

    ~xbn_face();

public:
    std::string get_serialize_str() const;

    BIGNUM * bn_value() const;

protected:
    BIGNUM * m_private_bn{nullptr};
};

NS_END2
