#include "CreateAliasOpFrame.h"

stellar::CreateAliasOpFrame::CreateAliasOpFrame(Operation const & op
	, OperationResult & res
	, TransactionFrame & parentTx)
	: OperationFrame(op, res, parentTx)
	, mCreateAlias(mOperation.body.createAliasOp())
{
}

bool stellar::CreateAliasOpFrame::doApply(Application & app, LedgerDelta & delta, LedgerManager & ledgerManager)
{

	return false;
}

bool stellar::CreateAliasOpFrame::doCheckValid(Application & app)
{
	return false;
}
