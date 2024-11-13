#include <iostream>
#include <gtest/gtest.h>
#include "xbase/xlog.h"
#include "xbase/xhash.h"
#include "xblockstore/xblockstore_face.h"
#include "xutility/xhash.h"
#include "xdata/xrootblock.h"
#include "xconfig/xconfig_register.h"
#include "xconfig/xpredefined_configurations.h"
#include "xmetrics/xmetrics.h"
#include "xloader/xconfig_genesis_loader.h"

using namespace std;
using namespace top;
using namespace top::config;
class xhashtest_t : public top::base::xhashplugin_t
{
public:
    xhashtest_t():
        top::base::xhashplugin_t(-1) //-1 = support every hash types
    {
    }
private:
    xhashtest_t(const xhashtest_t &);
    xhashtest_t & operator = (const xhashtest_t &);
    virtual ~xhashtest_t(){};
public:
    virtual const std::string hash(const std::string & input,enum_xhash_type type) override
    {
        auto hash = top::utl::xsha2_256_t::digest(input);
        return std::string(reinterpret_cast<char*>(hash.data()), hash.size());
    }
};

int main(int argc, char **argv) {
    cout << "xtxexecutor test main run" << endl;
    // printf("Running main() from gtest_main.cc\n");
    new xhashtest_t();

    auto genesis_loader = std::make_shared<top::loader::xconfig_genesis_loader_t>(std::string{});
    top::data::xrootblock_para_t rootblock_para;
    genesis_loader->extract_genesis_para(rootblock_para);
    top::data::xrootblock_t::init(rootblock_para);

    top::config::xconfig_register_t::get_instance().set(std::string{xchain_name_configuration_t::name}, std::string{chain_name_testnet});
    top::config::xconfig_register_t::get_instance().set(std::string{xtop_recv_tx_cache_window_configuration::name}, 30);
    top::config::xconfig_register_t::get_instance().set(std::string{xtop_toggle_whitelist_onchain_goverance_parameter::name}, false);
    top::config::xconfig_register_t::get_instance().set(std::string{xtop_tx_send_timestamp_tolerance_onchain_goverance_parameter::name}, 300);
    top::config::xconfig_register_t::get_instance().set(std::string{xtop_account_send_queue_tx_max_num_configuration::name}, 32);
    top::config::xconfig_register_t::get_instance().set(std::string{xtop_unitblock_recv_transfer_tx_batch_num_configuration::name}, 32);
    top::config::xconfig_register_t::get_instance().set(std::string{xtop_unitblock_send_transfer_tx_batch_num_configuration::name}, 32);
    top::config::xconfig_register_t::get_instance().set(std::string{xtop_fullunit_contain_of_unit_num_onchain_goverance_parameter::name}, 21);

    testing::InitGoogleTest(&argc, argv);

    xinit_log("./xblockmaker_test.log", true, true);
    xset_log_level(enum_xlog_level_debug);
    xdbg("------------------------------------------------------------------");
    xinfo("new log start here");
    XMETRICS_INIT();

    auto i = RUN_ALL_TESTS();
    sleep(2);
    XMETRICS_UNINT();
    sleep(15);  // for xbase exit double free abnormal issue
    return 0;
}
