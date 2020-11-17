#pragma once

#include <unordered_map>
#include <chrono>
#include <condition_variable>
#include "Utilities/mutex.h"

#include "util/asm.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "Emu/Cell/Modules/sceNp.h"
#include "Emu/Cell/Modules/sceNp2.h"

#include "generated/np2_structs_generated.h"

#include <wolfssl/ssl.h>

class vec_stream
{
public:
	vec_stream() = delete;
	vec_stream(std::vector<u8>& _vec, usz initial_index = 0)
	    : vec(_vec), i(initial_index) {}
	bool is_error() const
	{
		return error;
	}

	// Getters

	template <typename T>
	T get()
	{
		if (sizeof(T) + i > vec.size())
		{
			error = true;
			return 0;
		}
		T res = *utils::bless<le_t<T, 1>>(&vec[i]);
		i += sizeof(T);
		return res;
	}
	std::string get_string(bool empty)
	{
		std::string res{};
		while (i < vec.size() && vec[i] != 0)
		{
			res.push_back(vec[i]);
			i++;
		}
		i++;

		if (!empty && res.empty())
		{
			error = true;
		}

		return res;
	}
	std::vector<u8> get_rawdata()
	{
		std::vector<u8> ret;
		u32 size = get<u32>();

		if ((vec.begin() + i + size) <= vec.end())
			std::copy(vec.begin() + i, vec.begin() + i + size, std::back_inserter(ret));
		else
			error = true;

		return ret;
	}

	// Setters

	template <typename T>
	void insert(T value)
	{
		value = std::bit_cast<le_t<T>, T>(value);
		// resize + memcpy instead?
		for (usz index = 0; index < sizeof(T); index++)
		{
			vec.push_back(*(reinterpret_cast<u8*>(&value) + index));
		}
	}
	void insert_string(const std::string& str) const
	{
		std::copy(str.begin(), str.end(), std::back_inserter(vec));
		vec.push_back(0);
	}

protected:
	std::vector<u8>& vec;
	usz i      = 0;
	bool error = false;
};

enum CommandType : u16
{
	Login,
	Terminate,
	Create,
	SendToken,
	AddFriend,
	RemoveFriend,
	AddBlock,
	RemoveBlock,
	GetServerList,
	GetWorldList,
	CreateRoom,
	JoinRoom,
	LeaveRoom,
	SearchRoom,
	SetRoomDataExternal,
	GetRoomDataInternal,
	SetRoomDataInternal,
	PingRoomOwner,
	SendRoomMessage,
	RequestSignalingInfos,
	RequestTicket,
};

class rpcn_client
{
private:
	enum PacketType : u8
	{
		Request,
		Reply,
		Notification,
		ServerInfo,
	};

	enum ErrorType : u8
	{
		NoError,
		Malformed,
		Invalid,
		InvalidInput,
		ErrorLogin,
		ErrorCreate,
		AlreadyLoggedIn,
		AlreadyJoined,
		DbFail,
		NotFound,
		Blocked,
		AlreadyFriend,
		Unsupported,
		__error_last
	};

	static inline std::weak_ptr<rpcn_client> instance;
	static inline shared_mutex inst_mutex;

	std::thread thread_rpcn, thread_rpcn_reader, thread_rpcn_writer;

	std::condition_variable cond_reader, cond_writer, cond_rpcn;
	std::mutex mutex_cond_reader, mutex_cond_writer, mutex_rpcn;

	atomic_t<bool> terminate = false;

	std::vector<std::vector<u8>> packets_to_send;
	std::mutex mutex_packets_to_send;

private:
	rpcn_client();

	void rpcn_reader_thread();
	void rpcn_writer_thread();
	void rpcn_thread();
	
	bool handle_input();
	bool handle_output();

private:
	enum class recvn_result
	{
		recvn_success,
		recvn_nodata,
		recvn_timeout,
		recvn_noconn,
		recvn_fatal,
	};

	recvn_result recvn(u8* buf, usz n);
	bool send_packet(const std::vector<u8>& packet);

public:
	~rpcn_client();
	rpcn_client(rpcn_client& other) = delete;
	void operator=(const rpcn_client&) = delete;
	static std::shared_ptr<rpcn_client> get_instance();

