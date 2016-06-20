/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components 1.3

TestCase {
    name: "MenuAPI"

    TestUtil {
        id: util
    }

    function test_menubar() {
        compare(menuBar.menus.length, 2, "Incorrect number of menus in menubar")
        compare(menuBar.menus[0], submenu1, "Incorrect element found at menu bar index")
        compare(menuBar.menus[1], submenu2, "Incorrect element found at menu bar index")
    }

    function test_menu() {
        compare(submenu1.data.length, 4, "Incorrect number of menu items in menu");
        compare(submenu1.data[0], actionList, "Incorrect element found at menu index");
        compare(submenu1.data[1], menuGroup, "Incorrect element found at menu index");
        compare(submenu1.data[2], action1, "Incorrect element found at menu index");
        compare(submenu1.data[3], action2, "Incorrect element found at menu index");
    }

    function test_menugroup() {
        compare(group.data.length, 4, "Incorrect number of menu items in MenuGroup");
        compare(group.data[0], groupAction, "Action not found at correct index in MenuGroup");
        compare(group.data[1], groupList, "ActionList not found at correct index in MenuGroup");
        compare(group.data[2], groupMenu, "Menu not found at correct index in MenuGroup");
        compare(group.data[3], groupGroup, "MenuGroup not found at correct index in MenuGroup");
    }

    function test_detached_menu() {
        compare(detachedMenu.data.length, 0, "Incorrect number of menu items in menu");
    }

    function test_dynamic_append() {
        detachedMenu.appendObject(floatingAction);
        compare(detachedMenu.data.length, 1, "Action not added to menu");

        detachedMenu.removeObject(floatingAction);
        compare(detachedMenu.data.length, 0, "Action not removed from menu");
    }

    function test_dynamic_action_list_append() {
        detachedMenu.appendObject(floatingList);
        compare(detachedMenu.data.length, 1, "ActionList not added to menu");

        detachedMenu.removeObject(floatingList);
        compare(detachedMenu.data.length, 0, "ActionList not removed from menu");
    }

    function test_dynamic_insert() {
        detachedMenu.insertObject(0, floatingAction);
        detachedMenu.insertObject(0, floatingList);
        compare(detachedMenu.data.length, 2, "ActionList not removed from menu");

        compare(detachedMenu.data[0], floatingList, "Object was not inserted at correct index");
        compare(detachedMenu.data[1], floatingAction, "Object was not inserted at correct index");

        detachedMenu.removeObject(floatingAction);
        detachedMenu.removeObject(floatingList);
        compare(detachedMenu.data.length, 0, "Menu should be empty");
    }

    MenuBar {
        id: menuBar
        Menu {
            id: submenu1
            ActionList {
                id: actionList
                Action {
                }
                Action {
                }
            }
            MenuGroup {
                id: menuGroup
            }
            Action {
                id: action1
            }
            Action {
                id: action2
            }
        }
        Menu {
            id: submenu2
            Action {
            }
        }
    }

    Menu {
        id: detachedMenu
    }

    MenuGroup {
        id: group
        Action {
            id: groupAction
        }
        ActionList {
            id: groupList
        }
        Menu {
            id: groupMenu
        }
        MenuGroup {
            id: groupGroup
        }
    }

    Menu {
        id: dynamicMenu
    }

    Action {
        id: floatingAction
    }

    ActionList {
        id: floatingList
        Action {
        }
        Action {
        }
    }
}
