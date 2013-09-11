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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

"""Base classes for Autopilot tests using the Ubuntu UI Toolkit."""

from autopilot import (
    input,
    platform,
    testcase
)


class UbuntuUIToolkitAppTestCase(testcase.AutopilotTestCase):
    """Autopilot test case for applications using the Ubuntu UI Toolkit."""

    application_qml_path = ''

    def setUp(self):
        super(UbuntuUIToolkitAppTestCase, self).setUp()
        self.input_device_class = self._get_input_device_class()
        self.launch_application()

    def _get_input_device_class(self):
        if platform.model() == 'Desktop':
            return input.Mouse
        else:
            return input.Touch

    def launch_application(self):
        if self.local_application_exists():
            self.launch_local_application()
        else:
            self.launch_installed_application()

    def local_application_exists(self):
        raise NotImplementedError('This must be implemented on a subclass')

    def launch_installed_application(self):
        raise NotImplementedError('This must be implemented on a subclass')

    def launch_installed_application(self):
        raise NotImplementedError('This must be implemented on a subclass')
