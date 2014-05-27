# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Gallery - UbuntuListView component."""

import ubuntuuitoolkit
from ubuntuuitoolkit import emulators
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class UbuntuListViewTestCase(GalleryTestCase):

    def setup(self):
        super(UbuntuListViewTestCase, self).setUp()

    def test_pull_to_refresh_enabled(self):
        item = "UbuntuListView"
        self.loadItem(item)
        self.checkPageHeader(item)
