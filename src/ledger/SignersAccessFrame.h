//
// Created by krego on 06.09.17.
//


#ifndef STELLAR_SIGNERSACCESSFRAME_H
#define STELLAR_SIGNERSACCESSFRAME_H

#endif //STELLAR_SIGNERSACCESSFRAME_H

#pragma once

#include "ledger/EntryFrame.h"
#include <functional>
#include <map>
#include <unordered_map>

namespace soci
{
    class session;
}

namespace stellar
{
    class ManageSignersAccess;
    class StatementContext;

    class SignersAccessFrame : public EntryFrame
    {

        SignersAccessEntry& mSignersAccessEntry;

        bool isValid();

        void storeUpdateHelper(LedgerDelta& delta, Database& db, bool insert);

        static void loadSignersAccesses(StatementContext& prep,
                                        std::function<void(LedgerEntry const&)> signersAccessProcessor);

    public:
        typedef std::shared_ptr<SignersAccessFrame> pointer;

        SignersAccessFrame();
        SignersAccessFrame(LedgerEntry const& from);
        SignersAccessFrame(SignersAccessFrame const& from);
        SignersAccessFrame(AccountID const& accessGiverID, AccountID const& accessTakerID, int64 const& timeFrames);

        SignersAccessFrame& operator=(SignersAccessFrame const& other);

        EntryFrame::pointer
        copy() const override
        {
            return EntryFrame::pointer(new SignersAccessFrame(*this));
        }

        AccountID const& getAccessGiverID() const;
        AccountID const& getAccessTakerID() const;

        int64 const& getTimeFrames() const;

        SignersAccessEntry const&
        getSignersAccess() const
        {
            return mSignersAccessEntry;
        }
        SignersAccessEntry&
        getSignersAccess()
        {
            return mSignersAccessEntry;
        }

        void storeAdd(LedgerDelta& delta, Database& db) override;
        void storeChange(LedgerDelta& delta, Database& db) override;
        void storeDelete(LedgerDelta& delta, Database& db) const override;

        static void storeDelete(LedgerDelta& delta, Database& db,
                                LedgerKey const& key);
        static bool exists(Database& db, LedgerKey const& key);
        static uint64_t countObjects(soci::session& sess);


        static pointer
        loadSignersAccess(AccountID const& accessGiverID, AccountID const& accessTakerID,
                          Database& db, LedgerDelta* delta);

        static std::unordered_map<AccountID, std::vector<SignersAccessFrame::pointer>>
        loadAllSignersAccesses(Database& db);

        void
        loadSignersAccesses(AccountID const& owner,
                            std::vector<SignersAccessFrame::pointer>& retSignersAccesses,
                            Database& db);

        static SignersAccessFrame::pointer
        loadSignersAccess(AccountID const& accessGiverID, AccountID const& accessTakerID,
                          Database& db);

        static void dropAll(Database& db);
        static const char* kSQLCreateStatement1;
    };
}