#pragma once

#include "transactions\OperationFrame.h"

namespace stellar {
	class CreateAliasOpFrame : public OperationFrame {
	private:
		CreateAliasResult&
			innerResult()
		{
			return mResult.tr().createAliasResult();
		}
		CreateAliasOp const& mCreateAlias;
	public:
		CreateAliasOpFrame(Operation const& op, OperationResult& res,
			TransactionFrame& parentTx);

		bool doApply(Application& app, LedgerDelta& delta,
			LedgerManager& ledgerManager) override;
		bool doCheckValid(Application& app) override;

		static CreateAliasResultCode
			getInnerCode(OperationResult const& res)
		{
			return res.tr().createAliasResult().code();
		}

	};
}