#pragma once


#include "xbase/xobject_ptr.h"
#include "xvledger/xvstate.h"

#if defined(XCXX20)
#include <fifo_map.hpp>
#else
#include <nlohmann/fifo_map.hpp>
#endif
#include <nlohmann/json.hpp>


NS_BEG2(tcash, db_export)

template <class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;

using unordered_json = nlohmann::basic_json<my_workaround_fifo_map>;
using json = unordered_json;

void property_json(xobject_ptr_t<base::xvbstate_t> const & state, json & j);

NS_END2
