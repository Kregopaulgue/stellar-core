cmake_minimum_required(VERSION 3.8)

project(core)

add_definitions(
        -DASIO_STANDALONE
        -DNOMINMAX
        -DASIO_SEPARATE_COMPILATION
        -D_CRT_NONSTDC_NO_DEPRECATE
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
        -DSODIUM_STATIC
        -DASIO_ERROR_CATEGORY_NOEXCEPT=noexcept
        -D_CRT_SECURE_NO_WARNINGS
        -D_WIN32_WINNT=0x0501
        -DWIN32
        -D_MBCS
        -DUSE_POSTGRES
        )

add_definitions(
        /wd4060
        /wd4127
        /wd4324
        /wd4408
        /wd4510
        /wd4512
        /wd4582
        /wd4583
        /wd4592
        /wd4018
        /wd4244
        /wd4715
        /wd4267
        /wd4005
        /wd4307
        /Gd
)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /GS /W4 /wd4060 /wd4100 /wd4127 /wd4324 /wd4408 /wd4510 /wd4512 /wd4582 /wd4583 /wd4592 /Gy /Zc:wchar_t")
#
include_directories($ENV{POSTGRES_PATH_INCLUDE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)
include_directories(../lib)
include_directories(../lib/xdrpp)
include_directories(../lib/xdrpp/xdrpp/)
include_directories(../lib/asio/include)
include_directories(../lib/libmedida/src)
include_directories(../lib/libsodium/src/libsodium/include)
include_directories(../lib/soci/src/core)
include_directories(../lib/autocheck/include)
include_directories(../lib/cereal/include)
include_directories(../Builds/VisualStudio2015/src/generated)

set(STELLAR_CORE_SYSTEM_SOURCE_FILES
        ../lib/util/easylogging++.cc
        bucket/Bucket.cpp
        bucket/BucketApplicator.cpp
        bucket/BucketList.cpp
        bucket/BucketManagerImpl.cpp
        bucket/BucketTests.cpp
        bucket/FutureBucket.cpp
        crypto/CryptoTests.cpp
        crypto/ECDH.cpp
        crypto/Hex.cpp
        crypto/KeyUtils.cpp
        crypto/Random.cpp
        crypto/SHA.cpp
        crypto/SecretKey.cpp
        crypto/SignerKey.cpp
        crypto/SignerKeyUtils.cpp
        crypto/StrKey.cpp
        database/AccountQueries.cpp
        database/Database.cpp
        database/DatabaseConnectionString.cpp
        database/DatabaseConnectionStringTest.cpp
        database/DatabaseTests.cpp
        herder/Herder.cpp
        herder/HerderImpl.cpp
        herder/HerderTests.cpp
        herder/HerderUtils.cpp
        herder/LedgerCloseData.cpp
        herder/PendingEnvelopes.cpp
        herder/TxSetFrame.cpp
        historywork/ApplyBucketsWork.cpp
        historywork/ApplyLedgerChainWork.cpp
        historywork/BatchDownloadWork.cpp
        historywork/BucketDownloadWork.cpp
        historywork/CatchupCompleteImmediateWork.cpp
        historywork/CatchupCompleteWork.cpp
        historywork/CatchupMinimalWork.cpp
        historywork/CatchupRecentWork.cpp
        historywork/CatchupTransactionsWork.cpp
        historywork/CatchupWork.cpp
        historywork/FetchRecentQsetsWork.cpp
        historywork/GetAndUnzipRemoteFileWork.cpp
        historywork/GetHistoryArchiveStateWork.cpp
        historywork/GetRemoteFileWork.cpp
        historywork/GunzipFileWork.cpp
        historywork/GzipFileWork.cpp
        historywork/MakeRemoteDirWork.cpp
        historywork/PublishWork.cpp
        historywork/PutHistoryArchiveStateWork.cpp
        historywork/PutRemoteFileWork.cpp
        historywork/PutSnapshotFilesWork.cpp
        historywork/RepairMissingBucketsWork.cpp
        historywork/ResolveSnapshotWork.cpp
        historywork/RunCommandWork.cpp
        historywork/VerifyBucketWork.cpp
        historywork/VerifyLedgerChainWork.cpp
        historywork/WriteSnapshotWork.cpp
        history/CatchupManagerImpl.cpp
        history/FileTransferInfo.cpp
        history/HistoryArchive.cpp
        history/HistoryManagerImpl.cpp
        history/HistoryTests.cpp
        history/InferredQuorum.cpp
        history/InferredQuorumTests.cpp
        history/StateSnapshot.cpp
        invariant/CacheIsConsistentWithDatabase.cpp
        invariant/ChangedAccountsSubnetriesCountIsValid.cpp
        invariant/Invariant.cpp
        invariant/InvariantDoesNotHold.cpp
        invariant/Invariants.cpp
        invariant/TotalCoinsEqualsBalancesPlusFeePool.cpp
        ledger/AccountFrame.cpp
        ledger/DataFrame.cpp
        ledger/LedgerDelta.cpp
        ledger/EntryFrame.cpp
        ledger/LedgerDeltaTests.cpp
        ledger/LedgerEntryTests.cpp
        ledger/LedgerHeaderFrame.cpp
        ledger/LedgerHeaderTests.cpp
        ledger/LedgerManagerImpl.cpp
        ledger/LedgerPerformanceTests.cpp
        ledger/LedgerTests.cpp
        ledger/LedgerTestUtils.cpp
        ledger/OfferFrame.cpp
        ledger/SyncingLedgerChain.cpp
        ledger/SyncingLedgerChainTests.cpp
        ledger/TrustFrame.cpp
        ../lib/asio/src/asio.cpp
        ../lib/http/connection.cpp
        ../lib/http/connection_manager.cpp
        ../lib/http/HttpClient.cpp
        ../lib/http/reply.cpp
        ../lib/http/request_parser.cpp
        ../lib/http/server.cpp
        ../lib/json/jsoncpp.cpp
        ../lib/util/getopt_long.c
        ../lib/util/uint128_t.cpp
        #../lib/xdrpp/tests/arpc.cc
        )

set(STELLAR_CORE_MAIN_SOURCE_FILES
        main/Application.cpp
        main/ApplicationImpl.cpp
        main/ApplicationTests.cpp
        main/ConfigTests.cpp
        main/dumpxdr.cpp
        main/fuzz.cpp
        main/NtpSynchronizationChecker.cpp
        main/PersistentState.cpp
        main/ExternalQueue.cpp
        overlay/BanManagerImpl.cpp
        overlay/FloodTests.cpp
        overlay/ItemFetcherTests.cpp
        overlay/LoadManager.cpp
        overlay/OverlayManagerTests.cpp
        overlay/PeerAuth.cpp
        overlay/PeerRecord.cpp
        overlay/PeerRecordTests.cpp
        overlay/TCPPeerTests.cpp
        overlay/Tracker.cpp
        scp/BallotProtocol.cpp
        scp/LocalNode.cpp
        scp/NominationProtocol.cpp
        scp/QuorumSetTests.cpp
        scp/QuorumSetUtils.cpp
        scp/SCP.cpp
        scp/SCPDriver.cpp
        scp/SCPTests.cpp
        scp/SCPUnitTests.cpp
        scp/Slot.cpp
        simulation/CoreTests.cpp
        simulation/LoadGenerator.cpp
        simulation/Simulation.cpp
        simulation/Topologies.cpp
        test/test.cpp
        test/TestAccount.cpp
        test/TestExceptions.cpp
        test/TestPrinter.cpp
        test/TestUtils.cpp
        test/TxTests.cpp
        transactions/AllowTrustOpFrame.cpp
        transactions/ChangeTrustTests.cpp
        transactions/CreateAccountOpFrame.cpp
        transactions/CreatePassiveOfferOpFrame.cpp
        transactions/ManageDataOpFrame.cpp
        transactions/ManageDataTests.cpp
        transactions/ManageOfferOpFrame.cpp
        transactions/InflationOpFrame.cpp
        transactions/InflationTests.cpp
        transactions/MergeOpFrame.cpp
        transactions/MergeTests.cpp
        transactions/OfferExchange.cpp
        transactions/OfferTests.cpp
        transactions/OperationFrame.cpp
        transactions/PathPaymentOpFrame.cpp
        transactions/PaymentOpFrame.cpp
        transactions/PaymentTests.cpp
        transactions/SetOptionsOpFrame.cpp
        transactions/SetOptionsTests.cpp
        transactions/SignatureChecker.cpp
        transactions/SignatureUtils.cpp
        transactions/SignatureUtilsTest.cpp
        transactions/TxEnvelopeTests.cpp
        ../lib/util/crc16.cpp
        util/BigDivideTests.cpp
        util/BitsetEnumerator.cpp
        util/BitsetEnumeratorTests.cpp
        util/Format.cpp
        util/Fs.cpp
        util/FsTests.cpp
        util/GlobalChecks.cpp
        util/HashOfHash.cpp
        util/Math.cpp
        util/NtpClient.cpp
        util/NtpWork.cpp
        util/SecretValue.cpp
        util/StatusManager.cpp
        util/StatusManagerTest.cpp
        util/TmpDir.cpp
        util/Timer.cpp
        util/TimerTests.cpp
        util/types.cpp
        main/CommandHandler.cpp
        main/Config.cpp
        main/main.cpp
        overlay/Floodgate.cpp
        overlay/ItemFetcher.cpp
        overlay/LoopbackPeer.cpp
        overlay/OverlayTests.cpp
        overlay/Peer.cpp
        overlay/PeerDoor.cpp
        overlay/OverlayManagerImpl.cpp
        overlay/TCPPeer.cpp
        process/ProcessManagerImpl.cpp
        process/ProcessTests.cpp
        transactions/TransactionFrame.cpp
        transactions/ChangeTrustOpFrame.cpp
        util/Logging.cpp
        util/Uint128Tests.cpp
        work/Work.cpp
        work/WorkManagerImpl.cpp
        work/WorkParent.cpp
        work/WorkTests.cpp
        )

set(XDRC_COMMAND ${CMAKE_BINARY_DIR}/lib/xdrpp/xdrc/xdrc.exe)

# break into sections
add_custom_command(OUTPUT out
        COMMAND ${XDRC_COMMAND} -hh -o Stellar-ledger.h Stellar-ledger.x
        COMMAND ${XDRC_COMMAND} -hh -o Stellar-transaction.h Stellar-transaction.x
        COMMAND ${XDRC_COMMAND} -hh -o Stellar-ledger-entries.h Stellar-ledger-entries.x
        COMMAND ${XDRC_COMMAND} -hh -o Stellar-types.h Stellar-types.x
        COMMAND ${XDRC_COMMAND} -hh -o Stellar-overlay.h Stellar-overlay.x
        COMMAND ${XDRC_COMMAND} -hh -o Stellar-SCP.h Stellar-SCP.x
        DEPENDS
        xdr/Stellar-ledger.x
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/xdr
        )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xdr)

find_library(WSOCK32_LIBRARY wsock32)
find_library(WS2_32_LIBRARY ws2_32)
find_library(PSAPI psapi)

find_package(PostgreSQL REQUIRED)
find_library(LIBPSQ libpq)

if(NOT POSTGRESQL_FOUND)
        message(SEND_ERROR "NOT FOUND postgresql")
endif()

if(POSTGRESQL_FOUND)
        message(INFO " FOUND POSGRESQ")
endif()

message(INFO ${POSTGRESQL_LIBRARIES})

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE /LTCG" )

add_executable(core ${STELLAR_CORE_SYSTEM_SOURCE_FILES} ${STELLAR_CORE_MAIN_SOURCE_FILES})

target_link_libraries(core ${LIBPSQ})
target_link_libraries(core wsock32 ws2_32)
target_link_libraries(core ${PSQLIBS})
target_link_libraries(core soci)
target_link_libraries(core asio)
target_link_libraries(core medida)
target_link_libraries(core sodium)
target_link_libraries(core sqlite)
target_link_libraries(core soci_sqlite3)
target_link_libraries(core soci_postgresql)
target_link_libraries(core xdrpp)
target_link_libraries(core fmt)
target_link_libraries(core psapi)