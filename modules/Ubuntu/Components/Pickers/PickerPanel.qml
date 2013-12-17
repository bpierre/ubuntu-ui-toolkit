/*
 * Copyright 2013 Canonical Ltd.
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
import QtQuick.Window 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

/*!
    \qmltype DateTimePanel
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief Provides a panel for opening a DatePicker in place of the input panel or
    as Popover, depending on the form factor.

    PickerPanel is a singleton component designed to open a DatePicker in input panel
    area or in a Popover, depending on the form factor, following the design guides
    on date pickers.
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    MainWindow {
        width: units.gu(40)
        height: units.gu(71)

        Page {
            title: "PickerPanel"
            Button {
                id: dateButton
                property date date: new Date()
                text: Qt.formatDateTime(date, "yyyy/MMMM"
                onClicked: PickerPanel.openDatePicker(dateButton, "date", "Years|Months")
            }
        }
    }
    \endqml

    The opened panel is closed automatically when the user taps/presses outside
    of the panel or Popover area.
  */

Object {

    /*!
      The function opens a date picker panel. The date picker is opened in input
      panel area if there is on-screen input panel available and the form factor
      describes a phone. The picked date will be read from and reported into the
      \a property of the \a caller as date type. This implies that the caller
      must have defined a property with date type.

      On failure the function returns null. On success the returned object has the
      following properties:
      \code
      {
          property DatePicker picker
          property string pickerMode
          property date date
          property Item caller
          property string callerProperty
      }
      \endcode

      \table
        \header
            \li Property
            \li Description
        \row
            \li \b picker
            \li instance of the DatePicker component shown in the panel/popup
        \row
            \li \b pickerMode
            \li represents the DatePicker \l mode to be used. This is an optional
                parameter and if not defined, the default mode will be used.
        \row
            \li \b date
            \li represents the date selected
        \row
            \li \b caller
            \li the instance of the component opening the panel
        \row
            \li \b callerProperty
            \li the property of the caller holding the date value which will be
                updated by the picker.
      \endtable
      */
    function openDatePicker(caller, property, mode) {
        if (mode === undefined) {
            mode = "Years|Months|Days";
        }
        var params = {
            "date": caller[property],
            "pickerMode": mode,
            "callerProperty": property
        }

        if (!internal.isPhone()) {
            // we have no input panel defined, or the therefore we show the picker in a Popover
            return internal.openPopover(caller, params);
        }
        // OSK panel
        return internal.openPanel(caller, params);
    }

    QtObject {
        id: internal

        property bool formFactorPhone: Screen.width <= units.gu(40) && Screen.height <= units.gu(71)

        function isPhone() {
            return (formFactorPhone && QuickUtils.inputMethodProvider !== "");
//            return formFactorPhone;
        }

        function openPopover(caller, params) {
            var panel = PopupUtils.open(datePickerPopover, caller, params);
            panel.parent = QuickUtils.rootItem(null);
            return panel;
        }
        function openPanel(caller, params) {
            params["caller"] = caller;
            var panel = datePickerPanel.createObject(QuickUtils.rootItem(null), params);
            return panel;
        }
    }

    // popover
    Component {
        id: datePickerPopover
        Popover {
            property alias picker: picker
            property alias date: picker.date
            property alias pickerMode: picker.mode
            property string callerProperty

            contentWidth: frame.width
            contentHeight: frame.height
            __closeOnDismissAreaPress: true
            Rectangle {
                id: frame
                width: picker.width + units.gu(4)
                height: picker.height + units.gu(4)
                color: Qt.rgba(0, 0, 0, 0.02)
                DatePicker {
                    id: picker
                    anchors.centerIn: parent
                }
            }

            Binding {
                target: caller
                property: callerProperty
                when: callerProperty != undefined
                value: picker.date
            }
        }
    }

    // OSK panel
    Component {
        id: datePickerPanel
        Rectangle {
            property alias picker: picker
            property alias date: picker.date
            property alias pickerMode: picker.mode
            property string callerProperty
            property Item caller

            id: panel
            width: Qt.inputMethod.keyboardRectangle.width > 0 ? Qt.inputMethod.keyboardRectangle.width : units.gu(40)
            height: Qt.inputMethod.keyboardRectangle.height > 0 ? Qt.inputMethod.keyboardRectangle.height : units.gu(30)
            y: parent.height
            DatePicker {
                id: picker
                anchors {
                    fill: panel
                    margins: units.gu(2)
                }

                Binding {
                    target: caller
                    property: callerProperty
                    when: callerProperty != undefined
                    value: picker.date
                }
            }

            InverseMouseArea {
                anchors.fill: parent
                onPressed: {
                    panel.state = '';
                }
            }

            Component.onCompleted: state = 'opened'

            states: [
                State {
                    name: 'opened'
                    PropertyChanges {
                        target: panel
                        y: parent.height - height
                    }
                }
            ]
            transitions: [
                Transition {
                    from: ''
                    to: 'opened'
                    UbuntuNumberAnimation {
                        target: panel
                        property: 'y'
                        duration: UbuntuAnimation.BriskDuration
                    }
                },
                Transition {
                    from: 'opened'
                    to: ''
                    SequentialAnimation {
                        UbuntuNumberAnimation {
                            target: panel
                            property: 'y'
                            duration: UbuntuAnimation.BriskDuration
                        }
                        ScriptAction {
                            script: {
                                panel.destroy();
                            }
                        }
                    }
                }

            ]
        }
    }
}
