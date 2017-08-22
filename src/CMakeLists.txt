cmake_minimum_required(VERSION 3.8)

project(core)


add_definitions(
        -DHAVE_CONFIG_H
        -DUSE_POSTGRES
        -DASIO_STANDALONE
        -DASIO_SEPARATE_COMPILATION
        -DASIO_ERROR_CATEGORY_NOEXCEPT=noexcept
        )

if(NOT ${CMAKE_COMPILER_IS_GNUCXX})

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

else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall -Wno-unused-command-line-argument -Wno-unused-local-typedef -Wno-unknown-warning-option -Werror=unused-result")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -std=c++11  -pthread -Wall -Wno-unused-command-line-argument -Wno-unused-local-typedef -Wno-unknown-warning-option -Werror=unused-result")
endif()


if(NOT ${CMAKE_COMPILER_IS_GNUCXX})
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
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../)
include_directories(../lib)
include_directories(../lib/xdrpp)
include_directories(../lib/asio/include)
include_directories(../lib/libmedida/src)
include_directories(../lib/soci/src/core)
include_directories(../lib/cereal/include)
include_directories(../lib/libsodium/src/libsodium/include)
include_directories(../lib/autocheck/include)
include_directories(../Builds/VisualStudio2015/src/generated)

set(SRC_HEADERS_FILES
        bucket/Bucket.h
        bucket/BucketApplicator.h
        bucket/BucketList.h bucket/BucketManager.h
        bucket/BucketManagerImpl.h bucket/FutureBucket.h
        bucket/LedgerCmp.h crypto/ByteSlice.h crypto/ECDH.h
        crypto/Hex.h crypto/KeyUtils.h crypto/Random.h crypto/SHA.h
        crypto/SecretKey.h crypto/SignerKey.h crypto/SignerKeyUtils.h
        crypto/StrKey.h database/AccountQueries.h database/Database.h
        database/DatabaseConnectionString.h herder/Herder.h
        herder/HerderImpl.h herder/HerderUtils.h
        herder/LedgerCloseData.h herder/PendingEnvelopes.h
        herder/TxSetFrame.h history/CatchupManager.h
        history/CatchupManagerImpl.h history/FileTransferInfo.h
        history/HistoryArchive.h history/HistoryManager.h
        history/HistoryManagerImpl.h history/InferredQuorum.h
        history/StateSnapshot.h historywork/ApplyBucketsWork.h
        historywork/ApplyLedgerChainWork.h
        historywork/BatchDownloadWork.h
        historywork/BucketDownloadWork.h
        historywork/CatchupCompleteImmediateWork.h
        historywork/CatchupCompleteWork.h
        historywork/CatchupMinimalWork.h
        historywork/CatchupRecentWork.h
        historywork/CatchupTransactionsWork.h
        historywork/CatchupWork.h historywork/FetchRecentQsetsWork.h
        historywork/GetAndUnzipRemoteFileWork.h
        historywork/GetHistoryArchiveStateWork.h
        historywork/GetRemoteFileWork.h historywork/GunzipFileWork.h
        historywork/GzipFileWork.h historywork/MakeRemoteDirWork.h
        historywork/PublishWork.h
        historywork/PutHistoryArchiveStateWork.h
        historywork/PutRemoteFileWork.h
        historywork/PutSnapshotFilesWork.h
        historywork/RepairMissingBucketsWork.h
        historywork/ResolveSnapshotWork.h historywork/RunCommandWork.h
        historywork/VerifyBucketWork.h
        historywork/VerifyLedgerChainWork.h
        historywork/WriteSnapshotWork.h
        invariant/CacheIsConsistentWithDatabase.h
        invariant/ChangedAccountsSubnetriesCountIsValid.h
        invariant/Invariant.h invariant/InvariantDoesNotHold.h
        invariant/Invariants.h
        invariant/TotalCoinsEqualsBalancesPlusFeePool.h
        ledger/AccountFrame.h ledger/DataFrame.h ledger/EntryFrame.h
        ledger/LedgerDelta.h ledger/LedgerHeaderFrame.h
        ledger/LedgerManager.h ledger/LedgerManagerImpl.h
        ledger/LedgerTestUtils.h ledger/OfferFrame.h
        ledger/SyncingLedgerChain.h ledger/TrustFrame.h
        main/Application.h main/ApplicationImpl.h
        main/CommandHandler.h main/Config.h main/ExternalQueue.h
        main/NtpSynchronizationChecker.h main/PersistentState.h
        main/dumpxdr.h main/fuzz.h overlay/BanManager.h
        overlay/BanManagerImpl.h overlay/Floodgate.h
        overlay/ItemFetcher.h overlay/LoadManager.h
        overlay/LoopbackPeer.h overlay/OverlayManager.h
        overlay/OverlayManagerImpl.h overlay/Peer.h overlay/PeerAuth.h
        overlay/PeerDoor.h overlay/PeerRecord.h overlay/StellarXDR.h
        overlay/TCPPeer.h overlay/Tracker.h process/ProcessManager.h
        process/ProcessManagerImpl.h scp/BallotProtocol.h
        scp/LocalNode.h scp/NominationProtocol.h scp/QuorumSetUtils.h
        scp/SCP.h scp/SCPDriver.h scp/Slot.h
        simulation/LoadGenerator.h simulation/Simulation.h
        simulation/Topologies.h test/TestAccount.h
        test/TestExceptions.h test/TestPrinter.h test/TestUtils.h
        test/TxTests.h test/test.h transactions/AllowTrustOpFrame.h
        transactions/ChangeTrustOpFrame.h
        transactions/CreateAccountOpFrame.h
        transactions/CreatePassiveOfferOpFrame.h
        transactions/InflationOpFrame.h
        transactions/ManageDataOpFrame.h
        transactions/ManageOfferOpFrame.h transactions/MergeOpFrame.h
        transactions/OfferExchange.h transactions/OperationFrame.h
        transactions/PathPaymentOpFrame.h
        transactions/PaymentOpFrame.h transactions/SetOptionsOpFrame.h
        transactions/SignatureChecker.h transactions/SignatureUtils.h
        transactions/TransactionFrame.h util/Algoritm.h
        util/BitsetEnumerator.h util/Format.h util/Fs.h
        util/GlobalChecks.h util/HashOfHash.h util/Logging.h
        util/Math.h util/NonCopyable.h util/NtpClient.h util/NtpWork.h
        util/SecretValue.h util/SociNoWarnings.h util/StatusManager.h
        util/Timer.h util/TmpDir.h util/XDRStream.h util/asio.h
        util/make_unique.h util/must_use.h util/optional.h
        util/types.h work/Work.h work/WorkManager.h
        work/WorkManagerImpl.h work/WorkParent.h
        )

