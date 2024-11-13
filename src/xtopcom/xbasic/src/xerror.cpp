// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xbasic/xerror/xerror.h"

#include "xbase/xlog.h"

#include <cinttypes>
#include <string>

std::error_category const & base_category() {
    static class : public std::error_category {
    public:
        char const * name() const noexcept override {
            return "base";
        }

        std::string message(int errc) const override {
            auto const ec = static_cast<enum_xerror_code>(errc);
            switch (ec) {
            case enum_xerror_code_bad_packet:
                return "bad packet";

            default:
                return "unknown error";
            }
        }
    } base_cagegory;
    return base_cagegory;
}

std::error_code make_error_code(enum_xerror_code ec) noexcept {
    return std::error_code{ static_cast<int>(ec), base_category() };
}

std::error_condition make_error_condition(enum_xerror_code ec) noexcept {
    return std::error_condition{ static_cast<int>(ec), base_category() };
}

NS_BEG2(tcash, error)

xtcash_tcash_error::xtcash_tcash_error(std::error_code const ec) : base_t{ec.message()}, m_ec{ec} {
}

xtcash_tcash_error::xtcash_tcash_error(std::error_code const ec, char const * extra_what) : base_t{ec.message() + ": " + extra_what}, m_ec{ec} {
}

xtcash_tcash_error::xtcash_tcash_error(std::error_code const ec, std::string const & extra_what) : base_t{ec.message() + ": " + extra_what}, m_ec{ec} {
}

xtcash_tcash_error::xtcash_tcash_error(int const ev, std::error_category const & ecat) : base_t{std::error_code{ev, ecat}.message()}, m_ec{ev, ecat} {
}

xtcash_tcash_error::xtcash_tcash_error(int const ev, std::error_category const & ecat, char const * extra_what)
  : base_t{std::error_code{ev, ecat}.message() + ": " + extra_what}, m_ec{ev, ecat} {
}

xtcash_tcash_error::xtcash_tcash_error(int const ev, std::error_category const & ecat, std::string const & extra_what)
  : base_t{std::error_code{ev, ecat}.message() + ": " + extra_what}, m_ec{ev, ecat} {
}

std::error_code const & xtcash_tcash_error::code() const noexcept {
    return m_ec;
}

std::error_category const & xtcash_tcash_error::category() const noexcept {
    return m_ec.category();
}

template <typename ExceptionT>
void throw_exception(ExceptionT const & eh) {
    throw eh;
}

static void do_throw_error(std::error_code const & ec) {
    xtcash_error_t const eh{ec};
    xwarn("throw_error. category %s, errc %" PRIi32 " msg %s", eh.category().name(), eh.code().value(), eh.what());
    throw_exception(eh);
}

static void do_throw_error(std::error_code const & ec, char const * extra_what) {
    xtcash_error_t const eh{ec, extra_what};
    xwarn("throw_error. category %s, errc %" PRIi32 " msg %s", eh.category().name(), eh.code().value(), eh.what());
    throw_exception(eh);
}

static void do_throw_error(std::error_code const & ec, std::string const & extra_what) {
    xtcash_error_t const eh{ec, extra_what};
    xwarn("throw_error. category %s, errc %" PRIi32 " msg %s", eh.category().name(), eh.code().value(), eh.what());
    throw_exception(eh);
}

void throw_error(std::error_code const & ec) {
    if (ec) {
        do_throw_error(ec);
    }
}

void throw_error(std::error_code const ec, char const * extra_what) {
    if (ec) {
        do_throw_error(ec, extra_what);
    }
}

void throw_error(std::error_code const ec, std::string const & extra_what) {
    if (ec) {
        do_throw_error(ec, extra_what);
    }
}

static char const * errc_to_message(int const errc) noexcept {
    auto const ec = static_cast<error::xbasic_errc_t>(errc);
    switch (ec) {
    case xbasic_errc_t::successful:
        return "successful";

    case xbasic_errc_t::serialization_error:
        return "serialization error";

    case xbasic_errc_t::deserialization_error:
        return "deserialization error";

    case xbasic_errc_t::invalid_char_data:
        return "invalid_char error";

    case xenum_basic_errc::invalid_fixed_bytes_size:
        return "invalid_fixed_bytes_size error";

    case xenum_basic_errc::invalid_hex_string:
        return "invalid_hex_string error";

    case xenum_basic_errc::invalid_binary_string:
        return "invalid_binary_string error";

    case xenum_basic_errc::invalid_format:
        return "invalid_format error";

    default:
        return "unknown error";
    }
}

std::error_code make_error_code(xbasic_errc_t errc) noexcept {
    return std::error_code{static_cast<int>(errc), basic_category()};
}

std::error_condition make_error_condition(xbasic_errc_t errc) noexcept {
    return std::error_condition{static_cast<int>(errc), basic_category()};
}

std::error_category const & basic_category() {
    static class : public std::error_category {
    public:
        char const * name() const noexcept override {
            return "basic";
        }

        std::string message(int const errc) const override {
            return errc_to_message(errc);
        }
    } category;

    return category;
}

NS_END2

NS_BEG1(std)

#if !defined(XCXX14)

size_t hash<tcash::error::xbasic_errc_t>::operator()(tcash::error::xbasic_errc_t errc) const noexcept {
    return static_cast<size_t>(static_cast<std::underlying_type<tcash::error::xbasic_errc_t>::type>(errc));
}

#endif

NS_END1
