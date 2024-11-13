// Copyright (c) 2018-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xevm_common/xtriecommon.h"

namespace tcash {
namespace evm_common {


h256 const EmptySHA3 = sha3(bytesConstRef());
h256 const EmptyListSHA3 = sha3(rlpList());

bool sha3(bytesConstRef _input, bytesRef o_output) noexcept
{
    if (o_output.size() != 32) {
		return false;
	}

	const uint256_t tcash_hash = utl::xkeccak256_t::digest((const unsigned char *)_input.data(), _input.size());
	bytesConstRef((const unsigned char*)tcash_hash.data(), 32).copyTo(o_output);

   /* ethash::hash256 h = ethash::keccak256(_input.data(), _input.size());
    bytesConstRef{h.bytes, 32}.copyTo(o_output);*/
    return true;
}




h256 const EmptyTrie = sha3(xrlp(""));

/*
 * Hex-prefix Notation. First nibble has flags: oddness = 2^0 & termination = 2^1
 * NOTE: the "termination marker" and "leaf-node" specifier are completely equivalent.
 * [0,0,1,2,3,4,5]   0x10012345
 * [0,1,2,3,4,5]     0x00012345
 * [1,2,3,4,5]       0x112345
 * [0,0,1,2,3,4]     0x00001234
 * [0,1,2,3,4]       0x101234
 * [1,2,3,4]         0x001234
 * [0,0,1,2,3,4,5,T] 0x30012345
 * [0,0,1,2,3,4,T]   0x20001234
 * [0,1,2,3,4,5,T]   0x20012345
 * [1,2,3,4,5,T]     0x312345
 * [1,2,3,4,T]       0x201234
 */

std::string hexPrefixEncode(xbytes_t const & _hexVector, bool _leaf, int _begin, int _end) {
	unsigned begin = _begin;
	unsigned end = _end < 0 ? _hexVector.size() + 1 + _end : _end;
	bool odd = ((end - begin) % 2) != 0;

	std::string ret(1, ((_leaf ? 2 : 0) | (odd ? 1 : 0)) * 16);
	if (odd)
	{
		ret[0] |= _hexVector[begin];
		++begin;
	}
	for (unsigned i = begin; i < end; i += 2)
		ret += _hexVector[i] * 16 + _hexVector[i + 1];
	return ret;
}

std::string hexPrefixEncode(bytesConstRef _data, bool _leaf, int _beginNibble, int _endNibble, unsigned _offset)
{
	unsigned begin = _beginNibble + _offset;
	unsigned end = (_endNibble < 0 ? ((int)(_data.size() * 2 - _offset) + 1) + _endNibble : _endNibble) + _offset;
	bool odd = (end - begin) & 1;

	std::string ret(1, ((_leaf ? 2 : 0) | (odd ? 1 : 0)) * 16);
	ret.reserve((end - begin) / 2 + 1);

	unsigned d = odd ? 1 : 2;
	for (auto i = begin; i < end; ++i, ++d)
	{
		xbyte_t n = nibble(_data, i);
		if (d & 1)	// odd
			ret.back() |= n;		// or the nibble onto the back
		else
			ret.push_back(n << 4);	// push the nibble on to the back << 4
	}
	return ret;
}

std::string hexPrefixEncode(bytesConstRef _d1, unsigned _o1, bytesConstRef _d2, unsigned _o2, bool _leaf)
{
	unsigned begin1 = _o1;
	unsigned end1 = _d1.size() * 2;
	unsigned begin2 = _o2;
	unsigned end2 = _d2.size() * 2;

	bool odd = (end1 - begin1 + end2 - begin2) & 1;

	std::string ret(1, ((_leaf ? 2 : 0) | (odd ? 1 : 0)) * 16);
	ret.reserve((end1 - begin1 + end2 - begin2) / 2 + 1);

	unsigned d = odd ? 1 : 2;
	for (auto i = begin1; i < end1; ++i, ++d)
	{
		xbyte_t n = nibble(_d1, i);
		if (d & 1)	// odd
			ret.back() |= n;		// or the nibble onto the back
		else
			ret.push_back(n << 4);	// push the nibble on to the back << 4
	}
	for (auto i = begin2; i < end2; ++i, ++d)
	{
		xbyte_t n = nibble(_d2, i);
		if (d & 1)	// odd
			ret.back() |= n;		// or the nibble onto the back
		else
			ret.push_back(n << 4);	// push the nibble on to the back << 4
	}
	return ret;
}

xbyte_t uniqueInUse(RLP const& _orig, xbyte_t except)
{
	xbyte_t used = 255;
	for (unsigned i = 0; i < 17; ++i)
		if (i != except && !_orig[i].isEmpty())
		{
			if (used == 255)
				used = (xbyte_t)i;
			else
				return 255;
		}
	return used;
}


}
}