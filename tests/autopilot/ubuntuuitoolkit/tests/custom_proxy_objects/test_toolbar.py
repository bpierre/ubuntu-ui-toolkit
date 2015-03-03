# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013-2015 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

try:
    from unittest import mock
except ImportError:
    import mock

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class ToolbarTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.4
import Ubuntu.Components 1.0

MainView {
    width: units.gu(50)
    height: units.gu(50)
    objectName: "mainView"

    // Make sure that for these tests the toolbar starts closed.
    Component.onCompleted: {
        __propagated.toolbar.close();
    }

    Page {

        Label {
            id: "label"
            objectName: "clicked_label"
            anchors.centerIn: parent
            text: "Button not clicked."
        }

        tools: ToolbarItems {
            ToolbarButton {
                objectName: "buttonName"
                action: Action {
                    text: "buttonText"
                    onTriggered: label.text = "Button clicked."
                }
            }
        }
    }
}
""")

    def setUp(self):
        super(ToolbarTestCase, self).setUp()
        self.toolbar = self.main_view.get_toolbar()
        # toolbar may be opened or closed now, depending on whether
        # the application has been deactivated and resumed already

    def test_open_toolbar(self):
        self.toolbar.open()
        self.assertTrue(self.toolbar.opened)
        self.assertFalse(self.toolbar.animating)

    def test_opened_toolbar_is_not_opened_again(self):
        self.toolbar.open()
        with mock.patch.object(
                self.main_view.pointing_device, 'drag') as mock_drag:
            self.toolbar.open()

        self.assertFalse(mock_drag.called)
        self.assertTrue(self.toolbar.opened)

    def test_close_toolbar(self):
        self.toolbar.open()
        self.toolbar.close()
        self.assertFalse(self.toolbar.opened)
        self.assertFalse(self.toolbar.animating)

    def test_closed_toolbar_is_not_closed_again(self):
        self.toolbar.close()
        with mock.patch.object(
                self.main_view.pointing_device, 'drag') as mock_drag:
            self.toolbar.close()

        self.assertFalse(mock_drag.called)
        self.assertFalse(self.toolbar.opened)

    def test_click_toolbar_button(self):
        self.toolbar.close()
        label = self.app.select_single('Label', objectName='clicked_label')
        self.assertNotEqual(label.text, 'Button clicked.')
        self.toolbar.open()
        self.toolbar.click_button('buttonName')
        self.assertEqual(label.text, 'Button clicked.')

    def test_click_unexisting_button(self):
        self.main_view.open_toolbar()
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.toolbar.click_button,
            'unexisting')
        self.assertEqual(
            str(error), 'Button with objectName "unexisting" not found.')

    def test_click_button_on_closed_toolbar(self):
        self.toolbar.close()
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.toolbar.click_button,
            'buttonName')
        self.assertEqual(
            str(error),
            'Toolbar must be opened before calling click_button().')
