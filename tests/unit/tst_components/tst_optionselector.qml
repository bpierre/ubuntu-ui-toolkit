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
import Ubuntu.Test 0.1

UbuntuTestCase {
     name: "OptionSelectorAPI"

     function test_expanded() {
         var listContainer = findChild(optionSelector, "listContainer");

         optionSelector.expanded = false;
         compare(listContainer.isExpanded, false, "isExpanded should be true if list is an expanded one");

         optionSelector.expanded = true;
         compare(listContainer.isExpanded, true, "isExpanded should be false if list is an expanded one");
     }

     function test_states() {
         var listContainer = findChild(optionSelector, "listContainer");

         optionSelector.expanded = false;
         compare(listContainer.state, "collapsed", "state should be collapsed");

         optionSelector.expanded = true;
         compare(listContainer.state, "expanded", "state should be expanded");
     }

     function test_text() {
         compare(optionSelector.text,"","text is '' by default")
         var newText = "Hello World!";
         optionSelector.text = newText;
         compare(optionSelector.text, newText, "set/get");
     }

     function test_selectedIndex() {
        compare(optionSelector.selectedIndex,0,"selectedIndex is 0 by default");
     }

     function test_values() {
         compare(optionSelector.values,undefined,"values is undefined by default")
         var newValues = ["value1","value2","value3"];
         optionSelector.values = newValues;
         compare(optionSelector.values, newValues, "set/get");
     }

     function test_signal_triggered() {
         signalSpy.signalName = "scroll";
         compare(signalSpy.count, 0);
         optionSelector.scroll(0, 0);
         compare(signalSpy.count, 1);
     }

     Rectangle {
         id: testItem
     }

     OptionSelector {
         id: optionSelector

         SignalSpy {
             id: signalSpy
             target: optionSelector
         }
     }
}
