#include "ManageAliasOpFrame.h"
#include "ledger/AliasFrame.h"

#include "main/Application.h"
#include "medida/meter.h"
#include "medida/metrics_registry.h"

bool stellar::ManageAliasOpFrame::CreateAlias(Application& app, Database & db, LedgerDelta & delta, LedgerManager & ledgerManager)
{
	AccountID sourceAccountID = mSourceAccount->getAccount().accountID;
	auto alias = AliasFrame::loadAlias(mManageAlias.aliasID, sourceAccountID, db);
	if (alias) {
		app.getMetrics()
			.NewMeter({ "op-manage-alias", "failed", "alias-already-exist" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_ALREADY_EXIST);
		return false;
	}

	auto AccountWithAliasID = AccountFrame::loadAccount(mManageAlias.aliasID, db);

	if (AccountWithAliasID) {
		app.getMetrics()
			.NewMeter({ "op-manage-alias", "failed", "account-already-exist" },
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
	alias->getAlias().aliasID = mManageAlias.aliasID;
	alias->getAlias().accountID = sourceAccountID;
	alias->storeAdd(delta, db);
	mSourceAccount->storeChange(delta, db);

	app.getMetrics()
		.NewMeter({ "op-create-alias", "success", "apply" },
			"operation")
		.Mark();
	innerResult().code(MANAGE_ALIAS_SUCCESS);

	return true;
}

bool stellar::ManageAliasOpFrame::DeleteAlias(Application& app, Database & db, LedgerDelta & delta, LedgerManager & ledgerManager)
{
	AccountID sourceAccountID = mSourceAccount->getAccount().accountID;
	auto deleteAlias = AliasFrame::loadAlias(mManageAlias.aliasID, sourceAccountID, db);

	if (!deleteAlias) {
		app.getMetrics()
			.NewMeter({ "op-manage-alias", "failed", "not-alias-exist" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_NOT_EXIST);
		return false;
	}

	if (deleteAlias->getAlias().accountID == sourceAccountID) {

	}
	else {
		app.getMetrics()
			.NewMeter({ "op-manage-alias", "failed", "not-alias-owner" },
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

	if (mManageAlias.isDelete)
	{
		return DeleteAlias(app, db, delta, ledgerManager);
	}
	else
	{
		return CreateAlias(app, db, delta, ledgerManager);
	}
}

bool stellar::ManageAliasOpFrame::doCheckValid(Application & app)
{
	/*AccountID sourceAccountID ;

	if (mManageAlias.aliasID == sourceAccountID) {
		app.getMetrics()
			.NewMeter({ "op-manage-alias", "failure", "exist-account" },
				"operation")
			.Mark();
		innerResult().code(MANAGE_ALIAS_ALREAY_EXIST_ACCOUNT);
		return false;
	}*/
	return true;
}
