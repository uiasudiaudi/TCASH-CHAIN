#pragma once
#include "xbasic/xrunnable.h"
#include "xdata/xchain_param.h"
#include "xelect_net/include/elect_manager.h"
#include "xelect_net/include/elect_vhost.h"
#include "xpbase/base/args_parser.h"
#include "xpbase/base/tcash_config.h"
#include "xtransport/udp_transport/multi_message_handler.h"
#include "xtransport/transport.h"
#include "xwrouter/xwrouter.h"

#include <vector>

namespace tcash {

namespace elect {

class MultilayerNetwork : public xtrival_runnable_t<MultilayerNetwork> {
public:
    MultilayerNetwork(common::xnode_id_t const & node_id, const std::set<uint32_t> & xnetwork_id_set);
    MultilayerNetwork() = delete;
    MultilayerNetwork(MultilayerNetwork const &) = delete;
    MultilayerNetwork & operator=(MultilayerNetwork const &) = delete;
    MultilayerNetwork(MultilayerNetwork &&) = delete;
    MultilayerNetwork & operator=(MultilayerNetwork &&) = delete;
    ~MultilayerNetwork() override = default;

    void TryCombineP2PEndpoints();

    bool Init(const base::Config & config);
    bool Run(const base::Config & config);
    void start() override;
    void stcash() override;

public:
    inline EcNetcardPtr & GetEcNetcard() {
        return ec_netcard_;
    }

    inline std::shared_ptr<ElectManager> GetElectManager() const noexcept {
        return elect_manager_;
    }

    bool RegisterNodeCallback(std::function<int32_t(std::string const & node_addr, std::string const & node_sign)> cb);

    bool UpdateNodeSizeCallback(std::function<void(uint64_t & node_size, std::error_code & ec)> cb);

    std::shared_ptr<elect::xnetwork_driver_face_t> GetEcVhost(const uint32_t & xnetwork_id) const noexcept;

    std::vector<std::string> GetServiceNeighbours(const common::xip2_t & xip2);

protected:
    inline transport::TransportPtr & GetCoreTransport() {
        return core_transport_;
    };
    int ResetRootRouting(std::shared_ptr<transport::Transport> transport, const base::Config & config);

private:
    int CreateRootManager(std::shared_ptr<transport::Transport> transport,
                          const tcash::base::Config & config,
                          const std::set<std::pair<std::string, uint16_t>> & p2p_endpoints_config);

private:
    mutable std::mutex vhost_map_mutex_;
    std::map<uint32_t, EcVHostPtr> vhost_map_;
    std::shared_ptr<ElectManager> elect_manager_{nullptr};
    common::xnode_id_t m_node_id_;
    std::set<uint32_t> xnetwork_id_set_;

private:
    std::shared_ptr<wrouter::RootRoutingManager> root_manager_ptr_{nullptr};
    transport::TransportPtr core_transport_{nullptr};
    transport::TransportPtr nat_transport_{nullptr};
    std::shared_ptr<transport::MultiThreadHandler> multi_message_handler_{nullptr};
    EcNetcardPtr ec_netcard_{nullptr};
};

}  // namespace elect

}  // namespace tcash
