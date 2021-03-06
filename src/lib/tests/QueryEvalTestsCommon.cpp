// Copyright 2016 - 2018 Xcalar, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Xcalar, Inc.
// Please refer to the included "COPYING" file for terms and conditions
// regarding the use and redistribution of this software.
//

#include "StrlFunc.h"
#include "util/MemTrack.h"
#include "dag/DagLib.h"
#include "querymanager/QueryManager.h"
#include "usr/Users.h"
#include "sys/XLog.h"
#include "QueryEvalTestsCommon.h"
#include "queryparser/QueryParser.h"
#include "queryeval/QueryEvaluate.h"

static constexpr const char *moduleName = "libQueryEvalFuncTest";

static XcalarApiUserId dummyUserId = {
    "queryEvalFTUsr",
    0xdeadbeef,
};

static constexpr const char *dummyUserId2Name = "queryEvalFTUsr2-node%d-thr%d";
// userIdUnique is generated by also using node number and thread number
static XcalarApiUserId dummyUserId2 = {
    "",
    0xdeadc000,
};

static Status
createSession(XcalarApiUserId *userId,
              const uint32_t threadNum,
              uint64_t *sessionId)
{
    XcalarApiSessionNewInput *sessionNewInput = NULL;
    XcalarApiSessionNewOutput *sessionNewOutput = NULL;
    Status status = StatusUnknown;
    UserMgr *userMgr = UserMgr::get();
    Config *config = Config::get();

    sessionNewInput =
        (XcalarApiSessionNewInput *) memAlloc(sizeof(*sessionNewInput));
    if (sessionNewInput == NULL) {
        xSyslog(moduleName,
                XlogErr,
                "Thread %u (%s): Insufficient memory to allocate "
                "sessionNewInput",
                sysGetTid(),
                userId->userIdName);
        status = StatusNoMem;
        goto CommonExit;
    }
    memZero(sessionNewInput, sizeof(*sessionNewInput));

    sessionNewOutput =
        (XcalarApiSessionNewOutput *) memAlloc(sizeof(*sessionNewOutput));
    if (sessionNewOutput == NULL) {
        xSyslog(moduleName,
                XlogErr,
                "Thread %u (%s): Insufficient memory to allocate "
                "sessionNewOutput",
                sysGetTid(),
                userId->userIdName);
        status = StatusNoMem;
        goto CommonExit;
    }
    memZero(sessionNewOutput, sizeof(*sessionNewOutput));

    snprintf(sessionNewInput->sessionName,
             sizeof(sessionNewInput->sessionName),
             "%s-n%d-th%d",
             userId->userIdName,
             config->getMyNodeId(),
             threadNum);
    sessionNewInput->sessionNameLength = strlen(sessionNewInput->sessionName);
    sessionNewInput->fork = false;
    sessionNewInput->forkedSessionNameLength = 0;

    status = userMgr->create(userId, sessionNewInput, sessionNewOutput);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "Thread %u (%s): Could not create session. Status: %s (%s)",
                sysGetTid(),
                userId->userIdName,
                strGetFromStatus(status),
                sessionNewOutput->error);
        goto CommonExit;
    }

    xSyslog(moduleName,
            XlogInfo,
            "Thread %u (%s): Session created successfully. SessionId: %lu",
            sysGetTid(),
            userId->userIdName,
            sessionNewOutput->sessionId);

    status = StatusOk;

    // Return the session ID so the session can later be deleted.
    *sessionId = sessionNewOutput->sessionId;

CommonExit:
    if (sessionNewOutput != NULL) {
        memFree(sessionNewOutput);
        sessionNewOutput = NULL;
    }

    if (sessionNewInput != NULL) {
        memFree(sessionNewInput);
        sessionNewInput = NULL;
    }

    return status;
}

