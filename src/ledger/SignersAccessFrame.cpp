#include "SignersAccessFrame.h"
#include "ledger/LedgerDelta.h"
#include "crypto/Hex.h"
#include "crypto/KeyUtils.h"
#include "crypto/SecretKey.h"
#include "crypto/SignerKey.h"
#include "database/Database.h"
#include "ledger/LedgerManager.h"
#include "lib/util/format.h"
#include "util/basen.h"
#include "util/types.h"
#include <algorithm>

using namespace std;
using namespace soci;

//you have to rework all of this
//it seems to you that you started to understand
//but it may not be so
//ask Edik for help or continue struggling this code
//you're getting closer to understanding all of it

namespace stellar
{
    const char* SignersAccessFrame::kSQLCreateStatement1 =
            "CREATE TABLE signersaccess"
            "("
            "accessgiverid VARCHAR(56),"
            "accesstakerid VARCHAR(56),"
            "lastmodified INT NOT NULL,"
            "PRIMARY KEY(accessgiverid, accesstakerid)"
            ");";

    static const char* signersAccessColumnSelector =
            "SELECT accessgiverid, accesstakerid, lastmodified FROM signersaccess";

    SignersAccessFrame::SignersAccessFrame() : EntryFrame(SIGNERS_ACCESS), mSignersAccessEntry(mEntry.data.signersAccess())
    {
    }

    SignersAccessFrame::SignersAccessFrame(LedgerEntry const& from)
            : EntryFrame(from), mSignersAccessEntry(mEntry.data.signersAccess())
    {
    }

    SignersAccessFrame::SignersAccessFrame(SignersAccessFrame const& from) : SignersAccessFrame(from.mEntry)
    {
    }

    SignersAccessFrame::SignersAccessFrame(AccountID const& giverID, AccountID const& friendID) : SignersAccessFrame()
    {
        mSignersAccessEntry.accessGiverID = giverID;
        mSignersAccessEntry.accessTakerID = friendID;
    }

    SignersAccessFrame&
    SignersAccessFrame::operator=(SignersAccessFrame const& other)
    {
        if (&other != this)
        {
            mSignersAccessEntry = other.mSignersAccessEntry;
            mKey = other.mKey;
            mKeyCalculated = other.mKeyCalculated;
        }
        return *this;
    }

    AccountID const&
    SignersAccessFrame::getAccessGiverID() const
    {
        return mSignersAccessEntry.accessGiverID;
    }

    AccountID const&
    SignersAccessFrame::getAccessTakerID() const
    {
        return mSignersAccessEntry.accessTakerID;
    }

    bool
    SignersAccessFrame::isValid()
    {
        auto const& sa = mSignersAccessEntry;
        return !(sa.accessGiverID == sa.accessTakerID);
    }

    void
    SignersAccessFrame::storeUpdateHelper(LedgerDelta& delta, Database& db, bool insert)
    {
        assert(isValid());

        auto key = getKey();
        flushCachedEntry(key, db);

        touch(delta);

        std::string accessGiverIDStrKey = KeyUtils::toStrKey(mSignersAccessEntry.accessGiverID);
        std::string accessTakerIDStrKey = KeyUtils::toStrKey(mSignersAccessEntry.accessTakerID);

        std::string sql;

        if (insert)
        {
            sql = std::string(
                    "INSERT INTO signersaccess "
                    "( accessgiverid, accesstakerid, lastmodified )"
                    "VALUES ( :agid, :atid, :l )");
        }
        else
        {
            sql = std::string(
                    "UPDATE signersaccess "
                    "SET accesstakerid = :atid WHERE accessgiverid = :agid");
        }

        auto t = getLastModified();

        auto prep = db.getPreparedStatement(sql);
        {
            using soci::use;

            soci::statement& st = prep.statement();
            st.exchange(use(accessGiverIDStrKey, "agid"));
            st.exchange(use(accessTakerIDStrKey, "atid"));
            st.exchange(use(t, "l"));
            st.define_and_bind();
            {
                auto timer = insert ? db.getInsertTimer("signersaccess")
                                    : db.getUpdateTimer("signersaccess");
                st.execute(true);
            }
            if (st.get_affected_rows() != 1)
            {
                throw std::runtime_error("Could not update data in SQL");
            }
            if (insert)
            {
                delta.addEntry(*this);
            }
            else
            {
                delta.modEntry(*this);
            }
        }
    }

