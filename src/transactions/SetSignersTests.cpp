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

TEST_CASE("set signers", "[tx][setsigners]")
{
    using xdr::operator==;

    Config const& cfg = getTestConfig();
    VirtualClock clock;
    ApplicationEditableVersion app(clock, cfg);
    app.start();

    //set up world
    auto root = TestAccount::createRoot(app);
    auto accessGiver = root.create("A1", 10000000000);
    auto accessTaker = root.create("A2", 10000000000);
    auto notFriend = root.create("A3", 1000000000);


    AccountID accessGiverID = accessGiver.getPublicKey();
    AccountID accessTakerID = accessTaker.getPublicKey();
    AccountID notFriendID = notFriend.getPublicKey();

    std::string accessGiverIDStr = KeyUtils::toStrKey(accessGiverID);
    std::string accessTakerIDStr = KeyUtils::toStrKey(accessTakerID);


    accessGiver.giveSignersAccess(accessTakerID);

    SecretKey s1 = getAccount("S1");
    Signer sk1(KeyUtils::convertKey<SignerKey>(s1.getPublicKey()), 10);
    SecretKey s2 = getAccount("S2");
    Signer sk2(KeyUtils::convertKey<SignerKey>(s2.getPublicKey()), 10);
    SecretKey s3 = getAccount("S3");
    Signer sk3(KeyUtils::convertKey<SignerKey>(s3.getPublicKey()), 10);

    ThresholdSetter th;

    th.masterWeight = make_optional<uint8_t>(100);
    th.lowThreshold = make_optional<uint8_t>(1);
    th.medThreshold = make_optional<uint8_t>(10);
    th.highThreshold = make_optional<uint8_t>(100);

    SECTION("Connecting to access giver")
    {
        SECTION("friend is source")
        {
            for_all_versions(app, [&]{
                REQUIRE_THROWS_AS(
                        accessTaker.setSigners(accessTakerID, sk3),
                        ex_SET_SIGNERS_FRIEND_IS_SOURCE);
            });
        }

        SECTION("access entry doesnt exist")
        {
            for_all_versions(app, [&]{
                REQUIRE_THROWS_AS(
                        accessTaker.setSigners(notFriendID, sk3),
                        ex_SET_SIGNERS_ACCESS_ENTRY_DOESNT_EXIST);
            });
        }

    }

    SECTION("Controlling signers")
    {
        for_versions({1}, app, [&]{

            AccountFrame::pointer accessGiverAccount;

            accessGiverAccount = loadAccount(accessGiver, app);

            unsigned long signersAmount = accessGiverAccount->getAccount().signers.size();

            sk1.weight = 10;
            accessGiver.setOptions(nullptr, nullptr, nullptr, &th, &sk1, nullptr);
            accessGiverAccount = loadAccount(accessGiver, app);

            {
                Signer& a_sk1 = accessGiverAccount->getAccount().signers[0];
                REQUIRE(a_sk1.key == sk1.key);
                REQUIRE(a_sk1.weight == sk1.weight);
            }

            sk2.weight = 10;
            accessGiver.setOptions(nullptr, nullptr, nullptr, nullptr, &sk2, nullptr);
            accessGiverAccount = loadAccount(accessGiver, app);

            signersAmount = accessGiverAccount->getAccount().signers.size();
            {
                Signer& a_sk2 = accessGiverAccount->getAccount().signers[0];
                REQUIRE(a_sk2.key == sk2.key);
                REQUIRE(a_sk2.weight == sk2.weight);
            }

            accessGiverAccount = loadAccount(accessGiver, app);
            signersAmount = accessGiverAccount->getAccount().signers.size();
            REQUIRE(signersAmount == 2);

            accessTaker.setSigners(accessGiverID, sk3);

            accessGiverAccount = loadAccount(accessGiver, app);
            REQUIRE(accessGiverAccount->getAccount().signers.size() == 1);

            Signer ag_sk = accessGiverAccount->getAccount().signers[0];
            REQUIRE(ag_sk.key == sk3.key);
        });
    }
}
