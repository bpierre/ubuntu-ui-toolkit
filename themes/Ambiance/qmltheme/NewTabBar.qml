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

Item {
    id: tabBar
    height: itemStyle.tabBarHeight //units.gu(6.5)

    /*!
      The set of tabs this tab bar belongs to
     */
    property Tabs tabs

    Connections {
        target: tabs
        onSelectedTabIndexChanged: {
            tabBar.active = false;
            buttonView.position();
        }
    }

    /*!
      Use the tab bar only to display the currently selected tab,
      or can the user interact with it and select a tab?
     */
    property bool active: false

    /*!
      \internal
      Avoid interpreting a click to activate the tab bar as a button click.
     */
    property bool __buttonsClickable: true
    Timer {
        id: buttonsClickableTimer
        interval: 800 // same as the default timing for MouseArea.pressAndHold events
        running: false
        onTriggered: __buttonsClickable = true
    }

    onActiveChanged: {
        if (active) {
            __buttonsClickable = false;
            buttonsClickableTimer.restart();
            deactivateTimer.restart();
        } else {
//            buttonView.position();
        }
    }

    Timer {
        id: deactivateTimer
        interval: (itemStyle && itemStyle.deactivateTime) ?  itemStyle.deactivateTime : 5000
        running: false
        onTriggered: active = false
    }

    Component.onCompleted: buttonView.position();

    // used to position buttons and indicator image
    property real totalButtonWidth: 0
    property var relativeButtonPositions: []
    property var buttonOffsets: []

    Component {
        id: tabButtonRow
        Row {
            id: theRow
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: childrenRect.width

            Component.onCompleted: {
                tabBar.totalButtonWidth = theRow.width;
                tabBar.relativeButtonPositions = [];
                tabBar.buttonOffsets = [];
                for (var i=0; i < children.length-1; i++) { // children[length-2] is the repeater
                    tabBar.relativeButtonPositions.push(children[i].x / tabBar.totalButtonWidth);
                    print("relative position of child "+i+" = "+tabBar.relativeButtonPositions[i])
                    tabBar.buttonOffsets.push(1 - children[i].x / totalButtonWidth);
                }
                tabBar.buttonOffsets.push(0.0);
            }

            Repeater {
                id: repeater
                model: tabs.__tabsModel.children

                AbstractButton {
                    id: button
                    width: text.width + text.anchors.leftMargin + text.anchors.rightMargin
                    property bool selected: (index === tabs.selectedTabIndex)

                    anchors {
                        top: parent.top
                        //                        bottom: parent.bottom
                    }
                    height: parent.height - itemStyle.headerTextBottomMargin

                    Label {
                        id: text
                        color: selected ? itemStyle.headerTextColorSelected : itemStyle.headerTextColor
                        opacity: (tabBar.active || selected) ? 1.0 : 0

                        Behavior on opacity {
                            NumberAnimation {
                                duration: 500
                            }
                        }

                        anchors {
                            left: parent.left
                            leftMargin: units.gu(2)
                            rightMargin: units.gu(0)
                            baseline: parent.bottom
                        }
                        text: title
                        fontSize: itemStyle.headerFontSize
                        font.weight: itemStyle.headerFontWeight
                        verticalAlignment: Text.AlignBottom
                    }
                    onClicked: {
                        if (tabBar.__buttonsClickable) {
                            tabs.selectedTabIndex = index;
                            tabBar.active = false;
                        }
                    }
                }
            }
        }
    }

    PathView {
        id: buttonView
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        delegate: tabButtonRow
        model: 2 // The second buttonRow shows the buttons that disappear on the left
        property bool needsScrolling: tabBar.totalButtonWidth > tabBar.width
        interactive: needsScrolling
        width: needsScrolling ? tabBar.width : tabBar.totalButtonWidth
        clip: !needsScrolling // avoid showing the same button twice

        highlightRangeMode: PathView.NoHighlightRange
        offset: 0
        path: Path {
            startX: -tabBar.totalButtonWidth/2
            PathLine {
                x: tabBar.totalButtonWidth*1.5
            }
        }
        function position() {
            if (!tabBar.relativeButtonPositions) return;
            var newOffset = Math.ceil(offset) - tabBar.relativeButtonPositions[tabBar.tabs.selectedTabIndex];

//                print(newOffset + " too large");
//                newOffset = newOffset + 1;
            print("old offset = "+offset + ", new offset = " + newOffset);
            offset = newOffset; //1 - tabBar.relativeButtonPositions[tabBar.tabs.selectedTabIndex] //+ Math.floor(offset);
        }
        onOffsetChanged: {
//            print(offset)
//            if (offset >= 0 && offset < 1-tabBar.relativeButtonPositions[1]) print("yay");
//            else print("naj");
//            print(offset - Math.floor(offset) > buttonOffsets[1]);
            print(offset - Math.floor(offset));
            print(buttonOffsets);
            for (var i=0; i < buttonOffsets.length-1; i++)
                if (buttonInRange(i)) {

                    print("BUTTON "+i);
//                    tabBar.tabs.selectedTabIndex = i;
                }
        }

        function buttonInRange(i) {
            var relativePosition = offset - Math.floor(offset);
//            print(relativePosition);
            return (buttonOffsets[i] > relativePosition && relativePosition > buttonOffsets[i+1]);
        }

        Behavior on offset {
            SmoothedAnimation {
//                duration: 200
                velocity: 1
            }
        }


        onMovementStarted: deactivateTimer.stop();
        onMovementEnded: deactivateTimer.restart();
    }

    Image {
        id: indicatorImage
        source: itemStyle.indicatorImageSource
        anchors {
            bottom: parent.bottom
            bottomMargin: itemStyle.headerTextBottomMargin
        }

        visible: false //!tabBar.active

        x: getXPosition()

        function getXPosition() {
            var buttons = buttonView.children[1].children; // the first buttonRow
            var selectedButton = buttons[tabs.selectedTabIndex];
            return selectedButton.width + units.gu(2);
        }
    }

    MouseArea {
        // an inactive tabBar can be clicked to make it active
        id: mouseArea
        anchors.fill: parent
        enabled: !tabBar.active
        onPressed: {
            tabBar.active = true;
            mouse.accepted = false;
        }
    }
}
