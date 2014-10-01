/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1

Item {
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        id: mainView
        anchors.fill: parent
        useDeprecatedToolbar: false

        Page {
            id: page
            title: "Page title"
            head.actions: [
                Action {
                    objectName: "selectAction"
                    text: "Select all"
                    iconName: "select"
                }
            ]
        }
    }

    UbuntuTestCase {
        name: "HeaderSelectionPreset"
        when: windowShown
        id: testCase

        property var action_button

        property var app_header
        property var back_button
        property var custom_back_button
        property var head_style

        function initTestCase() {
            testCase.action_button = findChild(mainView, "selectAction_header_button");
            compare(action_button.visible, true, "Header action button is not visible.");
            compare(page.head.preset, "", "Default header preset is not empty string.");
        }

        function test_default_preset_no_label() {
            var label = findChild(action_button, "selectAction_header_button_label");
            compare(label, null, "Header button label defined in default header.");
        }

        function test_select_preset_has_label() {
            page.head.preset = "select";
            var label = findChild(action_button, "selectAction_header_button_label");
            compare(label.visible, true, "Header button has no visible label in selection mode.");
            page.head.preset = "";
        }
    }
}
