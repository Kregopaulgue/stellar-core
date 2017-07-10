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

	if (mCreateAlias.accountId == mCreateAlias.sourceId) {

	}

	if (mSourceAccount->getID() == mCreateAlias.accountId) {
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "not-exist-account" },
				"operation")
			.Mark();
		innerResult().code(CREATE_ALIAS_ALREAY_EXIST_ACCOUNT);
		return false;
	}

	if (!(mSourceAccount->getID() == mCreateAlias.sourceId)) { //delete this
		auto deleteAlias = AliasFrame::loadAlias(mCreateAlias.accountId, mSourceAccount->getID(), db);
		if (!deleteAlias) {
			app.getMetrics()
				.NewMeter({ "op-create-alias", "failure", "not-owner-account" },
					"operation")
				.Mark();
			innerResult().code(CREATE_ALIAS_NOT_OWNER);
			return false;
		}
		
		mSourceAccount->addNumEntries(-1, ledgerManager);
		mSourceAccount->storeChange(delta, db);
		deleteAlias->storeDelete(delta, db);
		
		app.getMetrics()
			.NewMeter({ "op-create-alias", "success", "apply" },
				"operation")
			.Mark();
		innerResult().code(CREATE_ALIAS_SUCCESS);
		return true;
	}

	auto alias = AliasFrame::loadAlias(mCreateAlias.accountId, mCreateAlias.sourceId, db);
	auto account = AccountFrame::loadAccount(mCreateAlias.accountId, db);
	auto needAccount = AccountFrame::loadAccount(delta, mCreateAlias.sourceId, db);

	if (!needAccount) {
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "not-exist-account" },
				"operation")
			.Mark();
		innerResult().code(CREATE_ALIAS_ALREAY_EXIST_ACCOUNT);
		return false;
	}

	if (alias) {
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "already-exist" },
				"operation")
			.Mark();
		innerResult().code(CREATE_ALIAS_ALREADY_EXIST);
		return false;
	}

	if (account) {
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "already-exist-account" },
				"operation")
			.Mark();
		innerResult().code(CREATE_ALIAS_ALREAY_EXIST_ACCOUNT);
		return false;
	}

	if (!mSourceAccount->addNumEntries(1, ledgerManager))
	{
		app.getMetrics()
			.NewMeter({ "op-create-alias", "failure", "low-reserve" },
				"operation")
			.Mark();
		innerResult().code(CREATE_ALIAS_UNDERFUNDED);
		return false;
	}

	alias = std::make_shared<AliasFrame>();
	alias->getAlias().accountSourceID = mCreateAlias.sourceId;
	alias->getAlias().accountID = mCreateAlias.accountId;
	alias->storeAdd(delta, db);
	mSourceAccount->storeChange(delta, db);
	
	app.getMetrics()
		.NewMeter({ "op-create-alias", "success", "apply" },
			"operation")
		.Mark();
	innerResult().code(CREATE_ALIAS_SUCCESS);
	return true;
}

bool stellar::CreateAliasOpFrame::doCheckValid(Application & app)
{
	return true;
}