static Status
activateSession(XcalarApiUserId *userId, const uint32_t threadNum)
{
    XcalarApiSessionActivateInput *sessionActivateInput = NULL;
    XcalarApiSessionGenericOutput *sessionActivateOutput = NULL;
    Status status = StatusUnknown;
    UserMgr *userMgr = UserMgr::get();
    Config *config = Config::get();

    sessionActivateInput = (XcalarApiSessionActivateInput *) memAlloc(
        sizeof(*sessionActivateInput));
    if (sessionActivateInput == NULL) {
        xSyslog(moduleName,
                XlogErr,
                "Thread %u (%s): Insufficient memory to allocate "
                "sessionActivateInput",
                sysGetTid(),
                userId->userIdName);
        status = StatusNoMem;
        goto CommonExit;
    }
    memZero(sessionActivateInput, sizeof(*sessionActivateInput));

    snprintf(sessionActivateInput->sessionName,
             sizeof(sessionActivateInput->sessionName),
             "%s-n%d-th%d",
             userId->userIdName,
             config->getMyNodeId(),
             threadNum);

    sessionActivateInput->sessionNameLength =
        strlen(sessionActivateInput->sessionName);
    sessionActivateInput->sessionId = 0;

    sessionActivateOutput = (XcalarApiSessionGenericOutput *) memAlloc(
        sizeof(*sessionActivateOutput));
    if (sessionActivateOutput == NULL) {
        xSyslog(moduleName,
                XlogErr,
                "Thread %u (%s): Insufficient memory to allocate "
                "sessionActivateOutput",
                sysGetTid(),
                userId->userIdName);
        status = StatusNoMem;
        goto CommonExit;
    }

    status =
        userMgr->activate(userId, sessionActivateInput, sessionActivateOutput);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "Thread %u (%s): Could not activate session. Status: %s (%s)",
                sysGetTid(),
                userId->userIdName,
                strGetFromStatus(status),
                sessionActivateOutput->errorMessage);
        goto CommonExit;
    }

CommonExit:
    if (sessionActivateOutput != NULL) {
        memFree(sessionActivateOutput);
        sessionActivateOutput = NULL;
    }

    if (sessionActivateInput != NULL) {
        memFree(sessionActivateInput);
        sessionActivateInput = NULL;
    }

    return status;
}

// Notes from QueryEvaluate.cpp
//
// Difference between queryStepThrough and queryEvaluate is
// 1) stepThrough doesn't automatically drop tables
// 2) stepThrough executes every command in the query graph
// as opposed to queryEvaluate which only executes just the commands
// required to produce the desired output
// 3) The output of executing each command in stepThrough is immediately
// added to the workspaceGraph. On the other hand. evalute only adds
// the output of evaluating the entire queryGraph to the workspaceGraph

Status
qeDoEvaluateTest(const char *query)
{
    Status status = StatusOk;
    QueryParser *qp = QueryParser::get();
    QueryEvaluate *qe = QueryEvaluate::get();
    QueryManager *qm = QueryManager::get();
    DagLib *dagLib = DagLib::get();
    Dag *queryGraph = NULL;
    uint64_t numQueryGraphNodes;
    DagTypes::NodeId *activeNodes = NULL;
    uint64_t numActiveNodes = 0;
    Dag *workspaceGraph = NULL;
    QueryManager::QueryJob *queryJob = NULL;
    LibNsTypes::NsId queryId = XidInvalid;

    status = qp->parse((char *) query, NULL, &queryGraph, &numQueryGraphNodes);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoEvaluate: Unable to parse query: %s",
                strGetFromStatus(status));
        goto CommonExit;
    }
    assert(queryGraph != NULL);

    status =
        queryGraph->getActiveDagNodes(DagTypes::QueryGraph,
                                      NULL,
                                      &activeNodes,
                                      &numActiveNodes,
                                      DagTypes::IncludeNodeStateProcessing);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoEvaluate: Unable to get active Dag nodes: %s",
                strGetFromStatus(status));
        goto CommonExit;
    }

    status = dagLib->createNewDag(16 /* numSlot */,
                                  DagTypes::WorkspaceGraph,
                                  NULL,
                                  &workspaceGraph);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoEvaluate: Unable to create new Dag node: %s",
                strGetFromStatus(status));
        goto CommonExit;
    }

    queryJob = new (std::nothrow) QueryManager::QueryJob();
    if (queryJob == NULL) {
        xSyslog(moduleName,
                XlogErr,
                "DoEvaluate: Unable to allocate query job");
        status = StatusNoMem;
        goto CommonExit;
    }

    status = qm->initQueryJob(queryJob, queryId, "\0", "\0", &dummyUserId);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoEvaluate: Unable to init query job: %s",
                strGetFromStatus(status));
        goto CommonExit;
    }

    // XXX: does it need to be globally visible?  (see processQueryGraph).
    queryJob->queryGraph = queryGraph;
    queryGraph = NULL;

    status = qe->evaluate(queryJob->queryGraph,
                          numActiveNodes,
                          activeNodes,
                          queryJob,
                          &workspaceGraph,
                          Runtime::SchedId::Sched0,
                          static_cast<ExecutionMode>(rand() % 2),
                          workspaceGraph->getSessionContainer());
    if (status == StatusOk) {
        atomicWrite32(&queryJob->queryState, qrFinished);
    } else if (status == StatusCanceled) {
        atomicWrite32(&queryJob->queryState, qrCancelled);
    } else {
        atomicWrite32(&queryJob->queryState, qrError);
    }

    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoEvaluate: Unable to evaluate query: %s",
                strGetFromStatus(status));
        goto CommonExit;
    }

