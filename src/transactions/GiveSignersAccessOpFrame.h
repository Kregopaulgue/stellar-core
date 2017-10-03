//
// Created by krego on 06.09.17.
//

#ifndef STELLAR_GIVESIGNERSACCESSOPFRAME_H
#define STELLAR_GIVESIGNERSACCESSOPFRAME_H

#endif //STELLAR_GIVESIGNERSACCESSOPFRAME_H

#pragma once

#include "transactions/OperationFrame.h"


namespace stellar
{
    class GiveSignersAccessOpFrame : public OperationFrame
    {
        GiveSignersAccessResult&
        innerResult()
        {
            return mResult.tr().giveSignersAccessResult();
        }
        GiveSignersAccessOp const& mGiveSignersAccess;

    public:
        GiveSignersAccessOpFrame(Operation const& op, OperationResult& res,
                                 TransactionFrame& parentTx);

        bool doApply(Application& app, LedgerDelta& delta,
                     LedgerManager& ledgerManager) override;
        bool doCheckValid(Application& app) override;

        static GiveSignersAccessResultCode
        getInnerCode(OperationResult const& res)
        {
            return res.tr().giveSignersAccessResult().code();
        }
    };
}