#pragma once

#include "xbase/xns_macro.h"
#include "xmutisig/xbignum.h"

#include <string>

NS_BEG2(tcash, xmutisig)

// present a point at curve
class xpoint_face {
public:
    xpoint_face() = delete;

    xpoint_face(const xbn_face & prikey);

    xpoint_face(const xpoint_face & pubkey);

    xpoint_face(const std::string & serialize_str);

    xpoint_face & operator=(const xpoint_face &);

    ~xpoint_face();

public:
    std::string get_serialize_str() const;

    EC_POINT * ec_point() const;

protected:
    EC_POINT * m_ec_point{nullptr};
};

NS_END2
