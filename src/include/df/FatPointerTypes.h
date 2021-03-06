// Copyright 2014 Xcalar, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Xcalar, Inc.
// Please refer to the included "COPYING" file for terms and conditions
// regarding the use and redistribution of this software.
//

#ifndef _FATPOINTERTYPES_H_
#define _FATPOINTERTYPES_H_

#include "primitives/Primitives.h"

typedef uint64_t Fatptr;
#define FatptrNull ((Fatptr) NULL)

// there can be many types of fat pointers
typedef Fatptr DfRecordFatptr;

#endif  // _FATPOINTERTYPES_H_
