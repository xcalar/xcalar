// Copyright 2018 Xcalar, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Xcalar, Inc.
// Please refer to the included "COPYING" file for terms and conditions
// regarding the use and redistribution of this software.
//

#include "primitives/Primitives.h"
#include "libapis/LibApisCommon.h"
#include "libapis/ApiHandler.h"
#include "libapis/ApiHandlerDatasetCreate.h"
#include "libapis/OperatorHandlerBulkLoad.h"
#include "sys/XLog.h"
#include "dataset/Dataset.h"
#include "operators/XcalarEval.h"

ApiHandlerDatasetCreate::ApiHandlerDatasetCreate(XcalarApis api)
    : ApiHandler(api), input_(NULL)
{
}

ApiHandler::Flags
ApiHandlerDatasetCreate::getFlags()
{
    return (Flags)(NeedsAck | NeedsToRunImmediately);
}

Status
ApiHandlerDatasetCreate::run(XcalarApiOutput **outputOut, size_t *outputSizeOut)
{
    Status status = StatusOk;
    Dataset *ds = Dataset::get();
    XcalarApiOutput *output = NULL;
    size_t outputSize = 0;

    outputSize = XcalarApiSizeOfOutput(output->outputResult.noOutput);
    output = (XcalarApiOutput *) memAllocExt(outputSize, moduleName);
    if (output == NULL) {
        xSyslogTxnBuf(moduleName,
                      XlogErr,
                      "Insufficient memory to allocate output (Required size: "
                      "%lu "
                      "bytes)",
                      outputSize);
        status = StatusNoMem;
        goto CommonExit;
    }
    memZero(output, outputSize);

    status = ds->createDatasetMeta(input_);
    if (status != StatusOk) {
        xSyslogTxnBuf(moduleName,
                      XlogErr,
                      "Failed to create dataset meta for %s: %s",
                      input_->datasetName,
                      strGetFromStatus(status));
        goto CommonExit;
    }

CommonExit:
    if (status != StatusOk) {
        if (output != NULL) {
            memFree(output);
            output = NULL;
        }
        outputSize = 0;
    }
    if (output != NULL) {
        output->hdr.status = status.code();
    }

    *outputOut = output;
    *outputSizeOut = outputSize;

    return status;
}

Status
ApiHandlerDatasetCreate::setArg(XcalarApiInput *input, size_t inputSize)
{
    Status status = StatusOk;
    apiInput_ = input;
    inputSize_ = inputSize;
    input_ = &input->datasetCreateInput;

    // Do some sanity checks
    if (sizeof(*input_) != inputSize) {
        xSyslogTxnBuf(moduleName,
                      XlogErr,
                      "Input size provided (%lu bytes) does not match "
                      "sizeof(*input_) = %lu bytes",
                      inputSize,
                      sizeof(*input_));
        status = StatusInval;
        goto CommonExit;
    }

    if (input_->loadArgs.maxSize < 0) {
        xSyslogTxnBuf(moduleName,
                      XlogErr,
                      "Invalid maxSize provided: %lu",
                      input_->loadArgs.maxSize);
        status = StatusInval;
        goto CommonExit;
    }

    if (!XcalarEval::get()->isValidDatasetName(input_->datasetName)) {
        xSyslogTxnBuf(moduleName,
                      XlogErr,
                      "Invalid dataset name provided: %s",
                      input_->datasetName);
        status = StatusInval;
        goto CommonExit;
    }

CommonExit:

    return status;
}
