#include "CreateAccountTest.h"

#include "lib/catch.hpp"
#include "lib/json/json.h"
#include "main/Application.h"
#include "overlay/LoopbackPeer.h"
#include "test/TestAccount.h"
#include "test/TestExceptions.h"
#include "test/TestUtils.h"
#include "test/TxTests.h"
#include "test/test.h"
#include "util/Logging.h"
#include "util/make_unique.h"
#include "transactions/CreateAccountOpFrame.h"
using namespace stellar;
using namespace stellar::txtest;

typedef std::unique_ptr<Application> appPtr;

TEST_CASE("exist trust", "[tx][existtrustline]") {

	Config const& cfg = getTestConfig();

	VirtualClock clock;
	ApplicationEditableVersion app{ clock, cfg };
	Database& db = app.getDatabase();

	app.start();

	auto testAcc = TestAccount::createRoot(app);

	auto a1 = testAcc.create("a1a1a1q1q", 1000000000000);
	REQUIRE(loadAccount(a1, app, true));
	auto paymentAmount = 1000000000;
	auto amount = app.getLedgerManager().getMinBalance(0) + paymentAmount;
	auto b1 = testAcc.create("B", amount);
	REQUIRE(loadAccount(b1, app, true));
	AccountID aliasID = SecretKey().random().getPublicKey();
	testAcc.createAlias(aliasID, b1.getPublicKey());
	int64 a1Balance = a1.getBalance();
	int64 b1Balance = b1.getBalance();
	LOG(INFO) << b1Balance;
	auto txFrame = a1.tx({ payment(b1,200000) });

	auto res = applyCheck(txFrame, app);
	REQUIRE(txFrame->getResultCode() == txSUCCESS);
	LOG(INFO) << res;
	REQUIRE(loadAccount(b1, app));
	LOG(INFO) << b1.getBalance();

	//LOG(INFO) << KeyUtils::toStrKey(a1.getSecretKey()).value.c_str();
	//auto a2 = testAcc.create("aaaa1", 1);

}


CreateAccountTest::CreateAccountTest()
{
}


CreateAccountTest::~CreateAccountTest()
{
}




//
//using namespace stellar;
//usign namespace tx::

