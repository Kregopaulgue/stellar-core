// Copyright 2014 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "util/asio.h"
#include "transactions/CreateAccountOpFrame.h"
#include "OfferExchange.h"
#include "database/Database.h"
#include "ledger/LedgerDelta.h"
#include "ledger/OfferFrame.h"
#include "ledger/TrustFrame.h"
#include "ledger/AliasFrame.h"
#include "util/Logging.h"
#include <algorithm>

#include "main/Application.h"
#include "medida/meter.h"
#include "medida/metrics_registry.h"

namespace stellar
{

	using namespace std;
	using xdr::operator==;

	CreateAccountOpFrame::CreateAccountOpFrame(Operation const& op,
		OperationResult& res,
		TransactionFrame& parentTx)
		: OperationFrame(op, res, parentTx)
		, mCreateAccount(mOperation.body.createAccountOp())
	{
	}

	bool
		CreateAccountOpFrame::doApply(Application& app, LedgerDelta& delta,
			LedgerManager& ledgerManager)
	{
		AccountFrame::pointer destAccount;

		Database& db = ledgerManager.getDatabase();


		bool isReverseId = AliasFrame::isAliasIdExist(mCreateAccount.destination, db);

		if (isReverseId) {
			app.getMetrics()
				.NewMeter({ "op-create-account", "failure", "already-exist" },
					"operation")
				.Mark();
			innerResult().code(CREATE_ACCOUNT_ALREADY_EXIST);
			return false;
		}

		destAccount =
			AccountFrame::loadAccount(delta, mCreateAccount.destination, db);

		if (!destAccount)
		{
			if (mCreateAccount.startingBalance < ledgerManager.getMinBalance(0))
			{ // not over the minBalance to make an account
				app.getMetrics()
					.NewMeter({ "op-create-account", "failure", "low-reserve" },
						"operation")
					.Mark();
				innerResult().code(CREATE_ACCOUNT_LOW_RESERVE);
				return false;
			}
			else
			{
				int64_t minBalance =
					mSourceAccount->getMinimumBalance(ledgerManager);

				if ((mSourceAccount->getAccount().balance - minBalance) <
					mCreateAccount.startingBalance)
				{ // they don't have enough to send
					app.getMetrics()
						.NewMeter({ "op-create-account", "failure", "underfunded" },
							"operation")
						.Mark();
					innerResult().code(CREATE_ACCOUNT_UNDERFUNDED);
					return false;
				}

				mSourceAccount->getAccount().balance -=
					mCreateAccount.startingBalance;
				mSourceAccount->storeChange(delta, db);

				destAccount = make_shared<AccountFrame>(mCreateAccount.destination);
				destAccount->getAccount().seqNum =
					delta.getHeaderFrame().getStartingSequenceNumber();
				destAccount->getAccount().balance = mCreateAccount.startingBalance;

				destAccount->storeAdd(delta, db);

				app.getMetrics()
					.NewMeter({ "op-create-account", "success", "apply" },
						"operation")
					.Mark();
				innerResult().code(CREATE_ACCOUNT_SUCCESS);
				return true;
			}
		}
		else
		{
			app.getMetrics()
				.NewMeter({ "op-create-account", "failure", "already-exist" },
					"operation")
				.Mark();
			innerResult().code(CREATE_ACCOUNT_ALREADY_EXIST);
			return false;
		}
	}

	bool
		CreateAccountOpFrame::doCheckValid(Application& app)
	{
		if (mCreateAccount.startingBalance <= 0)
		{
			app.getMetrics()
				.NewMeter(
			{ "op-create-account", "invalid", "malformed-negative-balance" },
					"operation")
				.Mark();
			innerResult().code(CREATE_ACCOUNT_MALFORMED);
			return false;
		}



		if (mCreateAccount.destination == getSourceID())
		{
			app.getMetrics()
				.NewMeter({ "op-create-account", "invalid",
						   "malformed-destination-equals-source" },
					"operation")
				.Mark();
			innerResult().code(CREATE_ACCOUNT_MALFORMED);
			return false;
		}

		return true;
	}

	bool CreateAccountOpFrame::addTrustLine(AccountFrame * account, const char* name, Application& app, LedgerDelta& delta,
		LedgerManager& ledgerManager)
	{
		//if (!addTrustLine(destAccount.get(), "UKD", app, delta, ledgerManager)) 
		Database& db = ledgerManager.getDatabase();
		Asset aset;
		aset.type(ASSET_TYPE_CREDIT_ALPHANUM4);
		strToAssetCode(aset.alphaNum4().assetCode, name);

		auto trustLine = std::make_shared<TrustFrame>();
		auto& tl = trustLine->getTrustLine();
		tl.accountID = account->getID();
		tl.asset = aset;
		tl.limit = 200;
		tl.balance = 11;
		trustLine->setAuthorized(!account->isAuthRequired());

		if (!account->addNumEntries(1, ledgerManager))
		{
			app.getMetrics()
				.NewMeter({ "op-change-account", "failure", "low-reserve" },
					"operation")
				.Mark();
			innerResult().code(CREATE_ACCOUNT_UNDERFUNDED);
			return false;
		}

		account->storeChange(delta, db);
		trustLine->storeAdd(delta, db);

		return true;
	}
}
