// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xns_macro.h"

#include <atomic>

NS_BEG1(tcash)

/**
 * @brief Defines basic interface for a runnable object.
 *
 * @tparam T Object type that enables the runnable trait.
 */
template <typename T>
class xtcash_runnable {
public:
    xtcash_runnable() = default;
    xtcash_runnable(xtcash_runnable const &) = delete;
    xtcash_runnable & operator=(xtcash_runnable const &) = delete;
    xtcash_runnable(xtcash_runnable &&) = default;
    xtcash_runnable & operator=(xtcash_runnable &&) = default;
    virtual ~xtcash_runnable() = default;

    virtual void start() = 0;

    virtual void stcash() = 0;

    virtual bool running() const noexcept = 0;

    virtual void running(bool value) noexcept = 0;
};

template <typename T>
using xrunnable_t = xtcash_runnable<T>;

/**
 * @brief Implement runnable interface by enabling atomic running status change.
 *
 * @tparam T Object type that enables the runnable trait.
 */
template <typename T>
class xtcash_basic_runnable : public xrunnable_t<T> {
    std::atomic<bool> m_running{false};

public:
    xtcash_basic_runnable(xtcash_basic_runnable const &) = default;
    xtcash_basic_runnable & operator=(xtcash_basic_runnable const &) = default;
    xtcash_basic_runnable(xtcash_basic_runnable &&) = default;
    xtcash_basic_runnable & operator=(xtcash_basic_runnable &&) = default;
    ~xtcash_basic_runnable() override = default;

#if !defined(XCXX14)
    xtcash_basic_runnable() noexcept {
    }
#else
    xtcash_basic_runnable() = default;
#endif

    bool running() const noexcept override {
        return m_running.load(std::memory_order_acquire);
    }

    void running(bool const value) noexcept override {
        m_running.store(value, std::memory_order_release);
    }
};

template <typename T>
using xbasic_runnable_t = xtcash_basic_runnable<T>;

template <typename T>
class xtcash_trival_runnable : public xrunnable_t<T> {
public:
    xtcash_trival_runnable(xtcash_trival_runnable const &) = default;
    xtcash_trival_runnable & operator=(xtcash_trival_runnable const &) = default;
    xtcash_trival_runnable(xtcash_trival_runnable &&) = default;
    xtcash_trival_runnable & operator=(xtcash_trival_runnable &&) = default;
    ~xtcash_trival_runnable() override = default;

#if !defined(XCXX14)
    xtcash_trival_runnable() noexcept {
    }
#else
    xtcash_trival_runnable() = default;
#endif
    bool running() const noexcept override {
        return true;
    }

    void running(bool const) noexcept override {
    }
};

template <typename T>
using xtrival_runnable_t = xtcash_trival_runnable<T>;

NS_END1