    void
    SignersAccessFrame::storeDelete(LedgerDelta& delta, Database& db, LedgerKey const& key)
    {
        flushCachedEntry(key, db);

        std::string accessGiverIDStrKey = KeyUtils::toStrKey(key.signersAccess().accessGiverID);
        std::string accessTakerIDStrKey = KeyUtils::toStrKey(key.signersAccess().accessTakerID);

        auto timer = db.getDeleteTimer("signersaccess");
        auto prep = db.getPreparedStatement( "DELETE FROM signersaccess"
                                             "WHERE accessgiverid= :agid AND accesstakerid= :atid");

        auto& st = prep.statement();

        st.exchange(soci::use(accessGiverIDStrKey));
        st.exchange(soci::use(accessTakerIDStrKey));
        st.define_and_bind();

        st.execute(true);
        delta.deleteEntry(key);
    }

    void
    SignersAccessFrame::storeChange(LedgerDelta& delta, Database& db)
    {
        storeUpdateHelper(delta, db, false);
    }

    void
    SignersAccessFrame::storeAdd(LedgerDelta& delta, Database& db)
    {
        storeUpdateHelper(delta, db, true);
    }

    void
    SignersAccessFrame::storeDelete(LedgerDelta& delta, Database& db) const
    {
        storeDelete(delta, db, getKey());
    }

    bool
    SignersAccessFrame::exists(Database& db, LedgerKey const& key)
    {
        if (cachedEntryExists(key, db) && getCachedEntry(key, db) != nullptr)
        {
            return true;
        }

        std::string accessGiverIDStrKey = KeyUtils::toStrKey(key.signersAccess().accessGiverID);
        std::string accessTakerIDStrKey = KeyUtils::toStrKey(key.signersAccess().accessTakerID);

        int exists = 0;
        {
            auto timer = db.getSelectTimer("signersaccess-exists");
            auto prep =
                    db.getPreparedStatement("SELECT EXISTS (SELECT NULL FROM signersaccess "
                            "WHERE accessgiverid=:agid AND accesstakerid=:atid");
            auto& st = prep.statement();
            st.exchange(use(accessGiverIDStrKey));
            st.exchange(use(accessTakerIDStrKey));
            st.exchange(into(exists));
            st.define_and_bind();
            st.execute(true);
        }
        return exists != 0;
    }

    uint64_t
    SignersAccessFrame::countObjects(soci::session& sess)
    {
        uint64_t count = 0;
        sess << "SELECT COUNT(*) FROM signersaccess;", into(count);
        return count;
    }

