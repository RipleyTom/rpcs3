#include "stdafx.h"
#include "Emu/Cell/PPUModule.h"
#include "Emu/Cell/lv2/sys_sync.h"
#include "Emu/system_config.h"
#include "Emu/Cell/Modules/cellSysutil.h"
#include "Emu/Memory/vm_ptr.h"
#include "Emu/IdManager.h"
#include "np_handler.h"
#include "np_contexts.h"
#include "np_helpers.h"
#include "np_structs_extra.h"

LOG_CHANNEL(rpcn_log, "rpcn");

namespace np
{
	std::pair<error_code, std::shared_ptr<matching_ctx>> gui_prelude(u32 ctx_id, vm::ptr<SceNpMatchingGUIHandler> handler, vm::ptr<void> arg)
	{
		auto ctx = get_matching_context(ctx_id);

		if (!ctx)
			return {SCE_NP_MATCHING_ERROR_CTX_NOT_FOUND, {}};

		if (!ctx->busy.compare_and_swap_test(0, 1))
			return {SCE_NP_MATCHING_ERROR_CTX_STILL_RUNNING, {}};

		ctx->ctx_id = ctx_id;
		ctx->gui_handler = handler;
		ctx->gui_arg = arg;

		ctx->queue_gui_cb(SCE_NP_MATCHING_GUI_EVENT_COMMON_LOAD, 0);

		return {CELL_OK, ctx};
	}

	void gui_epilog(std::shared_ptr<matching_ctx>& ctx)
	{
		ctx->queue_gui_cb(SCE_NP_MATCHING_GUI_EVENT_COMMON_UNLOAD, 0);
	}

	error_code np_handler::create_room_gui(u32 ctx_id, vm::cptr<SceNpCommunicationId> communicationId, vm::cptr<SceNpMatchingAttr> attr, vm::ptr<SceNpMatchingGUIHandler> handler, vm::ptr<void> arg)
	{
		auto&& [error, ctx] = gui_prelude(ctx_id, handler, arg);

		if (error)
			return error;

		u32 req_id = get_req_id(REQUEST_ID_HIGH::GUI);
		add_gui_request(req_id);

		get_rpcn()->createjoin_room_gui(req_id, *communicationId, attr.get_ptr());

		return CELL_OK;
	}
}
