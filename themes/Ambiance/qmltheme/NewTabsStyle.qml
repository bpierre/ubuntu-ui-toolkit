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

QtObject {
    /*!
      If this optional property is specified, it will be positioned
      between the bar with tab buttons, and the tab pages to act
      as a separator.
     */
    property Item separator

    /*!
      Enable left/right swiping in the contents area to go to
      the next/previous tab.
     */
    property bool swipeToSwitchTabs

    property color headerTextColor
    property real headerTextOpacity
    property color headerTextSelectedColor
    property real headerTextSelectedOpacity

    property int headerTextFadeDuration
    property string headerFontSize
    property int headerFontWeight
    property real headerTextLeftMargin
    property real headerTextRightMargin
    property real headerTextBottomMargin

    property url indicatorImageSource

    property real tabBarHeight

    /*!
      The time of inactivity in ms before the tab bar is automatically deactivated
     */
    property int deactivateTime

    /*!
      The velocity at which buttons are repositioned when a new tab gets selected.
     */
    property real buttonPositioningVelocity

    Component.onCompleted: print("NewTabsStyle deprecated")
}
