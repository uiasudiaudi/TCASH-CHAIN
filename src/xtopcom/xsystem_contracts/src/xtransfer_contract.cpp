#include "xsystem_contracts/xtransfer_contract.h"

#include "xdata/xgenesis_data.h"

NS_BEG2(tcash, system_contracts)

void xtcash_transfer_contract::setup() {
    common::xtoken_t token{100};
    m_balance.deposit(std::move(token));
}

void xtcash_transfer_contract::transfer(uint64_t const amount) {
    m_balance.withdraw(amount);
}

NS_END2
