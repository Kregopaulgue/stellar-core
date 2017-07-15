#include "ManageAliasOpFrame.h"
#include "ledger/AliasFrame.h"

#include "main/Application.h"
#include "medida/meter.h"
#include "medida/metrics_registry.h"

bool stellar::ManageAliasOpFrame::checkExistAccountWithIdAlias(AccountID const & accountID)
{

	return false;
}

stellar::ManageAliasOpFrame::ManageAliasOpFrame(Operation const & op
	, OperationResult & res
	, TransactionFrame & parentTx)
	: OperationFrame(op, res, parentTx)
	, mManageAlias(mOperation.body.manageAliasOp())
{
}

bool stellar::ManageAliasOpFrame::doApply(Application & app, LedgerDelta & delta, LedgerManager & ledgerManager)
{
	AliasFrame::pointer destAccount;
	Database& db = ledgerManager.getDatabase();

	if (mManageAlias.accountId == mManageAlias.sourceId) {

	}

	if (mSourceAccount->getID() == mManageAlias.accountId) {
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "not-exist-account" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_ALREAY_EXIST_ACCOUNT);
		return false;
	}

	if (!(mSourceAccount->getID() == mManageAlias.sourceId)) 
	{ //delete this
		auto deleteAlias = AliasFrame::loadAlias(mManageAlias.accountId, mSourceAccount->getID(), db);
		if (!deleteAlias) {
			app.getMetrics()
				.NewMeter({ "op-create-alias", "failure", "not-owner-account" },
					"operation")
				.Mark();
			innerResult().code(MANAGE_ALIAS_NOT_OWNER);
			return false;
		}
		
		mSourceAccount->addNumEntries(-1, ledgerManager);
		mSourceAccount->storeChange(delta, db);
		deleteAlias->storeDelete(delta, db);
		
		app.getMetrics()
			.NewMeter({ "op-create-alias", "success", "apply" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_SUCCESS);
		return true;
	}

	auto alias = AliasFrame::loadAlias(mManageAlias.accountId, mManageAlias.sourceId, db);
	auto account = AccountFrame::loadAccount(mManageAlias.accountId, db);
	auto needAccount = AccountFrame::loadAccount(delta, mManageAlias.sourceId, db);

	if (!needAccount) {
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "not-exist-account" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_ALREAY_EXIST_ACCOUNT);
		return false;
	}

	if (alias) {
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "already-exist" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_ALREADY_EXIST);
		return false;
	}

	if (account) {
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "already-exist-account" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_ALREAY_EXIST_ACCOUNT);
		return false;
	}

	if (!mSourceAccount->addNumEntries(1, ledgerManager))
	{
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "low-reserve" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_UNDERFUNDED);
		return false;
	}

	alias = std::make_shared<AliasFrame>();
	alias->getAlias().accountSourceID = mManageAlias.sourceId;
	alias->getAlias().accountID = mManageAlias.accountId;
	alias->storeAdd(delta, db);
	mSourceAccount->storeChange(delta, db);
	
	app.getMetrics()
		.NewMeter({ "op-create-alias", "success", "apply" },
			"operation")
		.Mark();
	innerResult().code(MANAGE_ALIAS_SUCCESS);
	return true;
}

bool stellar::ManageAliasOpFrame::doCheckValid(Application & app)
{
	return true;
}
