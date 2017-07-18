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

	public:
		static const char* kSQLCreateStatement1;
		static const char* aliasColumSelector;

		typedef std::shared_ptr<AliasFrame> pointer;

		AliasFrame();
		AliasFrame(LedgerEntry const& from);
		~AliasFrame();

		AliasFrame& operator=(AliasFrame const& other);

		EntryFrame::pointer copy() const override;

		AliasEntry& getAlias();

		static void dropAll(Database& db);
		static void storeDelete(LedgerDelta& delta, Database& db, LedgerKey const& key);
		static void loadAliases(StatementContext & prep, std::function<void(LedgerEntry const&)> aliasProcessor);

		static bool exists(Database& db, LedgerKey const& key);
		static bool isExist(AccountID const& aliasID, AccountID const& accountID, Database& db);
		static bool isAliasIdExist(AccountID const & aliasID, Database & db);
		static bool isValid(AliasEntry const& tl);
		static uint64_t countObjects(soci::session& sess);

		static std::unordered_map<AccountID, std::vector<AliasFrame::pointer>> loadAllAliases(Database & db);
		static pointer loadAlias(LedgerDelta& delta, AccountID const& aliasID, Database& db);
		static pointer loadAlias(AccountID const& aliasID, AccountID const& accountID, Database& db);
		
		bool isValid() const;

		virtual void storeDelete(LedgerDelta& delta, Database& db) const;
		virtual void storeAdd(LedgerDelta& delta, Database& db);
		virtual void storeChange(LedgerDelta& delta, Database& db);

	};
}

