#!/usr/bin/env python3.6

# Copyright 2018 Xcalar, Inc. All rights reserved.
#
# No use, or distribution, of this source code is permitted in any form or
# means without a valid, written license agreement with Xcalar, Inc.
# Please refer to the included "COPYING" file for terms and conditions
# regarding the use and redistribution of this software.
#

# Given XLRPASSWORD and a public key, this will generate LicenseEmbedded.cpp
# in liblicense that will be compiled as part of liblicense in order to burn
# both the public key and the XLRPASSWORD into the binary. Some obfuscation
# code is generated to thwart basic reverse engineering attempts to run
# string on the usrnode binary to obtain the password or public key.
#

import argparse
import base64

commonHeader="""// Copyright 2018 Xcalar, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Xcalar, Inc.
// Please refer to the included "COPYING" file for terms and conditions
// regarding the use and redistribution of this software.
//
//
// **********************************************************************
// *** DO NOT EDIT!  This file was autogenerated by burnPubKey.py     ***
// **********************************************************************
//
"""

headerFileContents=commonHeader + """
#ifndef LICENSEEMBEDDED_H_
#define LICENSEEMBEDDED_H_

class LicenseEmbedded final {
public:
    static MustCheck Status init(void);
    static void destroy() {}
    static const char *getPassword(void) {
        return password_;
    }

    // Note that the key below should only be used to verify
    // signatures from Xcalar, and never to decrypt information
    // We do not mix signing keys from encryption keys.
    static const char *getPubSigningKey(void) {
        return pubSigningKey_;
    }

private:
    static char password_[%d];
    static char pubSigningKey_[%d];

    // Keep this private, use init instead
    LicenseEmbedded() {}

    // Keep this private, use destroy instead
    ~LicenseEmbedded() {}
    LicenseEmbedded(const LicenseEmbedded&) = delete;
    LicenseEmbedded(const LicenseEmbedded&&) = delete;
    LicenseEmbedded& operator=(const LicenseEmbedded&) = delete;
    LicenseEmbedded& operator=(const LicenseEmbedded&&) = delete;
};

#endif /* LICENSEEMBEDDED_H_ */
"""

def genSourceFile(sourceFile, password, pubKey):
    sourceFileContents = commonHeader + """#include "Primitives.h"
#include "LicenseEmbedded.h"
    """

    sourceFileContents += "char LicenseEmbedded::password_[%d];\n" % (len(password) + 1)
    sourceFileContents += "char LicenseEmbedded::pubSigningKey_[%d];\n" % (len(pubKey) + 1)
    sourceFileContents += "\nStatus\nLicenseEmbedded::init(void)\n{\n"
    # Generate password
    sourceFileContents += "    // Generate password\n"
    for (ii, char) in enumerate(password):
        sourceFileContents += "    password_[%d] = %d;\n" % (ii, ord(char))
    sourceFileContents += "    password_[%d] = 0;\n" % len(password)

    # Generate pubkey
    sourceFileContents += "\n    // Generate pubSigningKey\n"
    for (ii, char) in enumerate(pubKey):
        sourceFileContents += "    pubSigningKey_[%d] = %d;\n" % (ii, char)
    sourceFileContents += "    pubSigningKey_[%d] = 0;\n" % len(pubKey)

    sourceFileContents += "\n    return (StatusOk);\n}"
    with open(sourceFile, 'w') as fp:
        fp.write(sourceFileContents)

def genHeaderFile(headerFile, password, pubKey):
    with open(headerFile, 'w') as fp:
        fp.write(headerFileContents % (len(password) + 1, len(pubKey) + 1))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="PubKey Code Generator")
    parser.add_argument('-k', '--keypath', help="Path to public key", required=True)
    parser.add_argument('-p', '--password', help="Secret Xcalar Password", required=True)
    parser.add_argument('-c', '--csrc', help="Generate a C++ source file")
    parser.add_argument('-r', '--chdr', help="Generate a C++ header file")
    args = parser.parse_args()

    password = args.password

    with open(args.keypath, 'rb') as fp:
        pubKey = base64.b64encode(fp.read())

    if (args.chdr):
        genHeaderFile(args.chdr, password, pubKey)

    if (args.csrc):
        genSourceFile(args.csrc, password, pubKey)