if(NOT ${CMAKE_COMPILER_IS_GNUCXX})
        include_directories(../Builds/VisualStudio2015/src/generated)
else()
endif()


set(STELLAR_CORE_SYSTEM_SOURCE_FILES
        ../config.h
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

set(OUTPUT_SOURCE_H_XDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/xdr)

message(INFO ${XDRC_COMMAND})
# break into sections

set(XDRS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/xdr)

include(gen_xdr.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xdr)

find_package(PostgreSQL REQUIRED)

if(NOT ${CMAKE_COMPILER_IS_GNUCXX})
        find_library(WSOCK32_LIBRARY wsock32)
        find_library(WS2_32_LIBRARY ws2_32)
        find_library(PSAPI psapi)
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE /LTCG")
endif()
add_executable(core ${STELLAR_CORE_SYSTEM_SOURCE_FILES} ${STELLAR_CORE_MAIN_SOURCE_FILES} ${SRC_HEADERS_FILES} ${OUTPUT_SOURCE_H_XDR_FILES} ${SRC_X_FILES})# xdr)

target_link_libraries(core ${PostgreSQL_LIBRARIES})
target_link_libraries(core soci)
target_link_libraries(core asio)
target_link_libraries(core medida)
target_link_libraries(core sodium)
target_link_libraries(core -lm -ldl sqlite)
target_link_libraries(core soci_sqlite3)
target_link_libraries(core soci_postgresql)
target_link_libraries(core xdrpp)
target_link_libraries(core fmt)