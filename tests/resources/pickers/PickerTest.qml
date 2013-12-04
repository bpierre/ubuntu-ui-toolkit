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
import Ubuntu.Components 0.1
import Ubuntu.Components.Pickers 0.1
Column{
    Row {
        ListModel {
            id: pickerModel
            Component.onCompleted: reset(100);

            function reset(num) {
                clear();
                for (var i = 0; i < num; i++) {
                    append({"label": "Item #"+i});
                }
            }
        }


        Picker {
            objectName: "Circular"
            id: circularPicker
            height: units.gu(40)
            model: pickerModel
            delegate: PickerDelegate {
                Label {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: modelData
                }
            }
            onSelectedIndexChanged: print("circular index="+selectedIndex)
        }

        Picker {
            objectName: "Linear"
            id: linearPicker
            height: units.gu(40)
            model: pickerModel
            circular: false
            delegate: PickerDelegate {
                Label {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: modelData
                }
            }
            onSelectedIndexChanged: print("linear index="+selectedIndex)
        }

//        Picker {
//            height: units.gu(40)
//            model: pickerModel
//            live: true
//            delegate: PickerDelegate {
//                Label {
//                    anchors.fill: parent
//                    horizontalAlignment: Text.AlignHCenter
//                    verticalAlignment: Text.AlignVCenter
//                    text: modelData
//                }
//            }
//            onSelectedIndexChanged: print("circular index="+selectedIndex)
//        }

//        Picker {
//            height: units.gu(40)
//            model: pickerModel
//            circular: false
//            live: true
//            delegate: PickerDelegate {
//                Label {
//                    anchors.fill: parent
//                    horizontalAlignment: Text.AlignHCenter
//                    verticalAlignment: Text.AlignVCenter
//                    text: modelData
//                }
//            }
//            onSelectedIndexChanged: print("linear index="+selectedIndex)
//        }
    }
    Button {
        text: "crop"
        onClicked: {
            pickerModel.remove(50, 50);
            print("circular: selectedIndex="+circularPicker.selectedIndex+
                  ", currentIndex="+circularPicker.itemList.currentIndex)
            print("linear: selectedIndex="+linearPicker.selectedIndex+
                  ", currentIndex="+linearPicker.itemList.currentIndex)
        }
    }
    Button {
        text: "reset"
        onClicked: {
            pickerModel.reset(100);
            print("circular: selectedIndex="+circularPicker.selectedIndex+
                  ", currentIndex="+circularPicker.itemList.currentIndex)
            print("linear: selectedIndex="+linearPicker.selectedIndex+
                  ", currentIndex="+linearPicker.itemList.currentIndex)
        }
    }
    Button {
        text: "clear"
        onClicked: {
            pickerModel.clear();
            print("circular: selectedIndex="+circularPicker.selectedIndex+
                  ", currentIndex="+circularPicker.itemList.currentIndex)
            print("linear: selectedIndex="+linearPicker.selectedIndex+
                  ", currentIndex="+linearPicker.itemList.currentIndex)
        }
    }
    Button {
        text: "append"
        onClicked: {
            pickerModel.append({"label": "Item #"+pickerModel.count});
        }
    }
}
