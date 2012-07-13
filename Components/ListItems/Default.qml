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
    \qmlclass Default
    \inqmlmodule UbuntuUIToolkit
    \brief The Default class is DOCME

    \b{This component is under heavy development.}

*/
Selectable {
    id: listItem
    height: 54

    property alias iconSource: iconHelper.source
    property alias text: label.text
    property alias progression: progressionHelper.visible

    Row {
        anchors.fill: parent
        IconHelper { id: iconHelper; height: parent.height }
        LabelHelper {
            id: label
            width: parent.width - iconHelper.width - progressionHelper.width
            selected: listItem.selected
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 20
        }
        ProgressionHelper { id: progressionHelper }
    }
}
