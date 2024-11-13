#include <string>
#include "xdata/xfull_tableblock.h"

#include "xcertauth/src/xsigndata.h"

class xdb_tool {
public:
    void init_xdb_tool(std::string const& db_path);
    void get_voteinfo_from_block(std::string const& tableblock_addr, uint64_t start, uint64_t end);
    uint64_t get_blockheight(std::string const& tableblock_addr) const;
    std::string cons_electinfo_by_height(uint64_t height, bool print = false) const;
    void all_cons_electinfo() const;

    void credit_data(uint64_t table_id);
    void specific_clockheight(uint64_t start_gmttime, uint64_t end_gmttime);

    tcash::data::xstatistics_data_t get_fulltable_statistic(std::string const& tableblock_addr, uint64_t height);

private:
    std::string get_multisig_votestr(tcash::auth::xmutisigdata_t const& aggregated_sig_obj) const;
    // void xdb_tool::get_electinfo_by_height(std::string const& elect_addr, uint64_t height) const;


private:
    std::string db_path_;
    tcash::xobject_ptr_t<tcash::store::xstore_face_t> store_; // mainnet just using m_store
    tcash::xobject_ptr_t<tcash::base::xvblockstore_t> blockstore_;
};