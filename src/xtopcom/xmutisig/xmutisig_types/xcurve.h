﻿#include "xbase/xns_macro.h"

#include "openssl/bn.h"
#include "openssl/ec.h"

NS_BEG2(tcash, xmutisig)

class xcurve {
public:
    xcurve();

    ~xcurve();

public:
    EC_GROUP * ec_group() const;

    BIGNUM * bn_order() const;

private:
    /*
     * Ellipse Curve Cryptography
     */
    EC_GROUP * m_ec_group{nullptr};

    /*
     * order of ec group
     */
    BIGNUM * m_bn_order{nullptr};
};

NS_END2
