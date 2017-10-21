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

    accessGiverID = mSetSigners.accessGiverID;
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

    if (signersAccess->getTimeFrames() <= time(nullptr)) {
        app.getMetrics()
                .NewMeter({"op-get-signers", "failure", "access-entry-doesnt-exist"},
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_CURRENT_TIME_NOT_WITHIN_ACCESS_TIME_FRAMES);
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

    unsigned long startSignersAmount = account.signers.size();
    account.signers.clear();


    /* Here we make master_account 'false'
     * for reason is when it has to pass through
     * check for master_account, it doesnt,
     * and master_account is not being added
     * to signers and doesn't sign a transactions */

    account.thresholds[0] = false;

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

    account.signers.push_back(mSetSigners.signer);
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
    AccountID accessGiverID = mSetSigners.accessGiverID;
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