CommonExit:
    if (queryGraph != NULL) {
        Status status2 =
            dagLib->destroyDag(queryGraph,
                               DagTypes::DestroyDeleteAndCleanNodes);
        assert(status2 == StatusOk);
        queryGraph = NULL;
    }

    if (activeNodes != NULL) {
        memFree(activeNodes);
        activeNodes = NULL;
        numActiveNodes = 0;
    }
    if (workspaceGraph != NULL) {
        Status status2 =
            dagLib->destroyDag(workspaceGraph,
                               DagTypes::DestroyDeleteAndCleanNodes);
        assert(status2 == StatusOk);
        workspaceGraph = NULL;
    }
    if (queryJob != NULL) {
        delete queryJob;
        queryJob = NULL;
    }

    return status;
}

Status
qeDoStepThroughTest(const char *query,
                    const uint32_t threadNum,
                    Runtime::SchedId schedId)
{
    Status status = StatusOk;
    UserMgr *userMgr = UserMgr::get();
    QueryParser *qp = QueryParser::get();
    QueryEvaluate *qe = QueryEvaluate::get();
    QueryManager *qm = QueryManager::get();
    DagLib *dagLib = DagLib::get();
    Dag *queryGraph = NULL;
    Dag *workspaceGraph = NULL;
    QueryManager::QueryJob *queryJob = NULL;
    uint64_t numQueryGraphNodes = 0;
    LibNsTypes::NsId queryId = XidInvalid;
    bool haveSession = false;
    XcalarApiUserId myUserId;
    uint64_t sessionId = 0;
    XcalarApiSessionDeleteInput *sessionDeleteInput = NULL;
    XcalarApiSessionGenericOutput *sessionGenOutput = NULL;
    Config *config = Config::get();
    char uniqueUserIdName[LOGIN_NAME_MAX];

    // Each thread gets a unique user name and unique ID
    int ret = snprintf(uniqueUserIdName,
                       LOGIN_NAME_MAX,
                       dummyUserId2Name,
                       config->getMyNodeId(),
                       threadNum);
    assert(ret <= LOGIN_NAME_MAX);
    strcpy(myUserId.userIdName, uniqueUserIdName);

    // The unique ID is seeded with a valid starting ID (e.g. zero is not
    // valid) and uses the node ID (unique in cluster or multi-usrnode
    // on desktop) and threadNum (not unique in cluster/desktop).
    myUserId.userIdUnique =
        dummyUserId2.userIdUnique + (config->getMyNodeId() << 16) + threadNum;

    status = userMgr->getDag(&myUserId, NULL, &workspaceGraph);
    if (status == StatusSessionNotFound) {
        // Create a session unique to this thread
        status = createSession(&myUserId, threadNum, &sessionId);
        // XXX: debug - Xc-5883
        xSyslog(moduleName,
                XlogErr,
                "Thread %u/%d (%s/%u): Create Status: %s, sessionId %lu",
                sysGetTid(),
                threadNum,
                myUserId.userIdName,
                myUserId.userIdUnique,
                strGetFromStatus(status),
                sessionId);
        // session is born inactive, so activate on success
        // StatusSessionExists is probably related to Xc-5883
        if (status == StatusOk || status == StatusSessionExists) {
            status = activateSession(&myUserId, threadNum);
            // XXX: debug - Xc-5883
            xSyslog(moduleName,
                    XlogErr,
                    "Thread %u/%d (%s/%u): Activate Status: %s, sessionId %lu",
                    sysGetTid(),
                    threadNum,
                    myUserId.userIdName,
                    myUserId.userIdUnique,
                    strGetFromStatus(status),
                    sessionId);
        }
        // Allow session not being inactive for testing
        assert(status == StatusOk || status == StatusSessionNotInact);
        status = userMgr->getDag(&myUserId, NULL, &workspaceGraph);
    }
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoStepThrough: Unable to obtain session for thread %d: %s",
                threadNum,
                strGetFromStatus(status));
        goto CommonExit;
    }
    assert(workspaceGraph != NULL);
    haveSession = true;

    status = qp->parse((char *) query, NULL, &queryGraph, &numQueryGraphNodes);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoStepThrough: unable to parse query for thread %d: %s",
                threadNum,
                strGetFromStatus(status));
        goto CommonExit;
    }
    assert(queryGraph != NULL);

    queryJob = new (std::nothrow) QueryManager::QueryJob();
    if (queryJob == NULL) {
        xSyslog(moduleName,
                XlogErr,
                "DoStepThrough: Unable to allocate query job for thread %d",
                threadNum);
        status = StatusNoMem;
        goto CommonExit;
    }

    status = qm->initQueryJob(queryJob, queryId, "\0", "\0", &myUserId);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoStepThrough: unable to init query job for thread %d: %s",
                threadNum,
                strGetFromStatus(status));
        goto CommonExit;
    }

    status = qe->stepThrough(workspaceGraph,
                             &queryGraph,
                             numQueryGraphNodes,
                             true /* bailOnError */,
                             schedId,
                             false,
                             queryJob);
    if (status != StatusOk) {
        xSyslog(moduleName,
                XlogErr,
                "DoStepThrough: Unable to step through query for thread %d: %s",
                threadNum,
                strGetFromStatus(status));
        goto CommonExit;
    }

