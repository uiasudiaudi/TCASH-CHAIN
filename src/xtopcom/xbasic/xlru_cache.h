// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include <string>
#include <unordered_map>
#include <list>
#include <mutex>
#include "xbase/xns_macro.h"

#if defined(__clang__)

#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpedantic"
#elif defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(_MSC_VER)
#    pragma warning(push, 0)
#endif

#include "xbase/xobject.h"

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

NS_BEG2(tcash, basic)
// from https://stackoverflow.com/questions/2504178/lru-cache-design
template<typename T>
struct xvoid_deletor_t {
    void operator() (T& t) {}
};

template<typename T>
struct xref_deleter_t {
    static_assert(std::is_base_of<base::xrefcount_t, T>::value, "");
    void operator() (T * t) {
        t->release_ref();
    }
};

template<typename key, typename value, typename deletor=xvoid_deletor_t<value>>
class xlru_cache {
 public:
    using key_value = std::pair<key, value>;

    xlru_cache(size_t max_size) :
        m_max_size(max_size) {
    }

    virtual ~xlru_cache() {
        for(auto& pair : m_item_list) {
            delete_value(pair.second);
        }
    }

    void put(const key& k, const value& v) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_item_map.find(k);
        if (it != m_item_map.end()) {
            delete_value(it->second->second);
            m_item_list.erase(it->second);
            m_item_map.erase(it);
        }
        m_item_list.push_front(std::make_pair(k, v));
        m_item_map.insert(std::make_pair(k, m_item_list.begin()));
        clean();
    }

    bool get(const key& k, value& v) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_item_map.find(k);
        if (it == m_item_map.end()) {
            return false;
        } else {
            m_item_list.splice(m_item_list.begin(), m_item_list, it->second);
            v = it->second->second;
            return true;
        }
    }

    bool exist(const key& k) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return (m_item_map.count(k) > 0);
    }

    void set_max_size(uint32_t max_size) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_max_size = max_size;
        clean();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (m_item_map.size() > 0) {
            auto last_it = m_item_list.end();
            last_it--;
            m_item_map.erase(last_it->first);
            delete_value(last_it->second);
            m_item_list.pop_back();
        }
    }

 private:
    inline void delete_value(value& v) {
        deletor del;
        del(v);
    }

    void clean() {
        while (m_item_map.size() > m_max_size) {
            auto last_it = m_item_list.end();
            last_it--;
            m_item_map.erase(last_it->first);
            delete_value(last_it->second);
            m_item_list.pop_back();
        }
    }

 protected:
    std::list<key_value> m_item_list;
    std::unordered_map<key, decltype(m_item_list.begin())> m_item_map;
    size_t m_max_size;
    std::mutex m_mutex;
};
NS_END2
