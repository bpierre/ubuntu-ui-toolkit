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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.2

Item {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Action {
        id: stockAction
        iconName: "starred"
    }
    ListItemActions {
        id: leading
        actions: [
            Action {
                iconName: "starred"
            },
            Action {
                iconName: "starred"
            },
            Action {
                iconName: "starred"
            }
        ]
    }
    ListItemActions {
        id: trailing
        actions: [
            stockAction,
        ]
        delegate: Item {
            objectName: "custom_delegate"
        }
    }
    ListItemActions {
        id: actionsDefault
    }

    Column {
        width: parent.width
        ListItem {
            id: defaults
            width: parent.width
        }
        ListItem {
            id: testItem
            width: parent.width
            color: "blue"
            leadingActions: leading
            trailingActions: ListItemActions {
                actions: leading.actions
            }
            Item {
                id: bodyItem
                anchors.fill: parent
            }
        }
        ListView {
            id: listView
            width: parent.width
            height: units.gu(24)
            clip: true
            model: 10
            delegate: ListItem {
                objectName: "listItem" + index
                width: parent.width
                leadingActions: leading
                trailingActions: trailing
            }
        }
    }

    UbuntuTestCase {
        name: "ListItemAPI"
        when: windowShown

        SignalSpy {
            id: pressedSpy
            signalName: "pressedChanged"
            target: testItem
        }

        SignalSpy {
            id: clickSpy
            signalName: "clicked"
            target: testItem;
        }

        function panelItem(actionList) {
            return findInvisibleChild(actionList, "ListItemPanel");
        }

        function initTestCase() {
            TestExtras.registerTouchDevice();
            waitForRendering(main);
        }

        function cleanup() {
            pressedSpy.clear();
            clickSpy.clear();
            listView.interactive = true;
            // tap on the first item to make sure we are rebounding all
            mouseClick(defaults, 0, 0);
            // make sure all events are processed
            wait(200);
        }

        function test_0_defaults() {
            verify(defaults.contentItem !== null, "Defaults is null");
            compare(defaults.color, "#000000", "Transparent by default");
            compare(defaults.highlightColor, Theme.palette.selected.background, "Theme.palette.selected.background color by default")
            compare(defaults.pressed, false, "Not pressed buy default");
            compare(defaults.divider.visible, true, "divider is visible by default");
            compare(defaults.divider.leftMargin, units.dp(2), "divider's left margin is 2GU");
            compare(defaults.divider.rightMargin, units.dp(2), "divider's right margin is 2GU");
            compare(defaults.divider.colorFrom, "#000000", "colorFrom differs.");
            fuzzyCompare(defaults.divider.colorFrom.a, 0.14, 0.01, "colorFrom alpha differs");
            compare(defaults.divider.colorTo, "#ffffff", "colorTo differs.");
            fuzzyCompare(defaults.divider.colorTo.a, 0.07, 0.01, "colorTo alpha differs");

            compare(actionsDefault.delegate, null, "ListItemActions has no delegate set by default.");
            compare(actionsDefault.actions.length, 0, "ListItemActions has no options set.");
        }

        function test_children_in_content_item() {
            compare(bodyItem.parent, testItem.contentItem, "Content is not in the right holder!");
        }

        function test_pressedChanged_on_click() {
            mousePress(testItem, testItem.width / 2, testItem.height / 2);
            pressedSpy.wait();
            mouseRelease(testItem, testItem.width / 2, testItem.height / 2);
        }
        function test_pressedChanged_on_tap() {
            TestExtras.touchPress(0, testItem, centerOf(testItem));
            pressedSpy.wait();
            TestExtras.touchRelease(0, testItem, centerOf(testItem));
            // local cleanup, wait few msecs to suppress double tap
            wait(400);
        }

        function test_clicked_on_mouse() {
            mouseClick(testItem, testItem.width / 2, testItem.height / 2);
            clickSpy.wait();
        }
        function test_clicked_on_tap() {
            TestExtras.touchClick(0, testItem, centerOf(testItem));
            clickSpy.wait();
        }

        function test_mouse_click_on_listitem() {
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "Cannot find listItem0");

            mousePress(listItem, listItem.width / 2, 0);
            compare(listItem.pressed, true, "Item is not pressed?");
            // do 5 moves to be able to sense it
            var dy = 0;
            for (var i = 1; i <= 5; i++) {
                dy += i * 10;
                mouseMove(listItem, listItem.width / 2, dy);
            }
            compare(listItem.pressed, false, "Item is pressed still!");
            mouseRelease(listItem, listItem.width / 2, dy);
        }
        function test_touch_click_on_listitem() {
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "Cannot find listItem0");

            TestExtras.touchPress(0, listItem, Qt.point(listItem.width / 2, 5));
            compare(listItem.pressed, true, "Item is not pressed?");
            // do 5 moves to be able to sense it
            var dy = 0;
            for (var i = 1; i <= 5; i++) {
                dy += i * 10;
                TestExtras.touchMove(0, listItem, Qt.point(listItem.width / 2, dy));
            }
            compare(listItem.pressed, false, "Item is pressed still!");
            // cleanup, wait few milliseconds to avoid dbl-click collision
            TestExtras.touchRelease(0, listItem, Qt.point(listItem.width / 2, dy));
        }

        function test_background_height_change_on_divider_visible() {
            // make sure the testItem's divider is shown
            testItem.divider.visible = true;
            verify(testItem.contentItem.height < testItem.height, "ListItem's background height must be less than the item itself.");
            testItem.divider.visible = false;
            compare(testItem.contentItem.height, testItem.height, "ListItem's background height must be the same as the item itself.");
            testItem.divider.visible = true;
        }

        function test_touch_tug_options_data() {
            var item = findChild(listView, "listItem0");
            return [
                {tag: "Trailing, mouse", item: item, pos: centerOf(item), dx: -units.gu(20), positiveDirection: false, mouse: true},
                {tag: "Leading, mouse", item: item, pos: centerOf(item), dx: units.gu(20), positiveDirection: true, mouse: true},
                {tag: "Trailing, touch", item: item, pos: centerOf(item), dx: -units.gu(20), positiveDirection: false, mouse: false},
                {tag: "Leading, touch", item: item, pos: centerOf(item), dx: units.gu(20), positiveDirection: true, mouse: false},
            ];
        }
        function test_touch_tug_options(data) {
            listView.positionViewAtBeginning();
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            waitForRendering(data.item, 400);
            if (data.positiveDirection) {
                verify(data.item.contentItem.x > 0, data.tag + " options did not show up");
            } else {
                verify(data.item.contentItem.x < 0, data.tag + " options did not show up");
            }

            // dismiss
            if (data.mouse) {
                mouseClick(main, 1, 1);
            } else {
                TestExtras.touchClick(0, main, Qt.point(1, 1));
            }
            waitForRendering(data.item, 400);
        }

        function test_rebound_when_pressed_outside_or_clicked_data() {
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            return [
                {tag: "Click on an other Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: true},
                {tag: "Click on the same Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item0.contentItem, mouse: true},
                {tag: "Tap on an other Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: false},
                {tag: "Tap on the same Item", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item0.contentItem, mouse: false},
            ];
        }
        function test_rebound_when_pressed_outside_or_clicked(data) {
            listView.positionViewAtBeginning();
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            waitForRendering(data.item, 400);
            verify(data.item.contentItem.x != 0, "The component wasn't tugged!");
            // dismiss
            if (data.mouse) {
                mouseClick(data.clickOn, centerOf(data.clickOn).x, centerOf(data.clickOn).y);
            } else {
                TestExtras.touchClick(0, data.clickOn, centerOf(data.clickOn));
            }
            waitForRendering(data.item, 400);
            tryCompareFunction(function(){ return data.item.contentItem.x; }, 0, 1000);
        }

        function test_listview_not_interactive_while_tugged_data() {
            var item0 = findChild(listView, "listItem0");
            var item1 = findChild(listView, "listItem1");
            return [
                {tag: "Trailing", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: true},
                {tag: "Leading", item: item0, pos: centerOf(item0), dx: units.gu(20), clickOn: item0.contentItem, mouse: true},
                {tag: "Trailing", item: item0, pos: centerOf(item0), dx: -units.gu(20), clickOn: item1, mouse: false},
                {tag: "Leading", item: item0, pos: centerOf(item0), dx: units.gu(20), clickOn: item0.contentItem, mouse: false},
            ];
        }
        function test_listview_not_interactive_while_tugged(data) {
            listView.positionViewAtBeginning();
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            waitForRendering(data.item, 800);
            compare(listView.interactive, false, "The ListView is still interactive!");
            // dismiss
            if (data.mouse) {
                mouseClick(data.clickOn, centerOf(data.clickOn).x, centerOf(data.clickOn).y);
            } else {
                TestExtras.touchClick(0, data.clickOn, centerOf(data.clickOn));
            }
            waitForRendering(data.item, 400);
            tryCompareFunction(function(){ return listView.interactive; }, true, 1000);
        }

        function test_selecting_action_rebounds_data() {
            var item0 = findChild(listView, "listItem0");
            return [
                {tag: "With mouse", item: item0, pos: centerOf(item0), dx: units.gu(20), actions: item0.leadingActions, select: "list_option_0", mouse: true},
                {tag: "With touch", item: item0, pos: centerOf(item0), dx: units.gu(20), actions: item0.leadingActions, select: "list_option_0", mouse: false},
            ]
        }
        function test_selecting_action_rebounds(data) {
            listView.positionViewAtBeginning();
            if (data.mouse) {
                flick(data.item, data.pos.x, data.pos.y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, data.pos, Qt.point(data.dx, 0));
            }
            waitForRendering(data.item, 800);
            var selectedOption = findChild(panelItem(data.actions), data.select);
            verify(selectedOption, "Cannot select option " + data.select);
            // dismiss
            if (data.mouse) {
                mouseClick(selectedOption, centerOf(selectedOption).x, centerOf(selectedOption).y);
            } else {
                TestExtras.touchClick(0, selectedOption, centerOf(selectedOption));
            }
            waitForRendering(data.item, 400);
            tryCompareFunction(function(){ return data.item.contentItem.x; }, 0, 1000);
        }

        function test_custom_trailing_delegate() {
            listView.positionViewAtBeginning();
            var item = findChild(listView, "listItem0");
            flick(item, centerOf(item).x, centerOf(item).y, -units.gu(20), 0);
            verify(panelItem(trailing), "Panel is not visible");
            var custom = findChild(panelItem(trailing), "custom_delegate");
            verify(custom, "Custom delegate not in use");
            // cleanup
            mouseClick(main, 0, 0);
        }

        // execute as last so we make sure we have the panel created
        function test_snap_data() {
            verify(panelItem(testItem.leadingActions), "Panel had not been created!");
            var action = findChild(panelItem(testItem.leadingActions), "list_option_0");
            verify(action, "Options not accessible");
            var actionSize = action.width;
            return [
                {tag: "Snap back leading, mouse", item: testItem.contentItem, dx: actionSize / 2 - 10, list: testItem.leadingActions, snap: false, mouse: true},
                {tag: "Snap back leading, touch", item: testItem.contentItem, dx: actionSize / 2 - 10, list: testItem.leadingActions, snap: false, mouse: false},
                {tag: "Snap in leading, mouse", item: testItem.contentItem, dx: actionSize / 2 + 10, list: testItem.leadingActions, snap: true, mouse: true},
                {tag: "Snap in leading, touch", item: testItem.contentItem, dx: actionSize / 2 + 10, list: testItem.leadingActions, snap: true, mouse: false},

                {tag: "Snap back trailing, mouse", item: testItem.contentItem, dx: -(actionSize / 2 - 10), list: testItem.trailingActions, snap: false, mouse: true},
                {tag: "Snap back trailing, touch", item: testItem.contentItem, dx: -(actionSize / 2 - 10), list: testItem.trailingActions, snap: false, mouse: false},
                {tag: "Snap in trailing, mouse", item: testItem.contentItem, dx: -(actionSize / 2 + 10), list: testItem.trailingActions, snap: true, mouse: true},
                {tag: "Snap in trailing, touch", item: testItem.contentItem, dx: -(actionSize / 2 + 10), list: testItem.trailingActions, snap: true, mouse: false},
            ];
        }
        function test_snap(data) {
            if (data.mouse) {
                flick(data.item, centerOf(data.item).x, centerOf(data.item).y, data.dx, 0);
            } else {
                TestExtras.touchDrag(0, data.item, centerOf(data.item), Qt.point(data.dx, 0));
            }

            waitForRendering(data.item, 800);
            if (data.snap) {
                verify(data.item.x != 0, "Not snapped to be visible");
            } else {
                verify(data.item.x == 0, "Not snapped back");
            }

            // cleanup
            if (data.mouse) {
                mouseClick(data.item, centerOf(data.item).x, centerOf(data.item).y);
            } else {
                TestExtras.touchClick(0, data.item, centerOf(data.item));
            }
            waitForRendering(data.item, 800);
        }
    }
}
