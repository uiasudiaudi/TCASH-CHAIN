#pragma once

#include "xcommon/common.h"
#include "xevm_common/xcrosschain/xeth_header.h"
#include "xevm_common/xcrosschain/xheco_config.h"
#include "xevm_common/xcrosschain/xheco_gaslimit.h"

NS_BEG3(tcash, evm_common, heco)

bool verify_eip1559_header(const xeth_header_t & parent_header, const xeth_header_t & header);

NS_END3