    SignersAccessFrame::pointer
    SignersAccessFrame::loadSignersAccess(AccountID const& accessGiverID, AccountID const& accessTakerID,
                                          Database& db)
    {
        LedgerKey key;
        key.type(SIGNERS_ACCESS);
        key.signersAccess().accessGiverID = accessGiverID;
        key.signersAccess().accessTakerID = accessTakerID;
        if (cachedEntryExists(key, db))
        {
            auto p = getCachedEntry(key, db);
            return p ? std::make_shared<SignersAccessFrame>(*p) : nullptr;
        }

        std::string accessGiverIDStrKey = KeyUtils::toStrKey(accessGiverID);
        std::string accessTakerIDStrKey = KeyUtils::toStrKey(accessTakerID);


        SignersAccessFrame::pointer res = make_shared<SignersAccessFrame>(accessGiverID, accessTakerID);
        SignersAccessEntry& signersAccess = res->getSignersAccess();

        auto prep =
                db.getPreparedStatement("SELECT lastmodified FROM signersaccess "
                                                "WHERE accessgiverid=:agid AND accesstakerid=:atid");
        auto& st = prep.statement();

        st.exchange(into(res->getLastModified()));

        st.exchange(use(accessGiverIDStrKey));
        st.exchange(use(accessTakerIDStrKey));

        st.define_and_bind();

        signersAccess.accessGiverID = accessGiverID;
        signersAccess.accessTakerID = accessTakerID;
        {
            auto timer = db.getSelectTimer("signersaccess");
            st.execute(true);
        }

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

    SignersAccessFrame::pointer
    SignersAccessFrame::loadSignersAccess(AccountID const& accessGiverID, AccountID const& accessTakerID,
                                          Database& db, LedgerDelta* delta)
    {
        LedgerKey key;
        key.type(SIGNERS_ACCESS);
        key.signersAccess().accessGiverID = accessGiverID;
        key.signersAccess().accessTakerID = accessTakerID;
        if (cachedEntryExists(key, db))
        {
            auto p = getCachedEntry(key, db);
            if(!p)
            {
                return nullptr;
            }
            pointer ret = std::make_shared<SignersAccessFrame>(*p);
            if(delta)
            {
                delta->recordEntry(*ret);
            }
            return ret;
        }

        std::string accessGiverIDStrKey = KeyUtils::toStrKey(accessGiverID);
        std::string accessTakerIDStrKey = KeyUtils::toStrKey(accessTakerID);

        std::string accessGiverKey, accessTakerKey;

        SignersAccessFrame::pointer res = make_shared<SignersAccessFrame>(accessGiverID, accessTakerID);
        SignersAccessEntry& signersAccess = res->getSignersAccess();


        auto prep =
                db.getPreparedStatement("SELECT accessgiverid, accesstakerid, lastmodified "
                                                "FROM signersaccess WHERE accessgiverid=:agid AND accesstakerid=:atid");
        auto& st = prep.statement();
        st.exchange(into(accessGiverIDStrKey));
        st.exchange(into(accessTakerIDStrKey));
        st.exchange(into(res->getLastModified()));

        st.exchange(use(accessGiverIDStrKey));
        st.exchange(use(accessTakerIDStrKey));


        st.define_and_bind();

        signersAccess.accessGiverID = accessGiverID;
        signersAccess.accessTakerID = accessTakerID;

        pointer retSignersAccess;
        auto timer = db.getSelectTimer("debit");
        loadSignersAccesses(prep, [&retSignersAccess](LedgerEntry const& debit) {
            retSignersAccess = make_shared<SignersAccessFrame>(debit);
        });

        if (retSignersAccess)
        {
            retSignersAccess->putCachedEntry(db);
        }
        else
        {
            putCachedEntry(key, nullptr, db);
        }

        if (delta && retSignersAccess)
        {
            delta->recordEntry(*retSignersAccess);
        }
        return retSignersAccess;
    }

    void
    SignersAccessFrame::loadSignersAccesses(StatementContext& prep,
                                            std::function<void(LedgerEntry const&)> signersAccessProcessor)
    {

        std::string accessGiverIDStrKey;
        std::string accessTakerIDStrKey;

        soci::indicator accessGiverIDIndicator;
        soci::indicator accessTakerIDIndicator;

        LedgerEntry le;
        le.data.type(SIGNERS_ACCESS);

        SignersAccessEntry& signersAccessEntry = le.data.signersAccess();

        statement& st = prep.statement();

        st.exchange(into(accessGiverIDStrKey, accessGiverIDIndicator));
        st.exchange(into(accessTakerIDStrKey, accessTakerIDIndicator));
        st.exchange(into(le.lastModifiedLedgerSeq));
        st.define_and_bind();
        st.execute(true);

        while (st.got_data())
        {
            signersAccessEntry.accessGiverID = KeyUtils::fromStrKey<PublicKey>(accessGiverIDStrKey);
            signersAccessEntry.accessTakerID = KeyUtils::fromStrKey<PublicKey>(accessTakerIDStrKey);

            if (accessGiverIDIndicator != soci::i_ok || accessTakerIDIndicator != soci::i_ok)
            {
                throw std::runtime_error("bad database state");
            }
            signersAccessProcessor(le);
            st.fetch();
        }
    }

    void
    SignersAccessFrame::loadSignersAccesses(AccountID const& accessGiverID,
                                   std::vector<SignersAccessFrame::pointer>& retSignersAccesses,
                                   Database& db)
    {
        std::string accessGiverIDStrKey = KeyUtils::toStrKey(accessGiverID);

        auto query = std::string(signersAccessColumnSelector);
        query += (" WHERE accessgiverid = :agid ");
        auto prep = db.getPreparedStatement(query);
        auto& st = prep.statement();
        st.exchange(use(accessGiverIDStrKey));

        auto timer = db.getSelectTimer("signersaccess");
        loadSignersAccesses(prep, [&retSignersAccesses](LedgerEntry const& cur){
            retSignersAccesses.emplace_back(make_shared<SignersAccessFrame>(cur));
        });
    }

    std::unordered_map<AccountID, std::vector<SignersAccessFrame::pointer>>
    SignersAccessFrame::loadAllSignersAccesses(Database& db)
    {
        std::unordered_map<AccountID, std::vector<SignersAccessFrame::pointer>> retSignersAccess;

        std::string sql = std::string(signersAccessColumnSelector);
        sql += " ORDER BY accessgiverdid";
        auto prep = db.getPreparedStatement(sql);

        auto timer = db.getSelectTimer("signersaccess");

        loadSignersAccesses(prep, [&retSignersAccess](LedgerEntry const& of) {
            auto& thisUserSignersAccesses = retSignersAccess[of.data.signersAccess().accessGiverID];
            thisUserSignersAccesses.emplace_back(make_shared<SignersAccessFrame>(of));
        });
        return retSignersAccess;
    }

    void SignersAccessFrame::dropAll(Database& db)
    {
        db.getSession() << "DROP TABLE IF EXISTS signersaccess;";
        db.getSession() << kSQLCreateStatement1;
    }
}