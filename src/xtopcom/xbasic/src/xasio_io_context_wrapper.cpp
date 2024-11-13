// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xbase/xlog.h"
#include "xbasic/xasio_io_context_wrapper.h"
#include "xbasic/xmemory.hpp"
#include "xbasic/xthreading/xbackend_thread.hpp"

#include <cassert>

NS_BEG1(tcash)

void
xtcash_asio_io_context_wrapper::start() {
    assert(m_work_guard == nullptr);

#if !defined NDEBUG
    m_thread_id = std::this_thread::get_id();
#endif

    assert(!running());
    running(true);

    m_work_guard = tcash::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(m_io_context.get_executor());
    m_io_context.restart();
    m_io_context.run();
}

void
xtcash_asio_io_context_wrapper::async_start() {
    auto self = shared_from_this();
    threading::xbackend_thread::spawn([this, self] {
        running(true);
        while (running()) {
            try {
                m_work_guard = tcash::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(m_io_context.get_executor());
                m_io_context.restart();
                m_io_context.run();
            } catch (std::exception const & eh) {
                xerror("[io context] unhandled exception %s", eh.what());
            }
        }
    });
}

void
xtcash_asio_io_context_wrapper::stcash() {
    assert(!m_io_context.stcashped());

    m_work_guard.reset();
    m_io_context.stcash();
#if !defined NDEBUG
    m_thread_id = std::thread::id{};
#endif

    running(false);
}

#if !defined NDEBUG
std::thread::id const &
xtcash_asio_io_context_wrapper::thread_id() const noexcept {
    return m_thread_id;
}
#endif

xtcash_base_io_context_wrapper::xtcash_base_io_context_wrapper() : m_iothread_ptr{make_object_ptr<base::xiothread_t>()} {
}

xtcash_base_io_context_wrapper::xtcash_base_io_context_wrapper(xobject_ptr_t<base::xiothread_t> io_thread) : m_iothread_ptr{std::move(io_thread)} {
}

void xtcash_base_io_context_wrapper::start() {
}

void xtcash_base_io_context_wrapper::stcash() {
}

bool xtcash_base_io_context_wrapper::running() const noexcept {
    return m_iothread_ptr->is_running();
}

void xtcash_base_io_context_wrapper::running(bool const) noexcept {
    assert(false);
}

base::xcontext_t & xtcash_base_io_context_wrapper::context() const noexcept {
    return *m_iothread_ptr->get_context();
}

std::int32_t xtcash_base_io_context_wrapper::thread_id() const noexcept {
    return m_iothread_ptr->get_thread_id();
}
NS_END1
