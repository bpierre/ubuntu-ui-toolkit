/*
 * Copyright 2012-2014 Canonical Ltd.
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
import Ubuntu.Components 1.1

Item {
    id: visuals
    // styling properties
    property int iconWidth: units.gu(3)
    property int iconHeight: units.gu(3)

    width: units.gu(5)
    height: units.gu(5)

    Icon {
        id: icon
        anchors {
            centerIn: parent
        }
        width: visuals.iconWidth
        height: visuals.iconWidth
        source: styledItem.iconSource
        opacity: styledItem.enabled ? 1.0 : 0.3

        color: styledItem.hasOwnProperty("color") ? styledItem.color : Qt.rgba(0, 0, 0, 0)
    }
}
