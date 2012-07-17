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
import "../Components"
import "../Components/ListItems" as ListItem

Template {
    title: "Basic List Items"

    Row {
        spacing: 10

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height

            ListItem.Container {
                width: 250

                ListItem.Header { text: "Standard" }

                ListItem.Default {
                    text: "Default"
                }
                ListItem.Default {
                    text: "Disabled"
                    enabled: false
                }
                ListItem.Default {
                    text: "Selected"
                    selected: true
                }
                ListItem.Default {
                    text: "Progression"
                    progression: true
                }
                ListItem.Default {
                    text: "Icon"
                    iconSource: "avatar_contacts_list.png"
                }
            }
        }

        Rectangle {
            color: "#eeeeee"
            width: childrenRect.width
            height: childrenRect.height

            ListItem.Container {
                width: 250

                ListItem.Header { text: "Single value" }

                ListItem.SingleValue {
                    text: "Default"
                    value: "Value"
                }
                ListItem.SingleValue {
                    text: "Disabled"
                    value: "Value"
                    enabled: false
                }
                ListItem.SingleValue {
                    text: "Selected"
                    value: "Value"
                    selected: true
                }
                ListItem.SingleValue {
                    text: "Progression"
                    value: "Value"
                    progression: true
                }
                ListItem.SingleValue {
                    text: "Icon"
                    value: "Value"
                    iconSource: "avatar_contacts_list.png"
                }
            }
        }
    }
}
