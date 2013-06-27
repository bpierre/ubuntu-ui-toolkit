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
    id: visuals
    anchors.fill: parent

    // styling properties
    property url borderSource
    property url borderPressed

    // pick either a clear or dark text color depending on the luminance of the
    // background color to maintain good contrast (works in most cases)
    function __luminance(hexcolor){
        hexcolor = String(hexcolor)
        var r = parseInt(hexcolor.substr(1,2),16);
        var g = parseInt(hexcolor.substr(3,2),16);
        var b = parseInt(hexcolor.substr(5,2),16);
        return ((r*212)+(g*715)+(b*73))/1000/255;
    }

    UbuntuShape {
        id: border

        anchors.fill: parent
        color: item.color
        borderSource: visuals.borderSource
        opacity: 1.0 - borderPressed.opacity
        visible: color.a != 0.0
    }

    UbuntuShape {
        id: borderPressed

        anchors.fill: parent
        color: item.color
        borderSource: visuals.borderPressed
        opacity: item.pressed ? 1.0 : 0.0
        Behavior on opacity { UbuntuNumberAnimation {} }
        visible: color.a != 0.0
    }

    TransparentButtonDelegate {
        textColor: __luminance(item.color) <= 0.72 ? Qt.rgba(0.95, 0.95, 0.91, 1.0) : Qt.rgba(0.2, 0.2, 0.2, 0.8)//"#333333"
    }
}
