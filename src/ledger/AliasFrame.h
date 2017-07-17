#pragma once
#include "ledger/EntryFrame.h"
#include "database/Database.h"
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
		static const char* aliasAndAccountSelector;
		bool checkExistAccountWithIdAlias(AccountID const& accountID);

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

		AliasEntry&
			getAlias()
		{
			clearCached();
			return mAlias;
		}

		virtual void storeDelete(LedgerDelta& delta, Database& db) const;
		virtual void storeAdd(LedgerDelta& delta, Database& db);
		virtual void storeChange(LedgerDelta& delta, Database& db);

		bool isValid() { // TODO: make check
			return true; 
		}

		static bool AliasFrame::exists(Database& db, LedgerKey const& key);
		static bool AliasFrame::isExist(AccountID const& aliasID, AccountID const& accountID, Database& db);
		static bool isAliasIdExist(AccountID const & aliasID, Database & db);
		static void loadAliases(StatementContext & prep, std::function<void(LedgerEntry const&)> aliasProcessor);
		static std::unordered_map<AccountID, std::vector<AliasFrame::pointer>> loadAllAliases(Database & db);
		static AliasFrame::pointer loadAlias(LedgerDelta& delta, AccountID const& aliasID, Database& db);

		static AliasFrame::pointer loadAlias(AccountID const& aliasID, AccountID const& accountID, Database& db);

		static void storeDelete(LedgerDelta& delta, Database& db, LedgerKey const& key);
		static void dropAll(Database& db);
		static const char* kSQLCreateStatement1;
		static const char* aliasColumSelector;
	};
}

