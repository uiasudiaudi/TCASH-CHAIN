// Copyright (c) 2018-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once
#include "xbasic/xfixed_hash.h"
#include <vector>

namespace tcash {
namespace evm_common {
  

	xbytes_t rlp256(BytesMap const & _s);
	h256 hash256(BytesMap const& _s);

	h256 orderedTrieRoot(std::vector<xbytes_t> const & _data);

	template <class T, class U> inline h256 trieRootOver(unsigned _itemCount, T const& _getKey, U const& _getValue)
	{
		BytesMap m;
		for (unsigned i = 0; i < _itemCount; ++i)
			m[_getKey(i)] = _getValue(i);
		return hash256(m);
	}

	h256 orderedTrieRoot(std::vector<bytesConstRef> const& _data);
    h256 orderedTrieRoot(std::vector<xbytes_t> const & _data);


}
}