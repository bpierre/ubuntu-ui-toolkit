/*
 * Copyright (C) 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Test 1.0

Item {
    width: units.gu(50)
    height: units.gu(70)

    Header {
//        anchors {
//            left: parent.left
//            right: parent.right
//            top: parent.top
//        }
        width: parent.width
        height: units.gu(8)

        flickable: flickable

        onYChanged: print("header.y = "+y)
        Component.onCompleted: print(" init v = "+exposed)
        onExposedChanged: {
            print("header.visible changed to "+exposed)
//            hiddenSwitch.checked = !header.visible;
        }
        id: header
//        title: "Visibility testing"
//        flickable: flickable
//        locked: lockedSwitch.checked
        Rectangle {
            anchors.fill: parent
            color: "red"
            opacity: 0.5
            border {
                color: "black"
                width: 10
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    print("click!")
                    header.anchors.topMargin = -units.gu(6)
                }
            }
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        anchors.topMargin: units.gu(10)
        contentHeight: height * 2

        Grid {
            id: switchGrid
            columns: 2
            spacing: units.gu(1)
            anchors {
                top: parent.top
                left: parent.left
                margins: units.gu(5)
            }
            Switch {
                id: lockedSwitch
                checked: false
            }
            Label {
                text: "header locked"
            }
            Switch {
                id: hiddenSwitch
                checked: !header.exposed
                onCheckedChanged: {
                    print("checked changed. visible = "+header.visible)
                    print("hidden checked changed to "+checked)
                }

                onClicked: {
//                    if (checked) header.hide();
//                    else header.show();
//                    print(" header hidden = "+checked)
                    header.exposed = checked;
//                    if (checked) header.exposed = true;
//                    else header.exposed = false;
                }
            }
            Label {
                text: "header hidden"
            }
        }
        Label {
            anchors {
                top : switchGrid.bottom
                horizontalCenter: parent.horizontalCenter
                topMargin: units.gu(6)
            }
            text: "Flick me"
        }
    }


//    UbuntuTestCase {
//        name: "HeaderLockedVisible"
//        when: windowShown
//        id: testCase

//        property var header
//        function initTestCase() {
//            testCase.header = findChild(mainView, "MainView_Header");
//        }

//        function init() {
//            page.head.visible = true;
//            page.head.locked = false;
//            otherPage.head.visible = true;
//            otherPage.head.locked = false;
//            wait_for_visible(true, "Header is not visible initially.");
//            compare(stack.currentPage, page, "Wrong Page on PageStack initially.");
//            compare(page.head.locked, false, "Header is not locked initially.");
//        }

//        function scroll(dy) {
//            var p = centerOf(mainView);
//            // Use mouseWheel to scroll because mouseDrag is very unreliable
//            // and does not properly handle negative values for dy.
//            mouseWheel(mainView, p.x, p.y, 0, 2*dy);
//        }

//        function scroll_down() {
//            scroll(-header.height);
//        }

//        function scroll_up() {
//            scroll(header.height);
//        }

//        function wait_for_visible(visible, errorMessage) {
//            waitForHeaderAnimation(mainView);
//            compare(stack.currentPage.head.visible, visible, errorMessage);
//            var mismatchMessage = " Page.head.visible does not match header visibility.";
//            if (visible) {
//                compare(header.y, 0, errorMessage + mismatchMessage);
//            } else {
//                compare(header.y, -header.height, errorMessage + mismatchMessage);
//            }
//        }

//        function test_set_visible_to_hide_and_show() {
//            page.head.visible = false;
//            wait_for_visible(false, "Cannot hide unlocked header by setting visible to false.");
//            page.head.visible = true;
//            wait_for_visible(true, "Cannot show unlocked header by setting visible to true.");

//            page.head.locked = true;
//            page.head.visible = false;
//            wait_for_visible(false, "Cannot hide locked header by setting visible to false.");
//            page.head.visible = true;
//            wait_for_visible(true, "Cannot show locked header by setting visible to true.");
//        }

//        function test_scroll_when_unlocked_updates_visible() {
//            scroll_down();
//            wait_for_visible(false, "Scrolling down does not hide header.");
//            scroll_up();
//            wait_for_visible(true, "Scrolling up does not show header.");
//        }

//        function test_scroll_when_locked_does_not_update_visible() {
//            // Note that with a locked header, scrolling up and down does not
//            //  cause the header to move, so the wait_for_visible() calls below
//            //  will return almost instantly.
//            page.head.locked = true;
//            scroll_down();
//            wait_for_visible(true, "Scrolling down hides locked header.");
//            scroll_up();
//            wait_for_visible(true, "Scrolling up hides locked header.");

//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);
//            scroll_down();
//            wait_for_visible(false, "Scrolling down shows locked header.");
//            scroll_up();
//            wait_for_visible(false, "Scrolling up shows locked header.");
//        }

//        function test_locking_updates_visible() {
//            // locked = false, visible = true.
//            page.head.locked = true;
//            wait_for_visible(true, "Locking hides header.");
//            page.head.locked = false;
//            wait_for_visible(true, "Unlocking hides header.");

//            page.head.locked = true;
//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);
//            // When the flickable is scrolled to the top, unlocking the header must show
//            //  the header because you cannot scroll more up to reveal it:
//            page.head.locked = false;
//            wait_for_visible(true, "Unlocking header when flickable is at Y beginning "+
//                             "does not show header.");

//            scroll_down();
//            wait_for_visible(false, "Scrolling down does not hide header.");
//            page.head.locked = true;
//            wait_for_visible(false, "Locking shows header.");
//            // When flickable is scrolled down, unlocking header does not show header
//            //  because the user can scroll up to reveal it:
//            page.head.locked = false;
//            wait_for_visible(false, "Unlocking shows header when flickable is not at " +
//                             "Y beginning.");
//        }

//        function test_activate_page_shows_header() {
//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);

//            // Header becomes visible when new Page becomes active:
//            stack.push(otherPage);
//            wait_for_visible(true, "Pushing page on stack does not show header.");

//            // Header becomes visible when Page with previously hidden header
//            // becomes active:
//            stack.pop();
//            wait_for_visible(true, "Activating unlocked Page does not make header visible.");
//        }

//        function test_activate_hides_locked_hidden_header() {
//            otherPage.head.locked = true;
//            otherPage.head.visible = false;

//            stack.push(otherPage);
//            wait_for_visible(false, "Pushing Page with locked hidden header shows header.");
//            compare(otherPage.head.locked, true, "Pushing Page unlocks header.");
//            compare(page.head.locked, false, "Pushing Page locks previous header.");

//            stack.pop();
//            wait_for_visible(true, "Popping to a Page with unlocked header does not show header.");
//            compare(otherPage.head.locked, true, "Popping Page unlocks previous header.");
//            compare(page.head.locked, false, "Popping Page locks header.");
//        }

//        function test_hidden_locked_header_stays_hidden() {
//            page.head.locked = true;
//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);
//            stack.push(otherPage);
//            waitForHeaderAnimation(mainView);
//            stack.pop();
//            wait_for_visible(false, "Popping to a Page with locked hidden header shows header.");
//        }

//        function test_page_with_no_title_on_pagestack_has_back_button_bug1402054() {
//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);
//            stack.push(titleLessPage);
//            wait_for_visible(true, "Page with no title hides the header.");

//            var backButton = findChild(testCase.header, "backButton");
//            verify(null !== backButton, "Header has no back button.");
//            compare(backButton.visible, true, "Page with no title hides the back button.");

//            var center = centerOf(backButton);
//            mouseClick(backButton, center.x, center.y);
//            waitForHeaderAnimation(mainView);
//            compare(stack.depth, 1, "Clicking back button of page with no title does not "+
//                    "pop the page from the PageStack.");
//        }

//        function test_disable_buttons_when_animating_header_bug1478147() {
//            stack.push(noHeaderPage);
//            stack.push(otherPage);
//            waitForHeaderAnimation(mainView);

//            var backButton = findChild(testCase.header, "backButton");
//            var center = centerOf(backButton);
//            mouseMove(backButton, center.x, center.y);

//            // Click the back button to pop the stack and go back to
//            //  noHeaderPage. This hides the header:
//            mouseClick(backButton, center.x, center.y);
//            // Second click, which does not generate an event because animating
//            //  of the header must disable its buttons:
//            mouseClick(backButton, center.x, center.y);
//            waitForHeaderAnimation(mainView);
//            // Compare the titles instead of the pages directly to avoid getting
//            //  a "Maximum call stack size exceeded." exception.
//            compare(stack.currentPage.title, noHeaderPage.title,
//                    "Back button in animating header was clicked.");

//            stack.pop(); // noHeaderPage
//            waitForHeaderAnimation(mainView);
//        }
//    }
}
