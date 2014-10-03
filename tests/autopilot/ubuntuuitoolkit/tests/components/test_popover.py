# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Header component."""

import os

from ubuntuuitoolkit import tests


class DialogScrollTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield.qml')

    def setUp(self):
        super(DialogScrollTestCase, self).setUp()

    def test_dialog_small(self):
        button = self.main_view_select_single(objectName='button_small')
        self.pointing_device.click_object(button)

    def test_dialog_scrollable(self):
        button = self.main_view.select_single(objectName='button_huge')
        self.pointing_device.click_object(button)
        dialog = self.main_view.select_single(objectName='dialog_huge')
        flickable = dialog.select_single(
            'QQuickFlickable', objectName='flickable_huge')
        butlet = dialog.select_single(objectName='butlet1000')
        flickable.swipe_child_into_view(butlet)
        self.assertEqual(flickable.is_child_visible(), True)
