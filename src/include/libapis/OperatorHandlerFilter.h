// Copyright 2019 Xcalar, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Xcalar, Inc.
// Please refer to the included "COPYING" file for terms and conditions
// regarding the use and redistribution of this software.
//

#ifndef _OPERATORHANDLER_FILTER_H
#define _OPERATORHANDLER_FILTER_H

#include "libapis/ApiHandler.h"
#include "libapis/OperatorHandler.h"

struct XcalarApiFilterInput;

class OperatorHandlerFilter final : public OperatorHandler
{
  public:
    OperatorHandlerFilter(XcalarApis api);
    virtual ~OperatorHandlerFilter();

    Status setArg(XcalarApiInput *input,
                  size_t inputSize,
                  bool parentNodeIdsToBeProvided) override;
    Status createDagNode(DagTypes::NodeId *dstNodeIdOut,
                         DagTypes::GraphType srcGraphType,
                         const char *nodeName,
                         uint64_t numParents,
                         Dag **parentGraphs,
                         DagTypes::NodeId *parentNodeIds) override;

    Status createXdb(void *optimizerContext) override;

    const char *getDstNodeName() override;
    const char *getDstNodeName(XcalarApiOutput *output) override;

  private:
    static constexpr const char *moduleName =
        "libapis::operatorHandler::filter";
    static constexpr const char *newFieldColName = "OriginalFilterColumn";
    XcalarApiFilterInput *input_;
    unsigned numAggVariables_;
    unsigned numVariables_;
    const char **variableNames_;
    XcalarEvalAstCommon ast_;
    bool astCreated_ = false;

    Flags getFlags() override;

    Status runHandler(
        XcalarApiOutput **outputOut,
        size_t *outputSizeOut,
        void *optimizerContext,
        DagTypes::NodeId *failureTableIdOut[XcalarApiMaxFailureEvals]) override;

    Status getParentNodes(uint64_t *numParentsOut,
                          XcalarApiUdfContainer **sessionContainersOut,
                          DagTypes::NodeId **parentNodeIdsOut,
                          DagTypes::DagId **parentGraphIdsOut) override;

    void destroyArg();

    MustCheck Status genSummaryFailureTab(
        DagTypes::NodeId *failureTableIdOut[XcalarApiMaxFailureEvals]);
};

#endif  // _OPERATORHANDLER_FILTER_H