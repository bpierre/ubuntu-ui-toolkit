/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: root
    width: 400
    height: 600

    Action {
        id: action
    }
    Action {
        id: other
        shortcut: 'Ctrl+G'
    }

    TestUtil {
        id: util
    }

    UbuntuTestCase {
        id: testCase
        name: "Shortcuts"
        when: windowShown

        function initTestCase() {
        }

        function init() {
        }
        function cleanup() {
            spy.clear();
            shortcutSpy.clear();
        }

        SignalSpy {
            id: spy
            signalName: 'triggered'
            target: action
        }

        function ignoreQMLWarning(message) {
            ignoreWarning(util.callerFile() + message);
        }

        function test_shortcut_triggered_data() {
            return [
                { tag: 'Multiple modifiers and letter', shortcut: 'Ctrl+Shift+Alt+A', key: Qt.Key_A, mod: Qt.ControlModifier + Qt.ShiftModifier + Qt.AltModifier },
                { tag: 'Modifier and letter', shortcut: 'Ctrl+A', key: Qt.Key_A, mod: Qt.ControlModifier },
                { tag: 'Single letter', shortcut: 'E', key: Qt.Key_E, mod: Qt.NoModifier },
                { tag: 'StandardKey', shortcut: StandardKey.Copy, key: Qt.Key_C, mod: Qt.ControlModifier }
            ];
        }
        function test_shortcut_triggered(data) {
            action.shortcut = data.shortcut;
            spy.clear();
            keyClick(data.key, data.mod);
            spy.wait();
        }

        function test_shortcut_invalid_data() {
            return [
                { tag: 'Typo', shortcut: 'Ctr+F' },
                { tag: 'Number', shortcut: 1234567890 },
                { tag: "Empty string", shortcut: "" }
            ];
        }
        function test_shortcut_invalid(data) {
            ignoreQMLWarning(':27:5: QML Action: Invalid shortcut: ');
            action.shortcut = data;
        }

        function test_shortcut_duplicate() {
            ignoreQMLWarning(':30:5: QML Action: Ambiguous shortcut: Ctrl+G');
            action.shortcut = other.shortcut;
            keyClick(Qt.Key_G, Qt.ControlModifier);
        }

        SignalSpy {
            id: shortcutSpy
            signalName: "shortcutChanged"
        }

        function test_shortcut_reset_bug1518420() {
            shortcutSpy.target = action;
            action.shortcut = "Ctrl+A";
            shortcutSpy.wait(200);
            shortcutSpy.clear();
            action.shortcut = undefined;
            shortcutSpy.wait(200);
            shortcutSpy.target = null;
        }

        function test_mnemonic_data() {
            return [
                {tag: "HW keyboard, valid", kbd: true, text: "C&all", displayText: "C<u>a</u>ll"},
                {tag: "HW keyboard, invalid", kbd: true, text: "&&Call", displayText: "&&Call"},
                {tag: "no HW keyboard", kbd: false, text: "&Call", displayText: "Call"},
            ];
        }
        function test_mnemonic(data) {
            QuickUtils.keyboardAttached = data.kbd;
            action.text = data.text;
            if (!data.kbd && QuickUtils.keyboardAttached) {
                skip("Cannot test this case: " + data.tag);
            }
            compare(action.text, data.displayText);
        }

        function test_mnemonic_trigger_data() {
            return [
                        {tag: "valid", text: "C&all", key: Qt.Key_A, mod: Qt.AltModifier, xfail: false},
                        {tag: "invalid", text: "C&&all", key: Qt.Key_Asterisk, mod: Qt.AltModifier, xfail: true},
                    ];
        }
        function test_mnemonic_trigger(data) {
            action.text = data.text;
            keyClick(data.key, data.mod);
            if (data.xfail) {
                expectFail(data.tag, "invalid mnemonic");
            }
            spy.wait(200);
        }
    }
}
