// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpedantic"
#elif defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
#    pragma warning(push, 0)
#endif

#include "xbase/xcontext.h"
#include "xbase/xdata.h"
#include "xbase/xmem.h"

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include "xbase/xns_macro.h"
#include "xbasic/xbyte_buffer.h"

#include <cstdint>
#include <string>
#include <system_error>

NS_BEG1(tcash)

/**
 * @brief Prototype for serialization.
 *
 * @tparam BasedOnT Serialization based on type.
 */
template <typename BasedOnT>
class xtcash_serializable_based_on;

/**
 * @brief Specialization for serialization based on base::xdataunit_t
 *
 */
template <>
class xtcash_serializable_based_on<base::xdataunit_t> : public base::xdataunit_t {
protected:
    xtcash_serializable_based_on(xtcash_serializable_based_on const &)             = default;
    xtcash_serializable_based_on & operator=(xtcash_serializable_based_on const &) = default;
    xtcash_serializable_based_on(xtcash_serializable_based_on &&)                  = default;
    xtcash_serializable_based_on & operator=(xtcash_serializable_based_on &&)      = default;
    virtual ~xtcash_serializable_based_on()                                      = default;

    xtcash_serializable_based_on() : base::xdataunit_t(base::xdataunit_t::enum_xdata_type_unknow) {
    }
};

/**
 * @brief Specialization for serialization based on nothing. Manually implement the serialization process.
 *
 */
template <>
class xtcash_serializable_based_on<void> {
public:
    xtcash_serializable_based_on()                                               = default;
    xtcash_serializable_based_on(xtcash_serializable_based_on const &)             = default;
    xtcash_serializable_based_on & operator=(xtcash_serializable_based_on const &) = default;
    xtcash_serializable_based_on(xtcash_serializable_based_on &&)                  = default;
    xtcash_serializable_based_on & operator=(xtcash_serializable_based_on &&)      = default;
    virtual ~xtcash_serializable_based_on()                                      = default;

    /**
     * @brief Serialize the object into the stream.  The serialization operation
     *        will serialize the object into a single internal stream and then
     *        serialize this internal stream into the 'stream' parameter object.
     */
    virtual std::int32_t serialize_to(base::xstream_t & stream) const;

    /// @brief Serialize the object into the stream.
    /// @param stream 
    /// @param ec 
    /// @return 
    std::int32_t serialize_to(base::xstream_t & stream, std::error_code & ec) const;

    /**
     * @brief Deserialize the object from the stream.  The deserialization operation
     *        will first read out an stream object form the 'stream' parameter and
     *        then deserialze based on the internal stream object.
     */
    virtual std::int32_t serialize_from(base::xstream_t & stream);

    virtual std::int32_t serialize_from_short(base::xstream_t & stream);

    std::int32_t serialize_from(base::xstream_t & stream, std::error_code & ec);

    virtual std::int32_t serialize_to(base::xbuffer_t & stream) const;

    std::int32_t serialize_to(base::xbuffer_t & stream, std::error_code & ec) const;

    virtual std::int32_t serialize_from(base::xbuffer_t & stream);

    std::int32_t serialize_from(base::xbuffer_t & stream, std::error_code & ec);

    template <typename StreamT>
    xbyte_buffer_t serialize_based_on() const;

    template <typename StreamT>
    xbyte_buffer_t serialize_based_on(std::error_code & ec) const;

    template <typename StreamT>
    void deserialize_based_on(xbyte_buffer_t bytes);

    template <typename StreamT>
    void deserialize_based_on(xbyte_buffer_t bytes, std::error_code & ec);

protected:
    /**
     * @brief Serialize the object into the steam directly.
     */
    virtual std::int32_t do_write(base::xstream_t & stream) const = 0;

    std::int32_t do_write(base::xstream_t & stream, std::error_code & ec) const;

    /**
     * @brief Deserialize the object from the stream directly.
     */
    virtual std::int32_t do_read(base::xstream_t & stream) = 0;

    std::int32_t do_read(base::xstream_t & stream, std::error_code & ec);
};

template <typename BasedOnT>
using xserializable_based_on = xtcash_serializable_based_on<BasedOnT>;

NS_END1

NS_BEG1(tcash)

template <>
xbyte_buffer_t xtcash_serializable_based_on<void>::serialize_based_on<base::xstream_t>() const;

template <>
xbyte_buffer_t xtcash_serializable_based_on<void>::serialize_based_on<base::xstream_t>(std::error_code & ec) const;

template <>
void xtcash_serializable_based_on<void>::deserialize_based_on<base::xstream_t>(xbyte_buffer_t bytes);

template <>
void xtcash_serializable_based_on<void>::deserialize_based_on<base::xstream_t>(xbyte_buffer_t bytes, std::error_code & ec);

NS_END1
