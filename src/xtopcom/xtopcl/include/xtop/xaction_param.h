#pragma once
#include <string>
#include "xtcashcl/include/api_method_imp.h"
#include "xdata/xproperty.h"

namespace xChainSDK {

    class xaction_param {
    public:
        xaction_param(api_method_imp* method);
        virtual ~xaction_param() {}
        virtual std::string create() = 0;
        api_method_imp* m_method_ptr;
    };

    class xaction_asset_param : public xaction_param {
    public:
        xaction_asset_param(api_method_imp* method, const std::string& token_name, uint64_t amount);
        virtual std::string create() override;
    private:
        std::string m_token_name{ tcash::data::XPROPERTY_ASSET_tcash };
        uint64_t    m_amount{ 0 };
    };
    class xaction_pledge_token_vote_param : public xaction_param {
    public:
        xaction_pledge_token_vote_param(api_method_imp* method,
            uint64_t amount, uint16_t lock_duration);
        virtual std::string create() override;
    private:
        uint64_t            m_vote_num;
        uint16_t            m_lock_duration;  // unit day
    };

    class xaction_redeem_token_vote_param : public xaction_param {
    public:
        xaction_redeem_token_vote_param(api_method_imp* method,
            uint64_t amount);
        virtual std::string create() override;
    private:
        uint64_t            m_vote_num;
    };

}
