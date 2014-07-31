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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 1.1

Item {
    width: units.gu(50)
    height: units.gu(50)

    Row {
        anchors.centerIn: parent
        height: units.gu(10)
        width: childrenRect.width

        Icon {
            // Fails to load the icon when suru-icon-theme or libqt5svg5 are
            // not installed. This causes a warning and rejection
            // by jenkins continuous integration.
            name: "add"
            height: parent.height
            width: height
        }
        Icon {
            // Fails when the icon is becoming invisible when non-atomic updates
            // cause sourceSize.width or sourceSize.height to be 0 before other
            // properties are updated.
            id: icon
            width: visible ? units.gu(10) : 0
            height: width
            name: "search"
        }
    }

    TestCase {
        name: "Icon"
        when: windowShown

        function test_updateIconSize_bug1349769() {
            icon.visible = false;
            // causes "QML Image: Failed to get image from provider: image://theme/search"
            // warning when sourceSize.width or sourceSize.height becomes 0 while
            // while still trying to render the icon. Tests will pass with the warning, but
            // the MR is rejected by jenkins continuous integration.
        }
    }
}