	bool connect(const std::string& host);
	bool login(const std::string& npid, const std::string& password, const std::string& token);
	bool create_user(const std::string& npid, const std::string& password, const std::string& online_name, const std::string& avatar_url, const std::string& email);
	void disconnect();
	bool manage_connection();
	std::vector<std::pair<u16, std::vector<u8>>> get_notifications();
	std::unordered_map<u32, std::pair<u16, std::vector<u8>>> get_replies();

	// Synchronous requests
	bool get_server_list(u32 req_id, const SceNpCommunicationId& communication_id, std::vector<u16>& server_list);
	// Asynchronous requests
	bool get_world_list(u32 req_id, const SceNpCommunicationId& communication_id, u16 server_id);
	bool createjoin_room(u32 req_id, const SceNpCommunicationId& communication_id, const SceNpMatching2CreateJoinRoomRequest* req);
	bool join_room(u32 req_id, const SceNpCommunicationId& communication_id, const SceNpMatching2JoinRoomRequest* req);
	bool leave_room(u32 req_id, const SceNpCommunicationId& communication_id, const SceNpMatching2LeaveRoomRequest* req);
	bool search_room(u32 req_id, const SceNpCommunicationId& communication_id, const SceNpMatching2SearchRoomRequest* req);
	bool set_roomdata_external(u32 req_id, const SceNpCommunicationId& communication_id, const SceNpMatching2SetRoomDataExternalRequest* req);
	bool get_roomdata_internal(u32 req_id, const SceNpCommunicationId& communication_id, const SceNpMatching2GetRoomDataInternalRequest* req);
	bool set_roomdata_internal(u32 req_id, const SceNpCommunicationId& communication_id, const SceNpMatching2SetRoomDataInternalRequest* req);
	bool ping_room_owner(u32 req_id, const SceNpCommunicationId& communication_id, u64 room_id);
	bool send_room_message(u32 req_id, const SceNpCommunicationId& communication_id, const SceNpMatching2SendRoomMessageRequest* req);
	bool req_sign_infos(u32 req_id, const std::string& npid);
	bool req_ticket(u32 req_id, const std::string& service_id);

	const std::string& get_online_name() const
	{
		return online_name;
	}
	const std::string& get_avatar_url() const
	{
		return avatar_url;
	}

	u32 get_addr_sig() const
	{
		return addr_sig.load();
	}
	u16 get_port_sig() const
	{
		return port_sig.load();
	}

protected:
	bool get_reply(u32 expected_id, std::vector<u8>& data);

	std::vector<u8> forge_request(u16 command, u32 packet_id, const std::vector<u8>& data) const;
	bool forge_send(u16 command, u32 packet_id, const std::vector<u8>& data);
	bool forge_send_reply(u16 command, u32 packet_id, const std::vector<u8>& data, std::vector<u8>& reply_data);

	bool is_error(ErrorType err) const;
	bool error_and_disconnect(const std::string& error_mgs);

	std::string get_wolfssl_error(WOLFSSL* wssl, int error) const;

protected:
	atomic_t<bool> connected    = false;
	atomic_t<bool> authentified = false;

	WOLFSSL_CTX* wssl_ctx = nullptr;
	WOLFSSL* read_wssl    = nullptr;
	WOLFSSL* write_wssl   = nullptr;

	bool abort_config = false;

	atomic_t<bool> server_info_received = false;
	u32 received_version                = 0;

	// UDP Signaling related
	steady_clock::time_point last_ping_time{}, last_pong_time{};

	sockaddr_in addr_rpcn{};
	sockaddr_in addr_rpcn_udp{};
	int sockfd = 0;

	shared_mutex mutex_notifs, mutex_replies, mutex_replies_sync;
	std::vector<std::pair<u16, std::vector<u8>>> notifications;            // notif type / data
	std::unordered_map<u32, std::pair<u16, std::vector<u8>>> replies;      // req id / (command / data)
	std::unordered_map<u32, std::pair<u16, std::vector<u8>>> replies_sync; // same but for sync replies(Login, Create, GetServerList)

	std::string online_name{};
	std::string avatar_url{};

	s64 user_id = 0;

	atomic_t<u32> addr_sig{};
	atomic_t<u16> port_sig{};
};
