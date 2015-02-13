/*
 * Copyright 2014-2015 Canonical Ltd.
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

import QtQuick 2.3
import Ubuntu.Components.Styles 1.2 as Styles
import Ubuntu.Components 1.2

Styles.ListItemStyle {

    id: listItemStyle
    /*
     * Take over the ListItem's index context property as repeater used in panel
     * overrides the property.
     */
    readonly property int listItemIndex: index

    // anchoring
    anchors {
        top: parent ? parent.top : undefined
        bottom: parent ? parent.bottom : undefined
        bottomMargin: styledItem.divider.visible ? styledItem.divider.height : 0
        left: styledItem.contentItem.left
        leftMargin: -styledItem.contentItem.anchors.leftMargin
        right: styledItem.contentItem.right
        rightMargin: -styledItem.contentItem.anchors.rightMargin
    }

    // leading/trailing panels
    Component {
        id: panelComponent
        Rectangle {
            id: panel
            objectName: "ListItemPanel" + (leading ? "Leading" : "Trailing")
            property bool leading: false
            readonly property real panelWidth: actionsRow.width

            // FIXME use theme palette colors once stabilized
            color: leading ? UbuntuColors.red : "white"
            anchors.fill: parent
            width: parent ? parent.width : 0

            readonly property ListItemActions itemActions: leading ? styledItem.leadingActions : styledItem.trailingActions

            Row {
                id: actionsRow
                anchors {
                    left: leading ? undefined : parent.left
                    right: leading ? parent.right : undefined
                    top: parent.top
                    bottom: parent.bottom
                    leftMargin: spacing
                }

                readonly property real maxItemWidth: parent.width / itemActions.actions.length

                Repeater {
                    model: itemActions.actions
                    AbstractButton {
                        id: actionButton
                        action: modelData
                        enabled: action.enabled
                        opacity: action.enabled ? 1.0 : 0.5
                        width: MathUtils.clamp(delegateLoader.item ? delegateLoader.item.width : 0, height, actionsRow.maxItemWidth)
                        anchors {
                            top: parent ? parent.top : undefined
                            bottom: parent ? parent.bottom : undefined
                        }
                        function trigger() {
                            internals.selectedAction = modelData;
                            listItemStyle.rebound();
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: Theme.palette.selected.background
                            visible: pressed
                        }

                        Loader {
                            id: delegateLoader
                            height: parent.height
                            sourceComponent: itemActions.delegate ? itemActions.delegate : defaultDelegate
                            property Action action: modelData
                            property int index: index
                            property bool pressed: actionButton.pressed
                            onItemChanged: {
                                // use action's objectName to identify the visualized action
                                if (item && item.objectName === "") {
                                    item.objectName = modelData.objectName;
                                    actionButton.objectName = "actionbutton_" + modelData.objectName
                                }
                            }
                        }
                    }
                }
            }

            Component {
                id: defaultDelegate
                Item {
                    width: height
                    Icon {
                        width: units.gu(2.5)
                        height: width
                        name: action.iconName
                        // FIXME use theme palette colors once stabilized
                        color: leading ? "white" : UbuntuColors.darkGrey
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }

    // leading panel loader
    Loader {
        id: leadingLoader
        objectName: "leading_loader"
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.left
        }
        width: parent.width
        sourceComponent: internals.swiped && styledItem.leadingActions && styledItem.leadingActions.actions.length > 0 ?
                             panelComponent : null
        onItemChanged: {
            if (item && item.hasOwnProperty("leading")) {
                item.leading = true;
            }
        }
    }
    // trailing panel loader
    Loader {
        id: trailingLoader
        objectName: "trailing_loader"
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.right
        }
        width: parent.width
        sourceComponent: internals.swiped && styledItem.trailingActions && styledItem.trailingActions.actions.length > 0 ?
                             panelComponent : null
        onItemChanged: {
            if (item && item.hasOwnProperty("leading")) {
                item.leading = false;
            }
        }
    }

    // internals
    QtObject {
        id: internals
        // action triggered
        property Action selectedAction
        // swipe handling
        readonly property bool swiped: listItemStyle.x != styledItem.x && !styledItem.selectable
        readonly property Item swipedPanel: listItemStyle.x > 0 ? leadingLoader.item : trailingLoader.item
        readonly property bool leadingPanel: listItemStyle.x > 0
        readonly property real swipedOffset: leadingPanel ? listItemStyle.x : -listItemStyle.x
        readonly property real panelWidth: swipedPanel && swipedPanel.hasOwnProperty("panelWidth") ? swipedPanel.panelWidth : 0
        property real prevX: 0.0
        property real snapChangerLimit: 0.0
        readonly property real threshold: units.gu(1.5)
        property bool snapIn: false

        // update snap direction
        function updateSnapDirection() {
            if (prevX < listItemStyle.x && (snapChangerLimit <= listItemStyle.x)) {
                snapIn = leadingPanel;
                snapChangerLimit = listItemStyle.x - threshold;
            } else if (prevX > listItemStyle.x && (listItemStyle.x < snapChangerLimit)) {
                snapIn = !leadingPanel;
                snapChangerLimit = listItemStyle.x + threshold;
            }
            prevX = listItemStyle.x;
        }
        // perform snapIn/Out
        function snap() {
            var snapPos = (swipedOffset > units.gu(2) && snapIn) ? panelWidth : 0.0;
            snapPos *= leadingPanel ? 1 : -1;
            snapAnimation.snapTo(snapPos);
        }
        // handle elasticity on overshoot
        function overshoot(event) {
            var offset = event.content.x - styledItem.contentItem.anchors.leftMargin;
            offset *= leadingPanel ? 1 : -1;
            if (offset > panelWidth) {
                // do elastic move
                event.content.x = styledItem.contentItem.x + (event.to.x - event.from.x) / 2;
            }
        }
    }
    snapAnimation: SmoothedAnimation {
        objectName: "snap_animation"
        target: styledItem.contentItem
        property: "x"
        // use 50GU/second velocity
        velocity: units.gu(60)
        onStopped: {
            // trigger action
            if (to == styledItem.contentItem.anchors.leftMargin && internals.selectedAction) {
                internals.selectedAction.trigger(listItemIndex);
                internals.selectedAction = null;
            }
        }
        // animated snapping
        function snapTo(pos) {
            if (pos == to && styledItem.contentItem.x == to) {
                return;
            }

            stop();
            from = styledItem.contentItem.x;
            if (!pos) {
                pos = styledItem.contentItem.anchors.leftMargin;
            }
            to = pos;
            start();
        }
    }

    onXChanged: internals.updateSnapDirection()
    // overriding default functions
    function swipeEvent(event) {
        if (event.status == SwipeEvent.Started) {
            internals.prevX = x;
            snapAnimation.stop();
        } else if (event.status == SwipeEvent.Finished) {
            internals.snap();
        } else if (event.status == SwipeEvent.Updated) {
            // handle elasticity when overshooting
            internals.overshoot(event)
        }
    }
    function rebound() {
        snapAnimation.snapTo(0);
    }

    // the selection/multiselection panel
    Component {
        id: selectionDelegate
        Item {
            objectName: "selection_panel" + listItemIndex
            anchors.fill: parent ? parent : undefined

            CheckBox {
                id: checkbox
                // for unit and autopilot tests
                objectName: "listitem_select"
                anchors.centerIn: parent
                // for the initial value
                checked: styledItem.selected
                onCheckedChanged: styledItem.selected = checked;
            }
            // update checkbox when ViewItems.selectionIndices changes
            Connections {
                target: styledItem
                onSelectedChanged: checkbox.checked = styledItem.selected
            }
        }
    }

    // make sure the state is changed only after component completion
    Component.onCompleted: {
        state = Qt.binding(function () {
            return styledItem.selectable ? "selected" : "";
        });
    }
    states: [
        State {
            name: "selected"
            PropertyChanges {
                target: leadingLoader
                sourceComponent: selectionDelegate
                width: units.gu(5)
            }
            PropertyChanges {
                target: listItemStyle
                anchors.leftMargin: 0
            }
            PropertyChanges {
                target: styledItem.contentItem
                anchors.leftMargin: units.gu(5)
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "selected"
            reversible: true
            enabled: animatePanels
            PropertyAnimation {
                target: styledItem.contentItem
                properties: "anchors.leftMargin"
                easing: UbuntuAnimation.StandardEasing
                duration: UbuntuAnimation.FastDuration
            }
        }
    ]
}
