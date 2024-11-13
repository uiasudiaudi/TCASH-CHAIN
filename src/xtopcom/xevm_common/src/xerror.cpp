// Copyright (c) 2017-present Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xevm_common/xerror/xerror.h"

#include <cassert>
#include <string>

NS_BEG3(tcash, evm_common, error)

static char const * errc_to_string(int code) {
    auto const ec = static_cast<xerrc_t>(code);

    switch (ec) {
    case xerrc_t::ok:
        return "ok";

    case xerrc_t::not_enough_data:
        return "not enough data";

    case xerrc_t::abi_data_length_error:
        return "invalid abi data length";

    case xerrc_t::abi_decode_outofrange:
        return "abi decode out of data range";

    case xerrc_t::abi_data_value_error:
        return "abi decode value error";

    case xerrc_t::trie_db_missing_node_error:
        return "trie db missing node error";

    case xerrc_t::trie_db_not_provided:
        return "trie db not provided";

    case xerrc_t::trie_db_not_found:
        return "trie db not found";

    case xerrc_t::trie_db_put_error:
        return "trie db put error";

    case xerrc_t::trie_db_delete_error:
        return "trie db delete error";

    case xerrc_t::rlp_canonint:
        return "rlp: non-canonical integer format";

    case xerrc_t::rlp_canonsize:
        return "rlp: non-canonical size information";

    case xerrc_t::rlp_uint_overflow:
        return "rlp: uint overflow";

    case xerrc_t::rlp_oversized:
        return "rlp: oversized data";

    case xerrc_t::rlp_value_too_large:
        return "rlp: value size exceeds available input length";

    case xerrc_t::rlp_expected_string:
        return "rlp: expected String or Byte";

    case xerrc_t::rlp_expected_list:
        return "rlp: expected List";

    case xerrc_t::trie_proof_missing:
        return "rlp: trie proof missing";

    case xerrc_t::trie_node_unexpected:
        return "trie node unexcepted";

    case xerrc_t::trie_sync_not_requested:
        return "trie sync not requested";

    case xerrc_t::trie_sync_already_processed:
        return "trie sync already processed";

    case xerrc_t::trie_prune_data_duplicated:
        return "trie purne data key duplicated";

    case xerrc_t::trie_prune_data_not_found:
        return "trie prune data not found";

    case xerrc_t::rlp_bytes_invalid:
        return "rlp bytes invalid";

    case xenum_errc::rlp_list_size_not_match:
        return "rlp list size not match";

    case xenum_errc::invalid_public_key_size:
        return "invalid public key size";

    default:  // NOLINT(clang-diagnostic-covered-switch-default)
        assert(false);
        return "unknown evm common error";
    }
}

std::error_code make_error_code(xerrc_t const errc) noexcept {
    return std::error_code{static_cast<int>(errc), evm_common_category()};
}

std::error_condition make_error_condition(xerrc_t const errc) noexcept {
    return std::error_condition{static_cast<int>(errc), evm_common_category()};
}

std::error_category const & evm_common_category() {
    static class xtcash_evm_common_category final : public std::error_category {
    public:
        xtcash_evm_common_category() = default;
        xtcash_evm_common_category(xtcash_evm_common_category const &) = delete;
        xtcash_evm_common_category & operator=(xtcash_evm_common_category const &) = delete;
        xtcash_evm_common_category(xtcash_evm_common_category &&) = delete;
        xtcash_evm_common_category & operator=(xtcash_evm_common_category &&) = delete;
        ~xtcash_evm_common_category() override = default;

        char const * name() const noexcept override {
            return "evm_common";
        }

        std::string message(int const errc) const override {
            return errc_to_string(errc);
        }
    } category{};

    return category;
}

NS_END3
