#pragma once

#include "ledger/AccountFrame.h"
#include "ledger/EntryFrame.h"
#include <functional>
#include <unordered_map>

namespace soci
{
	class session;
	namespace details
	{
		class prepare_temp_type;
	}
}

namespace stellar {
	class LedgerManager;

	class AliasFrame : EntryFrame
	{
	public:
		//AliasFrame();
		~AliasFrame();
	};
}

