//
// Created by krego on 06.09.17.
//

#ifndef STELLAR_SETSIGNERSOPFRAME_H
#define STELLAR_SETSIGNERSOPFRAME_H

#endif //STELLAR_SETSIGNERSOPFRAME_H

#pragma once

#include "transactions/OperationFrame.h"
#include "TransactionFrame.h"

namespace stellar
{
class SetSignersOpFrame : public OperationFrame
{
    SetSignersResult&
    innerResult()
    {
        return mResult.tr().setSignersResult();
    }

    SetSignersOp const& mSetSigners;

public:
    SetSignersOpFrame(Operation const& op, OperationResult& res,
                      TransactionFrame& parentTx);

    bool doApply(Application& app, LedgerDelta& delta,
                 LedgerManager& ledgerManager) override;
    bool doCheckValid(Application& app) override;

    static SetSignersResultCode
    getInnerCode(OperationResult const& res)
    {
        return res.tr().setSignersResult().code();
    }
};
}