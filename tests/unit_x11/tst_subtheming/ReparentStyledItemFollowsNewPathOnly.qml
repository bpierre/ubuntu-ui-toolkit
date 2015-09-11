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

StyledItem {
    width: units.gu(40)
    height: units.gu(40)

    ThemeSettings {
        objectName: "suruTheme"
        name: "Ubuntu.Components.Themes.SuruDark"
    }

    StyledItem {
        id: customThemed
        objectName: "customThemed"
        theme: ThemeSettings {
            name: "CustomTheme"
        }
    }

    StyledItem {
        id: movable
        objectName: "movable"
    }

    function test_reparent() {
        movable.parent = null;
    }
    function test_reparent_custom() {
        movable.parent = customThemed;
    }
}
