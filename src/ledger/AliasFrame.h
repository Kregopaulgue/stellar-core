#pragma once
#include "ledger/EntryFrame.h"
#include <functional>
#include <map>
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

	class AliasFrame : public EntryFrame
	{

	private:
		AliasEntry &mAlias;
		AliasFrame(AliasFrame const& from);

	public:
		typedef std::shared_ptr<AliasFrame> pointer;

		AliasFrame();
		AliasFrame(LedgerEntry const& from);

		AliasFrame& operator=(AliasFrame const& other);

		EntryFrame::pointer
			copy() const override
		{
			return EntryFrame::pointer(new AliasFrame(*this));
		}

		~AliasFrame();

		virtual void storeDelete(LedgerDelta& delta, Database& db) const;
		virtual void storeAdd(LedgerDelta& delta, Database& db);
		virtual void storeChange(LedgerDelta& delta, Database& db);


		static void storeDelete(LedgerDelta& delta, Database& db, LedgerKey const& key);
		static void dropAll(Database& db);
		static const char* kSQLCreateStatement1;

	};
}

