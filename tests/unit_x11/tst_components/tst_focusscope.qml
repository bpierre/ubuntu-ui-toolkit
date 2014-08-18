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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1

Item {
    width: units.gu(50)
    height: units.gu(100)

    Column {
        anchors.fill: parent

        FocusScope {
            id: passiveScope
            objectName: "passiveScope"
            width: units.gu(50)
            height: units.gu(50)

            Rectangle {
                width: height
                height: units.gu(20)
                color: "red"
                anchors.centerIn: parent

                FocusScope {
                    objectName: "in_passive_scope"
                    activeFocusOnMousePress: true
                    anchors.fill: parent
                }
            }
        }
        FocusScope {
            id: activeScope
            objectName: "activeScope"
            width: units.gu(50)
            height: units.gu(50)
            activeFocusOnMousePress: true

            Rectangle {
                width: height
                height: units.gu(20)
                color: "green"
                anchors.centerIn: parent

                FocusScope {
                    objectName: "in_active_scope"
                    activeFocusOnMousePress: true
                    anchors.fill: parent
                }
            }
        }
    }

    UbuntuTestCase {
         name: "Ubuntu.Components.FocusScope API"
         when: windowShown

         function test_scope_focusing_data() {
             return [
                 {tag: "main scope is passive", mainScope: passiveScope, innerScope: "in_passive_scope", focusing: false},
                 {tag: "main scope is active", mainScope: activeScope, innerScope: "in_active_scope", focusing: true},
             ];
         }
         function test_scope_focusing(data) {
             var innerScope = findChild(data.mainScope, data.innerScope);
             verify(innerScope, "Cannot reach inner scope!");
             var center = centerOf(innerScope);
             mouseClick(innerScope, center.x, center.y);
             if (!data.focusing) {
                 expectFailContinue(data.tag, "inner scope should not focus either.");
             }
             compare(innerScope.activeFocus, true, "Inner scope is not focused");
         }
    }
}
