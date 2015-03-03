/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Components 1.2

TestCase {
    name: "FontUtilsAPI"

    function test_modularScale() {
        compare(FontUtils.modularScale("xx-small"), 0.677, "xx-small scale");
        compare(FontUtils.modularScale("x-small"), 0.804, "x-small scale");
        compare(FontUtils.modularScale("small"), 0.931, "small scale");
        compare(FontUtils.modularScale("medium"), 1.079, "medium scale");
        compare(FontUtils.modularScale("large"), 1.291, "large scale");
        compare(FontUtils.modularScale("x-large"), 1.714, "x-large scale");
    }

    function test_modularScale_failures() {
        compare(FontUtils.modularScale("xxsmall"), 0.0, "xxsmall not defined");
        compare(FontUtils.modularScale("undefined"), 0.0, "undefined scale");
    }

    function test_sizeToPixels() {
        compare(FontUtils.sizeToPixels("xx-small"), 0.677 * units.dp(14), "xx-small scale");
        compare(FontUtils.sizeToPixels("x-small"), 0.804 * units.dp(14), "x-small scale");
        compare(FontUtils.sizeToPixels("small"), 0.931 * units.dp(14), "small scale");
        compare(FontUtils.sizeToPixels("medium"), 1.079 * units.dp(14), "medium scale");
        compare(FontUtils.sizeToPixels("large"), 1.291 * units.dp(14), "large scale");
        compare(FontUtils.sizeToPixels("x-large"), 1.714 * units.dp(14), "x-large scale");
    }

    function test_sizeToPixels_failures() {
        compare(FontUtils.sizeToPixels("xxsmall"), 0.0, "xxsmall not defined");
        compare(FontUtils.sizeToPixels("undefined"), 0.0, "undefined scale");
    }

}
