﻿#include "xmutisig/xecpoint.h"

#include "xmutisig/xmutisig_types/xcurve.h"
#include "xmutisig/xschnorr.h"
#include "xmutisig/xserialize/xecpoint_serialize.h"

#include <cassert>

using tcash::xmutisig::xpoint_face;
// using tcash::xmutisig::ec_point_ptr_t;

xpoint_face::xpoint_face(const xbn_face & bn_face) {
    m_ec_point = nullptr;
    m_ec_point = xschnorr::instance()->generate_ec_point();
    assert(nullptr != m_ec_point);

#ifdef DEBUG
    assert(!BN_is_zero(bn_face.bn_value()));
    assert((BN_cmp(bn_face.bn_value(), xschnorr::instance()->curve()->bn_order()) == -1));
#endif

    // bn_face is "k" and m_ec_point is "R", here R = k * G
    // or bn_face is "private_key" and m_ec_point is public Key,so  P = pk * G
#if !defined(NDEBUG)
    int32_t ret =
#endif
    EC_POINT_mul(xschnorr::instance()->curve()->ec_group(), m_ec_point, bn_face.bn_value(), nullptr, nullptr, nullptr);
    assert(0 != ret);
}

xpoint_face::xpoint_face(const xpoint_face & point) {
    m_ec_point = nullptr;
    m_ec_point = xschnorr::instance()->generate_ec_point();
    assert(nullptr != m_ec_point);

    /** Copies EC_POINT object
     *  \param  dst  destination EC_POINT object
     *  \param  src  source EC_POINT object
     *  \return 1 on success and 0 if an error occurred
     */
#if !defined(NDEBUG)
    int ret =
#endif
    EC_POINT_copy(m_ec_point, point.ec_point());
    assert(0 != ret);
}

xpoint_face::xpoint_face(const std::string & serialize_str) {
    m_ec_point = nullptr;
    assert(0 != serialize_str.size());
    m_ec_point = xecpointserialize::deserialize(serialize_str);
    assert(nullptr != m_ec_point);
}

xpoint_face & xpoint_face::operator=(const xpoint_face & point) {
    EC_POINT_copy(m_ec_point, point.ec_point());
    return *this;
}

xpoint_face::~xpoint_face() {
    if (m_ec_point != nullptr)
        EC_POINT_free(m_ec_point);
}

EC_POINT * xpoint_face::ec_point() const {
    return m_ec_point;
}

std::string xpoint_face::get_serialize_str() const {
    assert(nullptr != m_ec_point);

    std::string serialize_str;
    xecpointserialize::serialize(serialize_str, m_ec_point);
    assert(0 != serialize_str.size());

    return serialize_str;
}
