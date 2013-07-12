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
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "MainViewAPI"

    MainView {
        id: mainView
    }

    function initTestCase() {
        compare(mainView.active, true, "MainView always active.");
        compare(mainView.automaticOrientation, false, "Automatic orientation off by default.");
        compare(mainView.applicationName, "", "Application name is empty string by default.");
    }

    function test_automaticOrientation() {
        mainView.automaticOrientation = true;
        compare(mainView.automaticOrientation, true, "Automatic orientation can be enabled.");
        mainView.automaticOrientation = false;
        compare(mainView.automaticOrientation, false, "Automaic orientation can be disabled.");
    }

    function test_applicationName() {
        var appName = "testAppName";
        mainView.applicationName = appName;
        compare(mainView.applicationName, appName, "Applicatin name can be set.");
        mainView.applicationName = "";
        compare(mainView.applicationName, "", "Application name can be unset.");
    }
}
