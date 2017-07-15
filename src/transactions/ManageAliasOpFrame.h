#pragma once

#include "transactions\OperationFrame.h"
#include "util/Logging.h"

namespace stellar {
	class ManageAliasOpFrame : public OperationFrame {
	private:
		ManageAliasResult&
			innerResult()
		{
			return mResult.tr().manageAliasResult();
		}

		bool checkExistAccountWithIdAlias(AccountID const& accountID); //delete this
		ManageAliasOp const& mManageAlias;
	public:
		ManageAliasOpFrame(Operation const& op, OperationResult& res,
			TransactionFrame& parentTx);

		bool doApply(Application& app, LedgerDelta& delta,
			LedgerManager& ledgerManager) override;
		bool doCheckValid(Application& app) override;

		static ManageAliasResultCode
			getInnerCode(OperationResult const& res)
		{
			return res.tr().manageAliasResult().code();
		}

	};
}