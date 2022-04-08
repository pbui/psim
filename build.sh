#!/bin/sh

#-------------------------------------------------------------------------------
# build.sh: Makefile front-end
#-------------------------------------------------------------------------------

# Copyright (c) 2007 Peter Bui. All Rights Reserved.

# This software is provided 'as-is', without any express or implied warranty.
# In no event will the authors be held liable for any damages arising from the
# use of this software.

# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:

# 1. The origin of this software must not be misrepresented; you must not claim
# that you wrote the original software. If you use this software in a product,
# an acknowledgment in the product documentation would be appreciated but is
# not required.

# 2. Altered source versions must be plainly marked as such, and must not be
# misrepresented as being the original software.  

# 3. This notice may not be removed or altered from any source distribution.

# Peter Bui <pbui@cse.nd.edu>

#-------------------------------------------------------------------------------

PRJNAME="psim"
PKGNAME="$PRJNAME-$(date +%Y%m%d)"

#-------------------------------------------------------------------------------

function BUILD() {
    env BCFLAGS="$BCFLAGS" BLINKFLAGS="$BLINKFLAGS" make -f Makefile $*
}

#-------------------------------------------------------------------------------

case $1 in
    debug)
    shift 1
    BCFLAGS='-g -D__DEBUG__'
    BUILD $* || exit 1
    ;;
    package)
    echo "Building $PKGNAME.tar.gz"
    (cd ..; git-archive --format=tar --prefix=$PKGNAME/ HEAD | gzip > ../$PKGNAME.tar.gz)
    ;;
    *)
    BCFLAGS='-s -O2'
    BUILD $* || exit 1
    ;;
esac
    
exit 0

#-------------------------------------------------------------------------------
# vim: sts=4 sw=4 ts=8 ft=sh
#-------------------------------------------------------------------------------
