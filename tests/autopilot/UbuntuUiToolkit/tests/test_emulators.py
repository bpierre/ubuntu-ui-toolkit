# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


from UbuntuUiToolkit import tests
from UbuntuUiToolkit import emulators


class MainViewTestCase(tests.UbuntuUiToolkitTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Page {
        title: "Test title"
    }
}
""")

    def test_main_view_custom_emulator(self):
        self.assertIsInstance(self.main_view, emulators.MainView)

    def test_get_header(self):
        header = self.main_view.get_header()
        self.assertEqual(header.title, "Test title")
