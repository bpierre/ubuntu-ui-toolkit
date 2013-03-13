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
import Ubuntu.Components 0.1

Item {
    anchors.fill: parent
    implicitWidth: units.gu(4)
    implicitHeight: units.gu(4)
    opacity: enabled ? 1.0 : 0.5

    UbuntuShape {
        anchors.fill: parent
        color: item.checked ? StyleUtils.itemStyleProperty("checkedColor") : StyleUtils.itemStyleProperty("uncheckedColor")
        gradientColor: "transparent"
        Behavior on color {
            ColorAnimation {
                duration: StyleUtils.itemStyleProperty("backgroundColorAnimation").duration
                easing: StyleUtils.itemStyleProperty("backgroundColorAnimation").easing
            }
        }
    }

    Image {
        id: checkMark
        anchors.centerIn: parent
        smooth: true
        source: StyleUtils.itemStyleProperty("checkMarkSource", "")
        opacity: item.checked ? 1.0 : 0.0
        Behavior on opacity {
            NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
        }
    }
}
