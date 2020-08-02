#include "system_config_types.h"
#include "Utilities/Config.h"

LOG_CHANNEL(rpcn_cfg, "RPCN Config");

struct cfg_rpcn : cfg::node
{
	cfg::string host{this, "Host", "np.rpcs3.net"};
	cfg::string npid{this, "NPID", ""};
	cfg::string password{this, "Password", ""};

	void load();
	void save() const;

	std::string get_host() const;
	std::string get_npid(); // not const because it can save if npid is requested and it has never been set
	std::string get_password() const;

	void set_host(const std::string& host);
	void set_npid(const std::string& npid);
	void set_password(const std::string& password);

	private:
	static std::string get_path();
	static std::string generate_npid();
};

extern cfg_rpcn g_cfg_rpcn;
