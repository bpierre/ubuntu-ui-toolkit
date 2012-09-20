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
import Ubuntu.Components 0.1

/*!
    \qmlclass Empty
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief A list item with no contents.
    The Empty class can be used for generic list items, containing
    other components such as buttons. It draws an upper and lower
    divider, is selectable, and can take mouse clicks.
    For specific types of list items, see its subclasses.

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
            ListItem.Empty {
                height: 50
                Text {
                    text: "Hello world"
                    anchors.centerIn: parent
                }
            }
            ListItem.Empty {
                height: 100
                Tabs {
                    height: parent.height - 20
                    width: parent.width - 20
                    anchors.centerIn: parent
                    Tab {
                        text: "Tab 1"
                        page: TextCustom {
                            anchors.centerIn: parent
                            text: "Page one"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 2"
                        page: TextCustom {
                            anchors.centerIn: parent
                            text: "Page two"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 3"
                        page: TextCustom {
                            anchors.centerIn: parent
                            text: "Page three"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                }
            }
        }
    \endqml

    See the documentation of the derived classes of Empty for more examples.
    \b{This component is under heavy development.}
*/
AbstractButton {
    id: emptyListItem
    width: parent ? parent.width : 250

    /*!
      \preliminary
      Specifies whether the list item is selected.
     */
    property bool selected: false

    /*!
      \preliminary
      Highlight the list item when it is pressed.
      This is used to disable the highlighting of the full list item
      when custom highlighting needs to be implemented (for example in
      ListItem.Standard which can have a split).
    */
    property bool highlightWhenPressed: true

    Rectangle {
        id: highlight
        visible: emptyListItem.selected || (emptyListItem.highlightWhenPressed && emptyListItem.pressed)
        anchors.fill: parent
        color: "white"
        opacity: 0.7
    }

    /*!
      \internal
      Override in divider classes that should never
      draw the one-pixel divider at the top or bottom
      of the list item.
     */
    property bool __isDivider: false

    /*!
      \internal
     */
    property bool __showTopSeparator: __separateAtTop()

    /*!
      \internal
     */
    property bool __showBottomSeparator: __separateAtBottom()

    /*!
      \internal
      Retun the index of the given item in the list of
      parent's children.
     */
    function __childIndexOf(item) {
        if (!parent) return undefined;
        var index = parent.children.length - 1;
        while (index >= 0 && item !== parent.children[index]) {
            index--;
        }
        if (index === -1) return undefined;
        return index;
    }

    /*!
      \internal
      Determine whether the bottom separator must be shown.
      This is always the case, if the next item in the list of children
      is not a divider.
     */
    function __separateAtBottom() {
        var index = emptyListItem.__childIndexOf(emptyListItem);
        if (index === undefined) return true;
        // index is defined:
         if (index < parent.children.length - 1) {
            if (parent.children[index+1].__isDivider) return false;
        }
        return true;
    }

    /*!
      \internal
      Determine whether the top separator must be shown.
      This is the case, if the list item is not a divider, and
      it is the first in the list of the children.
    */
    function __separateAtTop() {
        var index = emptyListItem.__childIndexOf(emptyListItem);
        if (index === undefined) return true;
        // index is defined:
        if (emptyListItem.__isDivider || index !== 0) return false;
        return true;
    }

    Image {
        id: topSeparatorLine
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: visible ? 2 : 0
        source: "artwork/ListItemDividerHorizontal.png"
        visible: emptyListItem.__showTopSeparator
    }
    Image {
        id: bottomSeparatorLine
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: visible ? 2 : 0
        source: "artwork/ListItemDividerHorizontal.png"
        visible: emptyListItem.__showBottomSeparator
    }

    /*!
      \internal
      Reparent so that the visuals of the children do not
      occlude the separator lines.
     */
    default property alias children: body.children
    Item {
        id: body
        anchors {
            left: parent.left
            right: parent.right
            top: topSeparatorLine.bottom
            bottom: bottomSeparatorLine.top
        }
    }
}
