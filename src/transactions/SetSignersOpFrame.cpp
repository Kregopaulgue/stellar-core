//
// Created by krego on 06.09.17.
//

#include "util/asio.h"
#include "OperationFrame.h"
#include "TransactionFrame.h"
#include "SetSignersOpFrame.h"
#include "database/Database.h"
#include "ledger/AccountFrame.h"
#include "ledger/SignersAccessFrame.h"
#include "ledger/LedgerDelta.h"
#include "ledger/OfferFrame.h"
#include "ledger/TrustFrame.h"
#include "main/Application.h"
#include "medida/meter.h"
#include "medida/metrics_registry.h"
#include "transactions/PathPaymentOpFrame.h"
#include "util/Logging.h"
#include "crypto/KeyUtils.h"
#include <algorithm>

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
    AccountFrame::pointer accessGiverAccount;

    Database &db = ledgerManager.getDatabase();

    signersAccess =
            SignersAccessFrame::loadSignersAccess(accessGiverID, accessTakerID, db);
    accessGiverAccount =
            AccountFrame::loadAccount(delta, accessGiverID, db);

    if (!signersAccess) {
        app.getMetrics()
                .NewMeter({"op-get-signers", "failure", "access-entry-doesnt-exist"},
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_ACCESS_ENTRY_DOESNT_EXIST);
        return false;
    }

    if (!accessGiverAccount)
    {
        app.getMetrics()
                .NewMeter({ "op-set-signers", "failure", "access-giver-doesnt-exist" },
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_ACCESS_GIVER_DOESNT_EXIST);
        return false;
    }

    if (accessGiverID == accessTakerID)
    {
        app.getMetrics()
                .NewMeter({ "op-set-signers", "failure", "friend-is-source" },
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_FRIEND_IS_SOURCE);
        return false;
    }

    AccountEntry& account = accessGiverAccount->getAccount();
    auto& signers = account.signers;

    //deleting signers
    auto it = signers.begin();
    while (it != signers.end())
    {
        Signer& oldSigner = *it;
        it = signers.erase(it);
        accessGiverAccount->addNumEntries(-1, ledgerManager);
    }
    accessGiverAccount->setUpdateSigners();


    if(!accessGiverAccount->addNumEntries(1, ledgerManager))
    {
        app.getMetrics()
                .NewMeter({"op-set-options", "failure", "low-reserve"},
                          "operation")
                .Mark();
        innerResult().code(SET_SIGNERS_LOW_RESERVE);
        return false;
    }

    //adding signer
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