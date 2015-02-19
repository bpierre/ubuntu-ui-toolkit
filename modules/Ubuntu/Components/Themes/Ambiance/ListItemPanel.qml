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

/*
  This component is the holder of the ListItem actions.
  */
Item {

    // styling properties
    /*
      Color of the background.
      */
    // FIXME: use Palette colors instead when available
    property color backgroundColor: (leading ? UbuntuColors.red : "white")

    /*
      Color used in coloring the icons.
      */
    // FIXME: use Palette colors instead when available
    property color foregroundColor: leading ? "white" : UbuntuColors.darkGrey

    /*
      Specifies the width of the component visualizing the action.
      */
    property real paintedActionWidth: units.gu(2.5)

    // panel implementation
    id: panel
    width: Math.max(
               actionsRow.childrenRect.width,
               ListItem.visibleActions.length * MathUtils.clamp(paintedActionWidth, height, actionsRow.maxItemWidth))

    // used for module/autopilot testing
    objectName: "ListItemPanel" + (leading ? "Leading" : "Trailing")

    /*
      Specifies whether the panel is used to visualize leading or trailing actions.
      */
    readonly property bool leading: ListItem.panelStatus == ListItem.Leading
    readonly property real swipedOffset: leading ? width + x : styledItem.width - x;
    readonly property bool swiping: styledItem.highlighted && styledItem.contentMoving

    anchors {
        left: leading ? undefined : styledItem.contentItem.right
        right: leading ? styledItem.contentItem.left : undefined
        // anchor to the top of the item but to the bottom of the contentItem, so we do not draw over the divider
        top: styledItem.top
        bottom: styledItem.contentItem.bottom
        bottomMargin: -styledItem.contentItem.anchors.bottomMargin
        // adjust margins
        leftMargin: leading ? 0 : styledItem.contentItem.anchors.rightMargin
        rightMargin: leading ? styledItem.contentItem.anchors.leftMargin : 0
    }

    Rectangle {
        objectName: "panel_background"
        anchors {
            fill: parent
            // add 4 times the overshoot margins to cover the background when tugged
            leftMargin: (leading && styledItem) ? -units.gu(4 * styledItem.swipeOvershoot) : 0
            rightMargin: (!leading && styledItem) ? -units.gu(4 * styledItem.swipeOvershoot) : 0
        }
        color: panel.backgroundColor
    }

    // handle action triggering
    Connections {
        target: styledItem
        onContentMovementEnded: {
            if (actionsRow.selectedAction) {
                actionsRow.selectedAction.trigger(actionsRow.listItemIndex);
                actionsRow.listItemIndex = -1;
                actionsRow.selectedAction = null;
            }
        }
    }

    // track drag direction, so we know in which direction we should snap
    property real prevX: 0.0
    property real snapChangerLimit: 0.0
    property real threshold: units.gu(1)
    property bool snapIn: false
    onXChanged: {
        if (prevX < x && (snapChangerLimit <= x)) {
            snapIn = leading;
            snapChangerLimit = x - threshold;
        } else if (prevX > x && (x < snapChangerLimit)) {
            snapIn = !leading;
            snapChangerLimit = x + threshold;
        }
        prevX = x;
    }
    // default snapping!
    onSwipingChanged: {
        if (swiping) {
            // the dragging got started, set prevX
            prevX = panel.x;
            return;
        }
        if (!visible) {
            return;
        }
        // snap in if the offset is bigger than the overshoot and the direction of the drag is to reveal the panel
        var snapPos = (swipedOffset > units.gu(2) && snapIn) ? panel.width : 0.0;
        ListItem.snapToPosition(snapPos);
    }

    Row {
        id: actionsRow
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: spacing
        }

        property real maxItemWidth: styledItem.width / panel.ListItem.visibleActions.length

        property Action selectedAction
        property int listItemIndex: -1

        Repeater {
            model: panel.ListItem.visibleActions
            AbstractButton {
                id: actionButton
                action: modelData
                enabled: action.enabled
                opacity: action.enabled ? 1.0 : 0.5
                width: MathUtils.clamp(delegateLoader.item ? delegateLoader.item.width : 0, height, actionsRow.maxItemWidth)
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
                function trigger() {
                    actionsRow.selectedAction = modelData;
                    actionsRow.listItemIndex = panel.ListItem.index;
                    panel.ListItem.snapToPosition(0.0);
                }

                Rectangle {
                    anchors.fill: parent
                    color: Theme.palette.selected.background
                    visible: pressed
                }

                Loader {
                    id: delegateLoader
                    height: parent.height
                    sourceComponent: panel.ListItem.actions.delegate ? panel.ListItem.actions.delegate : defaultDelegate
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
                width: panel.paintedActionWidth
                height: width
                name: action.iconName
                color: panel.foregroundColor
                anchors.centerIn: parent
            }
        }
    }
}