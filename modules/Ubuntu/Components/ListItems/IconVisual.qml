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

import QtQuick 1.1

// internal helper class to create the visuals
// for the icon.
Item {
    id: iconVisual

    property alias source: icon.source
    visible: source != "" && icon.status == Image.Ready

    width: visible ? icon.width + 10 : 0
    height: icon.height + icon.anchors.topMargin

    Image {
        id: icon
        width: 36
        opacity: iconVisual.enabled ? 1.0 : 0.5
        fillMode: Image.PreserveAspectFit
        anchors {
            top: parent.top
            topMargin: 5
            left: parent.left
            leftMargin: 5
        }
    }

    BorderImage {
        id: iconFrame
        opacity: iconVisual.enabled ? 1.0 : 0.5
        source: "artwork/ListItemFrame.png"
        anchors.fill: icon
        anchors.bottomMargin: -1
        border {
            left: 3
            right: 3
            top: 3
            bottom: 3
        }
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch
    }
}
