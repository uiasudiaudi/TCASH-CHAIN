#pragma once

#include "xcommon/common.h"

NS_BEG3(tcash, evm_common, heco)

bool verify_gaslimit(const u256 parent_gas_limit, const u256 header_gas_limit);

NS_END3
