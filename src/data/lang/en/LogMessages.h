// Copyright 2014 Xcalar, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Xcalar, Inc.
// Please refer to the included "COPYING" file for terms and conditions
// regarding the use and redistribution of this software.
//

#ifndef _LOGMESSAGES_H_
#define _LOGMESSAGES_H_

// XXX FIXME only put strings that appear in log messages in here
// XXX FIXME fix CAPS_UNDERSCORES to CamelCase

#define MSG_HELLO "Hello World!\n"
#define MSG_HELLO_FAILED_ALLOC "Failed to allocate string for hello\n"
#define MSG_HELLO_FAILED_GET "Failed to get message for hello: %s\n"
#define MSG_ASSERT_FAILURE "*** Assertion %s failed @ %s:%u\n"
#define MSG_NULL_FMT "(fmt null)\n"
#define MSG_LOGTEST_CHAR_FMT "log test char: %c\n"
#define MSG_LOGTEST_INT_FMT "log test int: %d\n"
#define MSG_LOGTEST_LONG_FMT "log test long: %ld\n"
#define MSG_LOGTEST_HEX_FMT "log test hex: 0x%x\n"
#define MSG_LOGTEST_LONG_HEX_FMT "log test long hex: 0x%lx\n"
#define MSG_LOGTEST_PTR_FMT "log test ptr: %p\n"
#define MSG_LOGTEST_STR_FMT "log test string_t: %s\n"
#define MSG_LOGTEST_UID "log test uid: %U\n"
#define MSG_LOGTEST_MISC_FMT1 "log test %-04u%%\n"
#define MSG_TUPLEGEN_KEYVAL_FMT "%s,http://www.youtube.com/videos/%x\n"
#define MSG_IOGEN_USAGE "Usage:\n\tioGen <deviceUid>\n\nAvailable Devices:\nNAME\t\tUID\t\n"
#define MSG_IOGEN_DEVICE_ENTRY "%s\t%U\n"
#define MSG_IOGEN_UIDLIST_FAILURE "Failed to get logical device uid list: %s\n"
#define MSG_IOGEN_OPEN_FAILURE "Failed to open logical device %U: %s\n"
#define MSG_IOGEN_GETINFO_FAILURE "Failed to get info on logical device %U: %s\n"
#define MSG_IOGEN_STRALLOC_FAILURE "Failed to allocate uid string\n"
#define MSG_IOGEN_MEMALLOC_FAILURE "Failed to allocate memory for i/o\n"
#define MSG_IOGEN_SEM_INIT_FAILURE "Failed to initialize semaphore: %s\n"
#define MSG_IOGEN_GETTIME_FAILURE "Failed to acquire time: %s\n"
#define MSG_IOGEN_ISSUE_FAILURE "Failed to issue i/o: %s\n"
#define MSG_IOGEN_STR_CONVERT_FAILURE "Failed to convert C string to Xcalar string: %s\n"
#define MSG_IOGEN_UID_CONVERT_FAILURE "Failed to convert string %s into a uid: %s\n"
#define MSG_IOGEN_THR_INIT_FAILURE "Failed to initialize thread attributes: %s\n"
#define MSG_IOGEN_SET_DETACH_FAILURE "Failed to set joinable attribute: %s\n"
#define MSG_IOGEN_THR_CREATE_FAILURE "Failed to create thread: %s\n"
#define MSG_LISTTEST_ITEM "Employee id %d salary %u\n"
#define MSG_UTILTEST_NEWBAR_EXISTS "Refusing to add Bar a:%d b:%d because an existing Bar with the same a already exists\n"
#define MSG_UTILTEST_NEWBAR_INSERTED "Inserting Bar a:%d b:%d into hash\n"
#define MSG_UTILTEST_REPLACED_BAR "Replacement Bar a:%d b:%d old b was:%d\n"
#define MSG_UTILTEST_NOT_REPLACED_BAR "Did not replaceme Bar a:%d b:%d\n"
#define MSG_UTILTEST_BAR_REMOVE "Removing Bar a:%d b:%d\n"
#define MSG_UTILTEST_BAR_ITERATED "Iterating Bar a:%d b:%d from hash\n"
#define MSG_STRTEST_INSTRUCT_FMT "in struct string test: %s\n"
#define IP0_192_168_1_24 "192.168.1.24"
#define IP1_192_168_1_24 "192.168.1.24"
#define IP2_192_168_1_24 "192.168.1.24"
#define IP3_192_168_1_24 "192.168.1.24"
#define IP4_192_168_1_24 "192.168.1.24"
#define IP5_192_168_1_24 "192.168.1.24"
#define IP6_192_168_1_24 "192.168.1.24"
#define IP7_192_168_1_24 "192.168.1.24"
#define IP0_192_168_1_24 "192.168.1.24"
#define IP1_192_168_1_36 "192.168.1.36"
#define IP2_192_168_1_36 "192.168.1.36"
#define IP3_192_168_1_36 "192.168.1.36"
#define IP4_192_168_1_36 "192.168.1.36"
#define IP5_192_168_1_36 "192.168.1.36"
#define IP6_192_168_1_36 "192.168.1.36"
#define IP7_192_168_1_36 "192.168.1.36"
#define IP0_192_168_1_37 "192.168.1.37"
#define IP1_192_168_1_37 "192.168.1.37"
#define IP2_192_168_1_37 "192.168.1.37"
#define IP3_192_168_1_37 "192.168.1.37"
#define IP4_192_168_1_37 "192.168.1.37"
#define IP5_192_168_1_37 "192.168.1.37"
#define IP6_192_168_1_37 "192.168.1.37"
#define IP7_192_168_1_37 "192.168.1.37"
#define DEV_sdc "2f6465762f736463000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEV_sdd "2f6465762f736464000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEV_sde "2f6465762f736465000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEV_sdf "2f6465762f736466000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEV_sdg "2f6465762f736467000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEV_sdh "2f6465762f736468000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEV_sdi "2f6465762f736469000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define DEV_sdj "2f6465762f73646a000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define FLASH_DEV_fioa "2f6465762f66696f610000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
#define MSG_STRTEST_STRTOLL1 "-584"
#define MSG_STRTEST_STRTOLL2 "0xdeadbeef"
#define MSG_STRTEST_STRTOLL3 "0574"
#define MSG_LIBLDM_TEST_FAKE_NAME "XcalarDB device 0xdeadbeef"
#define MSG_LIBLDM_TEST_FAKE_VENDOR "Xcalar, Inc."
#define MSG_LIBLDM_TEST_FAKE_PRODUCT "XcalarDB Super-LUN Big Data Storage"
#define MSG_LIBLDM_TEST_FAKE_VER "0.1.0.0"
#define MSG_LIBLDM_DEV "Discovered device: %U\n"
#define MSG_LIBLDM_NAME "    Name: %s\n"
#define MSG_LIBLDM_VENDOR "    Vendor: %s\n"
#define MSG_LIBLDM_PRODUCT "    Product: %s\n"
#define MSG_LIBLDM_VER "    Version: %s\n"
#define SOCK_LOCALHOST "localhost"
#define SOCK_ADDR_ANY "SockIPAddrAny"
#define MH_PARAM_DATA "--data"
#define MH_PARAM_METADATA "--meta"
#define MH_PARAM_METARATIO "--metaratio"
#define MH_PARAM_THREADS "--threads"
#define MH_PARAM_READPCT "--readpct"
#define MH_PARAM_RUNTIME "--runtime"
#define MH_PARAM_TOUCHSIZE "--touchsize"

#endif
