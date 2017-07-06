#include "CreateAliasOpFrame.h"
#include "ledger/AliasFrame.h"

#include "main/Application.h"
#include "medida/meter.h"
#include "medida/metrics_registry.h"

bool stellar::CreateAliasOpFrame::checkExistAccountWithIdAlias(AccountID const & accountID)
{
	
	return false;
}

stellar::CreateAliasOpFrame::CreateAliasOpFrame(Operation const & op
	, OperationResult & res
	, TransactionFrame & parentTx)
	: OperationFrame(op, res, parentTx)
	, mCreateAlias(mOperation.body.createAliasOp())
{
}

bool stellar::CreateAliasOpFrame::doApply(Application & app, LedgerDelta & delta, LedgerManager & ledgerManager)
{
	AliasFrame::pointer destAccount;
	Database& db = ledgerManager.getDatabase();
	auto alias = AliasFrame::loadAlias(mCreateAlias.accountId, mCreateAlias.sourceId, db);

	if (!alias) {
		alias = std::make_shared<AliasFrame>();
		alias->getAlias().accountSourceID = mSourceAccount->getID();
		alias->getAlias().accountID = mCreateAlias.accountId;
		alias->storeAdd(delta,db);
		app.getMetrics()
			.NewMeter({ "op-create-alias", "success", "apply" },
				"operation")
			.Mark();
		innerResult().code(CREATE_ALIAS_SUCCESS);
		return true;
	}
	else {
		app.getMetrics()
			.NewMeter({ "op-create-account", "failure", "low-reserve" },
				"operation")
			.Mark();
		innerResult().code(CREATE_ALIAS_UNDERFUNDED);
	}

	return false;
}

bool stellar::CreateAliasOpFrame::doCheckValid(Application & app)
{
	return true;
}
