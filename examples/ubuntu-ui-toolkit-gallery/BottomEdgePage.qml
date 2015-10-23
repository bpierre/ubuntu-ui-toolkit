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
import Ubuntu.Components 1.3

Page {
    id: page
    TemplateSection {
        title: "Bottom Edge Hint"
        className: "BottomEdgeHint"

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }
    }

    Component {
        id: fake
        Rectangle {
            width: units.gu(40)
            height: page.height
            color: "blue"
        }
    }
    Component {
        id: bottomEdge
        Rectangle {
            width: units.gu(40)
            height: page.height
            color: UbuntuColors.green
        }
    }

    BottomEdge {
        visible: active
        hint: BottomEdgeHint {
            text: "Compose a new message"
            iconName: "stock_message"
        }
        anchors.fill: parent
        contentComponent: bottomEdge
    }
}
