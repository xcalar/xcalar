// Copyright 2016 Xcalar, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Xcalar, Inc.
// Please refer to the included "COPYING" file for terms and conditions
// regarding the use and redistribution of this software.
//

#ifndef _DATASET_TESTS_COMMON_H_
#define _DATASET_TESTS_COMMON_H_

#include "primitives/Primitives.h"

Status dsSetUp();
Status dsAccessorTest();
Status dsLoadXdbTests();
Status dsFatptrTests();
Status dsFieldTests();
Status dsBasicSourceTests();
Status dsNegativeSourceTests();
Status dsFormatTests();
Status dsDatasetTests();
Status dsTargetListTest();
Status exAddTargetTest();
Status dsDhtPersistTest();
Status dsTypeConversionTest();
Status dsLoadShutdownTest();
Status dsTearDown();

#endif  // _DATASET_TESTS_COMMON_H_