CommonExit:

    if (haveSession) {
        // Clean up the session
        UserMgr *userMgr = UserMgr::get();
        Status status2;

        sessionDeleteInput = (XcalarApiSessionDeleteInput *)
            memAllocExt(sizeof(*sessionDeleteInput), moduleName);
        sessionGenOutput = (XcalarApiSessionGenericOutput *)
            memAllocExt(sizeof(*sessionGenOutput), moduleName);
        if (sessionDeleteInput == NULL || sessionGenOutput == NULL) {
            xSyslog(moduleName,
                    XlogErr,
                    "DoStepThrough: Unable to allocate session cleanup "
                    "memory for thread %d",
                    threadNum);
            // If an error hasn't already occurred return one back to
            // the caller.
            if (status == StatusOk) {
                status = StatusNoMem;
            }
            goto CleanupExit;
        }

        memset(sessionDeleteInput, 0, sizeof(*sessionDeleteInput));

        // Inactivate the session.  If the sessionId is zero then
        // the inact will use the session
        // name.  Otherwise, the session name is ignored.
        sessionDeleteInput->sessionId = sessionId;
        strcpy(sessionDeleteInput->sessionName, myUserId.userIdName);
        sessionDeleteInput->sessionNameLength = strlen(myUserId.userIdName);
        sessionDeleteInput->noCleanup = false;
        status2 = userMgr->inactivate(&myUserId,
                                      sessionDeleteInput,
                                      sessionGenOutput);
        if (status2 != StatusOk) {
            xSyslog(moduleName,
                    XlogErr,
                    "DoStepThrough: Unable to inactivate session %s, id:%lX "
                    "for thread %d: %s",
                    sessionDeleteInput->sessionName,
                    sessionDeleteInput->sessionId,
                    threadNum,
                    strGetFromStatus(status2));
            goto CleanupExit;
        }

        // Delete the session
        status2 =
            userMgr->doDelete(&myUserId, sessionDeleteInput, sessionGenOutput);
        if (status2 != StatusOk) {
            xSyslog(moduleName,
                    XlogErr,
                    "DoStepThrough: Unable to delete session %s, id:%lX "
                    "for thread %d: %s",
                    sessionDeleteInput->sessionName,
                    sessionDeleteInput->sessionId,
                    threadNum,
                    strGetFromStatus(status2));
            goto CleanupExit;
        }
        // dele gets rid of workspace graph
        workspaceGraph = NULL;
    }

CleanupExit:
    if (queryGraph != NULL) {
        Status status2 =
            dagLib->destroyDag(queryGraph,
                               DagTypes::DestroyDeleteAndCleanNodes);
        assert(status2 == StatusOk);
        queryGraph = NULL;
    }
    if (queryJob != NULL) {
        delete queryJob;
        queryJob = NULL;
    }
    if (sessionDeleteInput) {
        memFree(sessionDeleteInput);
    }
    if (sessionGenOutput) {
        memFree(sessionGenOutput);
    }

    return status;
}
