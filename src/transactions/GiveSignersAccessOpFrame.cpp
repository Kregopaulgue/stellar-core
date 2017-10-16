//
// Created by krego on 06.09.17.
//

#include "util/asio.h"
#include "GiveSignersAccessOpFrame.h"
#include "TransactionFrame.h"
#include "OperationFrame.h"
#include "database/Database.h"
#include "ledger/SignersAccessFrame.h"
#include "ledger/LedgerDelta.h"
#include "ledger/OfferFrame.h"
#include "ledger/TrustFrame.h"
#include "main/Application.h"
#include "medida/meter.h"
#include "medida/metrics_registry.h"
#include "transactions/PathPaymentOpFrame.h"
#include "util/Logging.h"
#include <algorithm>

namespace stellar
{

using namespace std;
using xdr::operator==;

GiveSignersAccessOpFrame::GiveSignersAccessOpFrame(Operation const &op, OperationResult &res,
                                                   TransactionFrame &parentTx)
    : OperationFrame(op, res, parentTx), mGiveSignersAccess(mOperation.body.giveSignersAccessOp())
{
}

bool
GiveSignersAccessOpFrame::doApply(Application &app, LedgerDelta &delta, LedgerManager &ledgerManager)
{
    AccountID accessGiverID, accessTakerID;
    int64 uploadTimeFrames;

    SignersAccessFrame::pointer signersAccess;

    accessGiverID = mSourceAccount->getID();
    accessTakerID = mGiveSignersAccess.friendID;
    uploadTimeFrames = mGiveSignersAccess.timeFrames;

    Database& db = ledgerManager.getDatabase();
    AccountFrame::pointer accessTaker;
    accessTaker =
        AccountFrame::loadAccount(delta, accessTakerID, db);

    if (!accessTaker)
    {
        app.getMetrics()
                .NewMeter({ "op-give-signers-access", "failure", "friend-account-doesnt-exist" },
                          "operation")
                .Mark();
        innerResult().code(GIVE_SIGNERS_ACCESS_FRIEND_DOESNT_EXIST);
        return false;
    }

    //check here
    signersAccess = make_shared<SignersAccessFrame>(accessGiverID, accessTakerID, uploadTimeFrames);

    signersAccess->storeAdd(delta, db);

    app.getMetrics()
            .NewMeter({ "op-give-signers-access", "success", "apply" },
                      "operation")
            .Mark();
    innerResult().code(GIVE_SIGNERS_ACCESS_SUCCESS);
    return true;
}

bool
GiveSignersAccessOpFrame::doCheckValid(Application& app)
{

    if (mGiveSignersAccess.friendID == getSourceID())
    {
        app.getMetrics()
                .NewMeter({ "op-give-signers-access", "invalid",
                            "friend-is-source" },
                          "operation")
                .Mark();
        innerResult().code(GIVE_SIGNERS_ACCESS_FRIEND_IS_SOURCE);
        return false;
    }

    if (mGiveSignersAccess.timeFrames <= time(nullptr))
    {
        app.getMetrics()
                .NewMeter({ "op-give-signers-access", "invalid",
                            "friend-is-source" },
                          "operation")
                .Mark();
        innerResult().code(GIVE_SIGNERS_ACCESS_TIME_FRAMES_EQUAL_OR_LESS_THEN_CURRENT_TIME);
        return false;
    }

    SignersAccessFrame::pointer currentSignersAccess;
    currentSignersAccess =
            SignersAccessFrame::loadSignersAccess(getSourceID(), mGiveSignersAccess.friendID, app.getDatabase());

    if(currentSignersAccess) {
        app.getMetrics()
                .NewMeter({ "op-give-signers-access", "failure", "such-signers-access-entry-exists" },
                          "operation")
                .Mark();
        innerResult().code(GIVE_SIGNERS_ACCESS_SIGNERS_ACCESS_ALREADY_EXISTS);
        return false;
    }

    return true;
}
}