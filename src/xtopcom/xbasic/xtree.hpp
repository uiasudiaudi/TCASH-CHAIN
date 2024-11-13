// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <cassert>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

NS_BEG1(tcash)

/**
 * @brief Generic binary tree node. It holds a value with @relates ValueT
 *
 * @tparam ValueT Tree node value type.
 */
template <typename ValueT>
class xtcash_tree_node {
public:
    using value_type = ValueT;

private:
    ValueT m_value{};
    std::shared_ptr<xtcash_tree_node> m_left{};
    std::shared_ptr<xtcash_tree_node> m_right{};
    std::weak_ptr<xtcash_tree_node> m_parent{};

public:
    xtcash_tree_node()                                   = default;
    xtcash_tree_node(xtcash_tree_node const &)             = default;
    xtcash_tree_node & operator=(xtcash_tree_node const &) = default;
    xtcash_tree_node(xtcash_tree_node &&)                  = default;
    xtcash_tree_node & operator=(xtcash_tree_node &&)      = default;
    virtual ~xtcash_tree_node()                          = default;

    explicit
    xtcash_tree_node(ValueT value) noexcept(std::is_nothrow_move_constructible<ValueT>::value)
        : m_value{ std::move(value) } {
    }

    xtcash_tree_node(std::shared_ptr<xtcash_tree_node<ValueT>> const & parent, ValueT value) noexcept(std::is_nothrow_move_constructible<ValueT>::value)
        : m_value{ std::move(value) }, m_parent{ parent } {
    }

    xtcash_tree_node(std::shared_ptr<xtcash_tree_node<ValueT>> left,
                   std::shared_ptr<xtcash_tree_node<ValueT>> right,
                   ValueT value) noexcept(std::is_nothrow_move_constructible<ValueT>::value)
        : m_value{ std::move(value) }, m_left{ std::move(left) }, m_right{ std::move(right) } {
    }

    xtcash_tree_node(std::shared_ptr<xtcash_tree_node<ValueT>> const & parent,
                   std::shared_ptr<xtcash_tree_node<ValueT>> left,
                   std::shared_ptr<xtcash_tree_node<ValueT>> right,
                   ValueT value) noexcept(std::is_nothrow_move_constructible<ValueT>::value)
        : m_value{ std::move(value) }, m_left{ std::move(left) }
        , m_right{ std::move(right) }, m_parent{ parent } {
    }

    bool
    is_leaf() const noexcept {
        return m_left == nullptr && m_right == nullptr;
    }

    ValueT const &
    value() const noexcept {
        return m_value;
    }

    std::shared_ptr<xtcash_tree_node> const &
    left() const noexcept {
        return m_left;
    }

    std::shared_ptr<xtcash_tree_node> const &
    right() const noexcept {
        return m_right;
    }

    std::shared_ptr<xtcash_tree_node>
    parent() const noexcept {
        return m_parent.lock();
    }

    void
    parent(std::shared_ptr<xtcash_tree_node> const & p) noexcept {
        m_parent = p;
    }
};
template <typename ValueT>
using xtree_node_t = xtcash_tree_node<ValueT>;

/**
 * @brief Generic tree type. Must be used as a base or be derived by @relates DerivedTreeT
 *
 * @tparam NodeT        Defines tree node type.
 * @tparam DerivedTreeT Defines the derived tree type.
 */
template <typename NodeT, typename DerivedTreeT>
class xtcash_basic_tree {
public:
    using node_type = NodeT;
    using value_type = typename node_type::value_type;

protected:
    std::shared_ptr<node_type> m_root;

public:
    xtcash_basic_tree()                                    = default;
    xtcash_basic_tree(xtcash_basic_tree const &)             = delete;
    xtcash_basic_tree & operator=(xtcash_basic_tree const &) = delete;
    xtcash_basic_tree(xtcash_basic_tree &&)                  = default;
    xtcash_basic_tree & operator=(xtcash_basic_tree &&)      = default;
    virtual ~xtcash_basic_tree()                           = default;

    xtcash_basic_tree(std::shared_ptr<node_type> root) noexcept : m_root{ std::move(root) } {
    }

    DerivedTreeT
    left_tree() const noexcept {
        assert(m_root);
        return DerivedTreeT{ m_root->left() };
    }

    DerivedTreeT
    right_tree() const noexcept {
        assert(m_root);
        return DerivedTreeT{ m_root->right() };
    }

    std::shared_ptr<node_type> const &
    root() const noexcept {
        assert(m_root);
        return m_root;
    }

    bool
    empty() const noexcept {
        return m_root == nullptr;
    }
};

template <typename TreeNodeT, typename DerivedTreeT>
using xbasic_tree_t = xtcash_basic_tree<TreeNodeT, DerivedTreeT>;

NS_END1
