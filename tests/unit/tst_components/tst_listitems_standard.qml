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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Item {
    width: 200
    height: 200

    Rectangle {
        id: testItem
    }

    AbstractButton {
        id: testControl
    }

    ListItem.Standard {
        id: listItemStandard
        anchors.fill: parent
    }


    TestCase {
        name: "ListItemsStandardAPI"
        when: windowShown

        function test_fallbackIconSource() {
            expectFail("","https://bugs.launchpad.net/tavastia/+bug/1076762")
            compare(listItemStandard.fallbackIconSource,undefined,"fallbackIconSource is not set by default")
        }

        function test_control() {
            listItemStandard.control = testControl
            compare(listItemStandard.control,testControl,"set/get")
            listItemStandard.control = null
            compare(listItemStandard.control, null, "unset")
        }

        function test_clicked() {
            compare(listItemClickedSpy.valid, true, "clicked signal exists")
            compare(controlClickedSpy.valid, true, "control has clicked signal")
            listItemStandard.control = testControl;
            compare(listItemStandard.control, testControl, "control can be set")
            var listItemClickedCount = listItemClickedSpy.count
            var controlClickedCount = controlClickedSpy.count
            mouseMove(listItemStandard, 10, 10)
            mouseClick(listItemStandard, 10, 10)
            compare(listItemClickedSpy.count, listItemClickedCount+1, "List item clicked triggered")
            compare(listItemClickedSpy.count, controlClickedCount+1, "Control clicked triggered")
        }

        function test_icon() {
            compare(listItemStandard.icon,undefined,"icon is not set by default")

            // test with item
            listItemStandard.icon = testItem
            compare(listItemStandard.icon,testItem,"set/get from Item")

            // test with url
            var newIcon = "../../../demos/small_avatar.png"
            listItemStandard.icon = newIcon
            compare(listItemStandard.icon,newIcon,"set/get from url")
        }

        function test_iconFrame() {
            compare(listItemStandard.iconFrame,true,"iconFrame is true by default")
        }

        function test_progression() {
            compare(listItemStandard.progression,false,"progression is false by default")
        }

        function test_text() {
            compare(listItemStandard.text,"","text is '' by default")
            var newText = "Hello World!"
            listItemStandard.text = newText
            compare(listItemStandard.text,newText,"set/get")
        }

        SignalSpy {
            id: listItemClickedSpy
            target: listItemStandard
            signalName: "clicked"
        }

        SignalSpy {
            id: controlClickedSpy
            target: testControl
            signalName: "clicked"
        }
    }
}
