#include "database/Database.h"
#include "ledger/LedgerManager.h"
#include "lib/catch.hpp"
#include "main/Application.h"
#include "main/Config.h"
#include "overlay/LoopbackPeer.h"
#include "test/TestAccount.h"
#include "test/TestExceptions.h"
#include "test/TestUtils.h"
#include "test/TxTests.h"
#include "test/test.h"
#include "transactions/ChangeTrustOpFrame.h"
#include "transactions/MergeOpFrame.h"
#include "transactions/PaymentOpFrame.h"
#include "util/Logging.h"
#include "util/Timer.h"
#include "util/make_unique.h"

using namespace stellar;
using namespace stellar::txtest;

typedef std::unique_ptr<Application> appPtr;

TEST_CASE("alias", "[tx][alias]") {
	Config const& cfg = getTestConfig();

	VirtualClock clock;
	ApplicationEditableVersion app(clock, cfg);
	app.start();

	// set up world
	auto root = TestAccount::createRoot(app);

	auto fixAmount = 10000000000000000;

	SECTION("a pays b, use alias")
	{
		auto rA = root.create("rrrA", fixAmount);
		auto secretKeyForB1 = SecretKey().random();
		auto b1 = root.create(secretKeyForB1, fixAmount);
		PublicKey &aliasID = SecretKey().random().getPublicKey();
		b1.manageAlias(aliasID, secretKeyForB1.getPublicKey());
		int64 b1Balance = b1.getBalance();
		auto payAmount = 200;
		auto txFrame = rA.tx(
		{
			payment(aliasID, payAmount)
		});
		auto res = applyCheck(txFrame, app);
		REQUIRE(b1Balance + payAmount == b1.getBalance());
	}

	SECTION("Try create Alias with reserve AccountID")
	{
		auto secretKeyForA = SecretKey().random();
		auto accountA = root.create(secretKeyForA, fixAmount);
		auto secretKeyForB = SecretKey().random();
		auto b1 = root.create(secretKeyForB, fixAmount);
		SecretKey &aliasID = SecretKey().random();
		CHECK_NOTHROW(b1.manageAlias(secretKeyForA.getPublicKey(), secretKeyForB.getPublicKey()));
	}

	SECTION("Try create Account with ID reserve Alias")
	{
		SecretKey &accountKey = SecretKey().random();
		auto accountA = root.create(accountKey, fixAmount);
		SecretKey &aliasID = SecretKey().random();
		accountA.manageAlias(aliasID.getPublicKey(), accountKey.getPublicKey());
		REQUIRE(AliasFrame::isAliasIdExist(aliasID.getPublicKey(), app.getDatabase()));
		REQUIRE_THROWS_AS(root.create(aliasID, fixAmount), ex_CREATE_ACCOUNT_ALREADY_EXIST);
	}

	SECTION("Try create Alias for NOT exist account")
	{
		SecretKey &accountKey = SecretKey().random();
		CHECK_NOTHROW(root.manageAlias(accountKey.getPublicKey(), SecretKey().random().getPublicKey()));
	}

	SECTION("Try create Alias for not owner account") {
		SecretKey skA = SecretKey().random();
		SecretKey skB = SecretKey().random();
		PublicKey aliasID = SecretKey().random().getPublicKey();
		auto accountA = root.create(skA, fixAmount);
		auto accountB = root.create(skB, fixAmount);
		CHECK_NOTHROW(accountA.manageAlias(aliasID, skB.getPublicKey()));
	}

	SECTION("a pays b to use alias(not exist)")
	{
		SecretKey skA = SecretKey().random();
		SecretKey skB = SecretKey().random();
		PublicKey aliasID = SecretKey().random().getPublicKey();
		auto accountA = root.create(skA, fixAmount);
		auto accountB = root.create(skB, fixAmount);
		auto payAmount = 200;
		auto txFrame = accountA.tx(
		{
			payment(aliasID, payAmount)
		});
		REQUIRE(!(applyCheck(txFrame, app)));
	}

	SECTION("a deletes own alias")
	{
		SecretKey skA = SecretKey().random();
		PublicKey aliasID = SecretKey().random().getPublicKey();
		auto accountA = root.create(skA, fixAmount);
		accountA.manageAlias(aliasID, skA.getPublicKey());
		REQUIRE(AliasFrame::isAliasIdExist(aliasID, app.getDatabase()));
		accountA.manageAlias(aliasID, SecretKey().random().getPublicKey());
		REQUIRE(!AliasFrame::isAliasIdExist(aliasID, app.getDatabase()));
	}
}





