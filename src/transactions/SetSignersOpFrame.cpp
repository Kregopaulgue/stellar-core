//
// Created by krego on 06.09.17.
//

#include "util/asio.h"
#include "OperationFrame.h"
#include "TransactionFrame.h"
#include "SetSignersOpFrame.h"
#include "database/Database.h"
#include "ledger/SignersAccessFrame.h"
#include "ledger/LedgerDelta.h"
#include "main/Application.h"
#include "medida/meter.h"
#include "medida/metrics_registry.h"

namespace stellar
{

using namespace std;
using xdr::operator==;

SetSignersOpFrame::SetSignersOpFrame(Operation const &op, OperationResult &res, TransactionFrame &parentTx)
        : OperationFrame(op, res, parentTx)
        , mSetSigners(mOperation.body.setSignersOp())
{
}

bool
SetSignersOpFrame::doApply(Application& app, LedgerDelta& delta,
                           LedgerManager& ledgerManager) {

    AccountID accessGiverID, accessTakerID;

    accessGiverID = *(mSetSigners.accessGiverID);
    accessTakerID = mSourceAccount->getID();

    SignersAccessFrame::pointer signersAccess;

    Database &db = ledgerManager.getDatabase();

    signersAccess =
            SignersAccessFrame::loadSignersAccess(accessGiverID, accessTakerID, db);

    if (!signersAccess) {
        app.getMetrics()
                .NewMeter({"op-get-signers", "failure", "access-entry-doesnt-exist"},
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_ACCESS_ENTRY_DOESNT_EXIST);
        return false;
    }


    AccountFrame::pointer accessGiverAccount;
    accessGiverAccount =
            AccountFrame::loadAccount(delta, accessGiverID, db);

    if (!accessGiverAccount)
    {
        app.getMetrics()
                .NewMeter({ "op-set-signers", "failure", "access-giver-doesnt-exist" },
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_ACCESS_GIVER_DOESNT_EXIST);
        return false;
    }

    AccountEntry& account = accessGiverAccount->getAccount();
    auto& signers = account.signers;

    unsigned long startSignersAmount = signers.size();
    signers.clear();
    account.thresholds[0] = 0; //do we make the master signer zero weight?
    accessGiverAccount->addNumEntries(-(startSignersAmount), ledgerManager);


    if(!accessGiverAccount->addNumEntries(1, ledgerManager))
    {
        app.getMetrics()
                .NewMeter({"op-set-options", "failure", "low-reserve"},
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_LOW_RESERVE);
        return false;
    }

    Signer& signerToAdd = *mSetSigners.signer;
    account.signers.push_back(*mSetSigners.signer);
    accessGiverAccount->setUpdateSigners();


    app.getMetrics()
            .NewMeter({ "op-set-signers", "success", "apply" },
                      "operation")
            .Mark();
    innerResult().code(SET_SIGNERS_SUCCESS);
    accessGiverAccount->storeChange(delta, db);
    return true;
}

bool
SetSignersOpFrame::doCheckValid(Application& app)
{
    AccountID accessGiverID = *(mSetSigners.accessGiverID);
    AccountID accessTakerID = getSourceID();

    if (accessGiverID == accessTakerID)
    {
        app.getMetrics()
                .NewMeter({ "op-set-signers", "failure", "friend-is-source" },
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_FRIEND_IS_SOURCE);
        return false;
    }
}
}