#include "ledger/AliasFrame.h"
#include "ledger/LedgerDelta.h"
#include "crypto/Hex.h"
#include "crypto/KeyUtils.h"
#include "crypto/SecretKey.h"
#include "crypto/SignerKey.h"
#include "ledger/LedgerManager.h"
#include "lib/util/format.h"
#include "util/basen.h"
#include "util/types.h"
#include "util/Logging.h"
#include <algorithm>

namespace stellar {

	const char* AliasFrame::kSQLCreateStatement1 =
		"CREATE TABLE aliases"
		"("
		"accountid       VARCHAR(56)	  NOT NULL,"
		"aliasid		 VARCHAR(56)      NOT NULL,"
		"lastmodified	 INT			  NOT NULL,"
		"PRIMARY KEY  (aliasid)"
		");";

	const char* AliasFrame::aliasColumSelector =
		"SELECT accountid, aliasid, lastmodified FROM aliases";

	const char* AliasFrame::aliasAndAccountSelector =
		"SELECT * FROM account";

	AliasFrame::AliasFrame() : EntryFrame(ALIAS), mAlias(mEntry.data.alias()) { ; }

	AliasFrame::AliasFrame(AliasFrame const& from) : AliasFrame(from.mEntry) { ; }

	AliasFrame::AliasFrame(LedgerEntry const & from) : EntryFrame(from), mAlias(mEntry.data.alias()) { ; }

	AliasFrame::~AliasFrame() { ; }

	AliasFrame & AliasFrame::operator=(AliasFrame const & other)
	{
		if (&other != this)
		{
			mAlias = other.mAlias;
			mKey = other.mKey;
			mKeyCalculated = other.mKeyCalculated;
		}
		return *this;
	}

	EntryFrame::pointer AliasFrame::copy() const
	{
		return EntryFrame::pointer(new AliasFrame(*this));
	}

	void AliasFrame::storeDelete(LedgerDelta& delta, Database& db) const
	{
		storeDelete(delta, db, getKey());
	}

	void AliasFrame::storeAdd(LedgerDelta & delta, Database & db)
	{
		assert(isValid());
		auto key = getKey();
		flushCachedEntry(key, db);
		touch(delta);

		std::string aliasIDStrKey = KeyUtils::toStrKey(mAlias.aliasID);
		std::string accountIDStrKey = KeyUtils::toStrKey(mAlias.accountID);
		std::string sql = std::string(
			"INSERT INTO aliases ( accountid, aliasid, lastmodified ) "
			"VALUES ( :v1, :v2, :v3 )");

		auto prep = db.getPreparedStatement(sql);
		{
			using soci::use;
			soci::statement& st = prep.statement();
			st.exchange(use(accountIDStrKey, "v1"));
			st.exchange(use(aliasIDStrKey, "v2"));
			st.exchange(use(getLastModified(), "v3"));
			st.define_and_bind();
			{
				auto timer = db.getInsertTimer("alias");
				st.execute(true);
			}

			if (st.get_affected_rows() != 1)
			{
				throw std::runtime_error("Could not update data in SQL");
			}
			delta.addEntry(*this);
		}
	}

	//This method need to override because base class conteins clean virtualy method. (Keep empty)
	void AliasFrame::storeChange(LedgerDelta & delta, Database & db) { ; }

	void AliasFrame::storeDelete(LedgerDelta & delta, Database & db, LedgerKey const & key)
	{
		flushCachedEntry(key, db);
		std::string aliasIDStrKey = KeyUtils::toStrKey(key.alias().aliasID);
		auto timer = db.getDeleteTimer("alias");
		auto prep = db.getPreparedStatement(
			"DELETE from aliases where aliasid= :v1");
		auto& st = prep.statement();
		st.exchange(soci::use(aliasIDStrKey));
		st.define_and_bind();
		st.execute(true);
		delta.deleteEntry(key);
	}

	bool AliasFrame::exists(Database & db, LedgerKey const & key)
	{
		if (cachedEntryExists(key, db) && getCachedEntry(key, db) != nullptr)
		{
			return true;
		}

		using soci::use;
		using soci::into;

		std::string actIDStrKey = KeyUtils::toStrKey(key.alias().accountID);
		int exists = 0;
		{
			auto timer = db.getSelectTimer("alias-exists");
			auto prep =
				db.getPreparedStatement("SELECT EXISTS (SELECT NULL FROM aliases "
					"WHERE aliasid=:v1)");
			auto& st = prep.statement();
			st.exchange(use(actIDStrKey));
			st.exchange(into(exists));
			st.define_and_bind();
			st.execute(true);
		}
		return exists != 0;
	}

	bool AliasFrame::isExist(AccountID const & aliasID, AccountID const & AccountID, Database & db)
	{
		LedgerKey key;
		key.type(ALIAS);
		key.alias().aliasID = aliasID;
		key.alias().accountID = AccountID;
		return exists(db, key);
	}

	bool AliasFrame::isAliasIdExist(AccountID const & aliasID, Database & db)
	{
		using soci::use;
		using soci::into;

		std::string aliasIDStrKey = KeyUtils::toStrKey(aliasID);
		int exists = 0;
		{
			auto timer = db.getSelectTimer("alias-exists");
			auto prep =
				db.getPreparedStatement("SELECT EXISTS (SELECT NULL FROM aliases "
					"WHERE aliasid=:v1)");
			auto& st = prep.statement();
			st.exchange(use(aliasIDStrKey));
			st.exchange(into(exists));
			st.define_and_bind();
			st.execute(true);
		}
		return exists != 0;
	}

