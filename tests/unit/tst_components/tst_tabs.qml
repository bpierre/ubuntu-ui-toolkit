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
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "TabsAPI"

    function test_selectedTabIndex() {
        compare(emptyTabs.selectedTabIndex,-1,"The default value for selectedTabIndex is -1 when there are no tabs");
        compare(tabs.selectedTabIndex,0,"The default selectedTabIndex is 0 when Tabs has contents");
        var newSelectedTabIndex = 1;
        tabs.selectedTabIndex = newSelectedTabIndex;
        compare(tabs.selectedTabIndex, newSelectedTabIndex,"can set selectedTabIndex");
    }

    Tabs {
        id: emptyTabs
    }

    Tabs {
        id: tabs
        Tab {
            id: tab1
        }
        Tab {
            id: tab2
        }
        Tab {
            id: tab3
        }
    }
}
