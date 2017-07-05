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

	auto a1 = testAcc.create("aaaa", 100000000000);
	LOG(INFO) << KeyUtils::toStrKey(a1.getPublicKey()).c_str();
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

