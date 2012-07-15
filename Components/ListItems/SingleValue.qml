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
import ".."

/*!
    \qmlclass SingleValue
    \inqmlmodule UbuntuUIToolkit
    \brief The SingleValue class is DOCME

    \b{This component is under heavy development.}

*/
Selectable {
    id: listItem
    height: 54

    property alias iconSource: iconHelper.source
    property alias text: label.text
    property alias progression: progressionHelper.visible
    property alias value: valueLabel.text

    BackgroundVisual { anchors.fill: parent }

    Row {
        anchors.fill: parent
        IconVisual { id: iconHelper; height: parent.height }
        Item {
            width: parent.width - iconHelper.width - progressionHelper.width
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            LabelVisual {
                id: label
                selected: listItem.selected
                anchors {
                    verticalCenter: parent.verticalCenter
                    leftMargin: 5
                    left: parent.left
                }
                width: Math.min(label.implicitWidth, parent.width - 10)
                height: implicitHeight
            }
            LabelVisual {
                id: valueLabel
                selected: listItem.selected
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: 5
                }
                width: Math.min(implicitWidth, parent.width - label.width - 15)
                fontSize: "medium"
            }
        }
        ProgressionVisual {
            id: progressionHelper
            showSplit: true
        }
    }
}