	bool AliasFrame::isValid(AliasEntry const & al)
	{
		bool res = !(al.accountID == al.aliasID);
		return res;
	}

	uint64_t AliasFrame::countObjects(soci::session & sess)
	{
		using soci::into;
		uint64_t count = 0;
		sess << "SELECT COUNT(*) FROM aliases;", into(count);
		return count;
	}

	void AliasFrame::loadAliases(StatementContext& prep, std::function<void(LedgerEntry const&)> aliasProcessor)
	{
		std::string aliasIDStrKey;
		std::string accountIDStrKey;
		LedgerEntry le;
		le.data.type(ALIAS);

		AliasEntry& al = le.data.alias();
		using soci::into;
		auto& st = prep.statement();
		st.exchange(into(accountIDStrKey));
		st.exchange(into(aliasIDStrKey));
		st.exchange(into(le.lastModifiedLedgerSeq));
		st.define_and_bind();

		st.execute(true);
		while (st.got_data())
		{
			al.aliasID = KeyUtils::fromStrKey<PublicKey>(aliasIDStrKey);
			al.accountID = KeyUtils::fromStrKey<PublicKey>(accountIDStrKey);
			
			if (!isValid(al))
			{
				throw std::runtime_error("Invalid AliasEntry");
			}
			
			aliasProcessor(le);
			st.fetch();
		}
	}

	std::unordered_map<AccountID, std::vector<AliasFrame::pointer>>
		AliasFrame::loadAllAliases(Database& db)
	{
		std::unordered_map<AccountID, std::vector<AliasFrame::pointer>> retAliases;

		auto query = std::string(aliasColumSelector);
		query += (" ORDER BY accountid");
		auto prep = db.getPreparedStatement(query);

		auto timer = db.getSelectTimer("alias");
		loadAliases(prep, [&retAliases](LedgerEntry const& cur) {
			auto& thisUserAliases = retAliases[cur.data.alias().accountID];
			thisUserAliases.emplace_back(std::make_shared<AliasFrame>(cur));
		});
		return retAliases;
	}

	AliasFrame::pointer AliasFrame::loadAlias(LedgerDelta &delta, AccountID const & aliasID, Database & db)
	{
		LedgerKey key;
		key.type(ALIAS);
		key.alias().aliasID = aliasID;

		AliasFrame::pointer res = std::make_shared<AliasFrame>();
		AliasEntry& alias = res->getAlias();

		std::string sql = aliasColumSelector;
		std::string aliasIDStrKey = KeyUtils::toStrKey(aliasID);
		std::string aliasIdStr, accountIdStr;
		sql += " WHERE aliasid = :v";

		auto prep = db.getPreparedStatement(sql);

		using soci::use;
		using soci::into;
		soci::statement& st = prep.statement();
		st.exchange(into(accountIdStr));
		st.exchange(into(aliasIdStr));
		st.exchange(into(res->getLastModified()));
		st.exchange(use(aliasIDStrKey, "v"));
		st.define_and_bind();
		{
			auto timer = db.getSelectTimer("alias");
			st.execute(true);
		}

		alias.aliasID = KeyUtils::fromStrKey<AccountID>(aliasIdStr);
		alias.accountID = KeyUtils::fromStrKey<AccountID>(accountIdStr);
		key.alias().accountID = alias.accountID;

		if (!st.got_data())
		{
			putCachedEntry(key, nullptr, db);
			return nullptr;
		}

		assert(res->isValid());
		res->mKeyCalculated = false;
		res->putCachedEntry(db);
		return res;
	}

	AliasFrame::pointer AliasFrame::loadAlias(AccountID const & aliasID, AccountID const &accountID, Database & db)
	{
		LedgerKey key;
		key.type(ALIAS);
		key.alias().aliasID = aliasID;
		key.alias().accountID = accountID;

		if (cachedEntryExists(key, db))
		{
			auto p = getCachedEntry(key, db);
			return p ? std::make_shared<AliasFrame>(*p) : nullptr;
		}

		AliasFrame::pointer res = std::make_shared<AliasFrame>();
		AliasEntry& alias = res->getAlias();

		std::string sql = aliasColumSelector;
		std::string aliasIDStrKey = KeyUtils::toStrKey(aliasID);
		std::string accountIDSourceStrKey = KeyUtils::toStrKey(accountID);
		std::string aliasIdStr, accountIdStr;
		sql += " WHERE aliasid = :v";

		auto prep = db.getPreparedStatement(sql);

		using soci::use;
		using soci::into;
		soci::statement& st = prep.statement();
		st.exchange(use(aliasIDStrKey, "v"));
		st.exchange(into(accountIdStr));
		st.exchange(into(aliasIdStr));
		st.exchange(into(res->getLastModified()));
		st.define_and_bind();
		{
			auto timer = db.getSelectTimer("alias");
			st.execute(true);
		}

		if (!st.got_data())
		{
			putCachedEntry(key, nullptr, db);
			return nullptr;
		}
		alias.aliasID = KeyUtils::fromStrKey<PublicKey>(aliasIdStr);
		alias.accountID = KeyUtils::fromStrKey<PublicKey>(accountIdStr);

		assert(res->isValid());
		res->mKeyCalculated = false;
		res->putCachedEntry(db);
		return res;
	}

	bool AliasFrame::isValid() const {
		return isValid(mAlias);
	}

	AliasEntry & AliasFrame::getAlias()
	{
		clearCached();
		return mAlias;
	}

	void AliasFrame::dropAll(Database & db)
	{
		db.getSession() << "DROP TABLE IF EXISTS aliases;";
		db.getSession() << kSQLCreateStatement1;
	}
}