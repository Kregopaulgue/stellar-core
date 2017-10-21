//
// Created by krego on 07.09.17.
//

#include "crypto/SignerKey.h"
#include "lib/catch.hpp"
#include "main/Application.h"
#include "main/Config.h"
#include "overlay/LoopbackPeer.h"
#include "test/TestAccount.h"
#include "test/TestExceptions.h"
#include "test/TestUtils.h"
#include "test/TxTests.h"
#include "test/test.h"
#include "transactions/TransactionFrame.h"
#include "util/Logging.h"
#include "util/Timer.h"
#include "util/make_unique.h"
#include <iostream>

using namespace stellar;
using namespace stellar::txtest;

typedef std::unique_ptr<Application> appPtr;

TEST_CASE("give signers access", "[tx][givesignersaccess]")
{
    using xdr::operator==;

    Config const& cfg = getTestConfig();
    VirtualClock clock;
    ApplicationEditableVersion app(clock, cfg);

    app.start();

    app.getLedgerManager().setCurrentLedgerVersion(0);

    auto root = TestAccount::createRoot(app);

    auto accessGiver = root.create("A1", app.getLedgerManager().getMinBalance(0) + 1000);
    auto accessTaker = root.create("A2", app.getLedgerManager().getMinBalance(0) + 1000);

    AccountID accessGiverID = accessGiver.getPublicKey();
    AccountID accessTakerID = accessTaker.getPublicKey();

    int64 rightTime = time(nullptr) + INT16_MAX;
    int64 wrongTime = time(nullptr) - INT16_MAX;

    SECTION("access taker is access giver")
    {
        for_all_versions(app, [&]{
            REQUIRE_THROWS_AS(
                    accessGiver.giveSignersAccess(accessGiverID, rightTime),
                    ex_GIVE_SIGNERS_ACCESS_FRIEND_IS_SOURCE);
        });
    }

    //may just froze it
    SECTION("time frames equal or less then current time")
    {
        for_all_versions(app, [&]{
            REQUIRE_THROWS_AS(
                    accessGiver.giveSignersAccess(accessTakerID, wrongTime),
                    ex_GIVE_SIGNERS_ACCESS_TIME_FRAMES_EQUAL_OR_LESS_THEN_CURRENT_TIME);
        });
    }

    SECTION("creating signersaccess")
    {
        for_versions({1}, app, [&]{

            accessGiver.giveSignersAccess(accessTakerID, rightTime);

            SignersAccessFrame::pointer signersAccess;

            signersAccess = loadSignersAccess(accessGiverID, accessTakerID, app, true);

            REQUIRE(accessGiverID == signersAccess->getAccessGiverID());
            REQUIRE(accessTakerID == signersAccess->getAccessTakerID());
            REQUIRE(rightTime == signersAccess->getTimeFrames());
        });
    }

    SECTION("signers access already exists")
    {
        for_all_versions(app, [&]{
            accessGiver.giveSignersAccess(accessTakerID, rightTime);
            REQUIRE_THROWS_AS(
                    accessGiver.giveSignersAccess(accessTakerID, rightTime),
                    ex_GIVE_SIGNERS_ACCESS_SIGNERS_ACCESS_ALREADY_EXISTS);
        });
    }

}