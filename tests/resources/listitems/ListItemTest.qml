/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.2

MainView {
    id: main
    width: units.gu(50)
    height: units.gu(100)

    property bool override: false

    Action {
        objectName: "stock"
        id: stock
        iconName: "starred"
        text: "Staaaar"
        onTriggered: print(iconName, "triggered", value)
    }

    ListItemOptions {
        id: leading
        objectName: "StockLeading"
        Action {
            iconName: "delete"
            onTriggered: print(iconName, "triggered", value)
        }
        Action {
            iconName: "alarm-clock"
            enabled: false
            onTriggered: print(iconName, "triggered")
        }
        Action {
            iconName: "camcorder"
            onTriggered: print(iconName, "triggered", value)
        }
        Action {
            iconName: "stock_website"
            onTriggered: print(iconName, "triggered", value)
        }
    }

    property bool selectable: false
    Column {
        anchors {
            left: parent.left
            right: parent.right
        }

        Button {
            text: "Selectable " + (selectable ? "OFF" : "ON")
            onClicked: selectable = !selectable
        }

        ListItem {
            id: testItem
            objectName: "single"
            selectable: main.selectable
            color: "lime"
            onClicked: {
                print("click")
                main.override = !main.override
            }
            Label {
                anchors.fill: parent
                text: units.gridUnit + "PX/unit"
            }
            leadingOptions: ListItemOptions {
                objectName: "InlineLeading"
                options: [stock]
                delegate: Column {
                    width: height + units.gu(2)
                    Icon {
                        width: units.gu(3)
                        height: width
                        name: option.iconName
                        color: "blue"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        text: option.text + index
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
            trailingOptions: leading
        }

        ListView {
            id: view
            clip: true
            width: parent.width
            height: units.gu(20)
            model: 10
            pressDelay: 0
            delegate: ListItem {
                objectName: "ListItem" + index
                id: listItem
                selectable: main.selectable
                selected: true
                onClicked: print(" clicked")
                leadingOptions: leading
                Label {
                    text: modelData + " item"
                }
                states: State {
                    name: "override"
                    when: main.override
                    PropertyChanges {
                        target: listItem
                        pressedColor: "brown"
                    }
                }
            }
        }
        Flickable {
            id: flicker
            width: parent.width
            height: units.gu(20)
            clip: true
            contentHeight: column.childrenRect.height
            Column {
                id: column
                width: view.width
                property alias count: repeater.count
                Repeater {
                    id: repeater
                    model: 10
                    ListItem {
                        objectName: "InFlickable"+index
                        selectable: main.selectable
                        leadingOptions: ListItemOptions {
                            Action {
                                iconName: "edit"
                                onTriggered: print(iconName, "triggered", value)
                            }
                            Action {
                                iconName: "delete"
                                onTriggered: print(iconName, "triggered", value)
                            }
                        }
                        trailingOptions: ListItemOptions {
                            options: leadingOptions.options
                        }

                        color: "red"
                        pressedColor: "lime"
                        divider.colorFrom: UbuntuColors.green

                        Label {
                            text: modelData + " Flickable item"
                        }
                        onClicked: divider.visible = !divider.visible
                    }
                }
            }
        }
    }
}
