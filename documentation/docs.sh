#!/bin/sh
#
# Copyright 2012 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

DOC_PATH=$1
if [ ! $SRC -o ! $DOC_PATH ]; then
    echo "Error - SRC and DOC_PATH must be defined"
    exit 1
fi

# Offline docs for QtCreator
qdoc $DOC_PATH/ubuntu-ui-toolkit-qtcreator.qdocconf 2> $DOC_PATH/qdoc.log
# FIXME: With Qt 5.2 this warning shows up, forcibly omit it from errors
cat $DOC_PATH/qdoc.log | grep -v "qdoc: warning: No documentation for 'global'" > $DOC_PATH/qdoc.err
test ! -s $DOC_PATH/qdoc.err || exit 1
echo docs: Offline done.

qhelpgenerator -o "$DOC_PATH/html/ubuntuuserinterfacetoolkit.qch" "$DOC_PATH/html/ubuntuuserinterfacetoolkit.qhp"
echo docs: qch done.

# Online docs. Run qdoc twice: the second run with indexes for cross-referencing
# other APIs but discard errors because qdoc inherits all doc bugs otherwise
qdoc $DOC_PATH/ubuntu-ui-toolkit-online.qdocconf 2> $DOC_PATH/qdoc.log
cat $DOC_PATH/qdoc.log | grep -v "qdoc: warning: No documentation for 'global'" > $DOC_PATH/qdoc.err
test ! -s $DOC_PATH/qdoc.err || exit 1
echo docs: Online done.

qdoc $DOC_PATH/ubuntu-ui-toolkit-online-indexes.qdocconf 2> /dev/null
echo docs: Cross-referenced done.

sed -r -i 's@("main-content">)@\1<ul class="breadcrumb">@g' $1/html/*.html
sed -r -i 's@(<div class="toc">|<h1 class="title">)@</ul>\1@g' $1/html/*.html
sed -r -i 's@(</p>)<p/>@\1@g' $1/html/*.html
sed -r -i 's@(<a class="(next|prev)Page" href=".+">)(.+)(</a>)@<li>\1\3\4</li>@g' $1/html/*.html
sed -r -i 's@(href="qmlmodule-ubuntu-components0-ubuntu-components-0-1.html">Basic QML Types)@href="overview-ubuntu-sdk.html">Ubuntu User Interface Toolkit@g' $1/html/*.html
echo docs: HTML fixes done.
