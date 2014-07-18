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

import logging

from autopilot import logging as autopilot_logging
from autopilot.introspection import dbus

from ubuntuuitoolkit._custom_proxy_objects import (
    _common,
    _tabbar
)


_NO_TABS_ERROR = 'The MainView has no Tabs.'


logger = logging.getLogger(__name__)


class AppHeader(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """AppHeader Autopilot emulator."""

    def _show_if_not_visible(self):
        if not self._is_visible():
            self._show()

    def _is_visible(self):
        return self.y == 0

    def _show(self):
        # FIXME This will fail if the header is not linked to a flickable that
        # fills the main view. The header has a flickable property but it
        # can't be read by autopilot. See bug http://pad.lv/1318829
        top_container = self._get_top_container()
        start_x = stop_x = (self.globalRect.x + self.globalRect.width) // 2
        start_y = top_container.globalRect.y + 5
        stop_y = start_y + self.globalRect.height
        self.pointing_device.drag(start_x, start_y, stop_x, stop_y)
        self.y.wait_for(0)

    @autopilot_logging.log_action(logger.info)
    def switch_to_section_by_index(self, index):
        """Select a section in the header divider

        :parameter index: The index of the section to select
        :raise ToolkitEmulatorException: If the selection index is out of range or
                useDeprecatedToolbar is set.

        """
        self._show_if_not_visible()

        if self.useDeprecatedToolbar:
            raise _common.ToolkitException('Old header has no sections')

        try:
            object_name = "section_button_" + str(index)
            button = self.select_single(
                'AbstractButton', objectName=object_name)
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(
                'Button for section with given index not found')

        self.pointing_device.click_object(button)


    def get_selected_section_index(self):
        if self.useDeprecatedToolbar:
            raise _common.ToolkitException('Old header has no sections')

        sectionsProperties = self.select_single('QQuickItem', objectName='sectionsProperties')
        return sectionsProperties.selectedIndex

    def click_back_button(self):
        self._show_if_not_visible()

        if self.useDeprecatedToolbar:
            raise _common.ToolkitException('Old header has no back button')
        try:
            back_button = self.select_single(
                'AbstractButton', objectName='backButton')
        except dbus.StateNotFoundError:
            raise _common.ToolkitException('Missing back button in header')
        if not back_button.visible:
            raise _common.ToolkitException('Back button in header not visible')
        self.pointing_device.click_object(back_button)

    def click_custom_back_button(self):
        self._show_if_not_visible()

        if self.useDeprecatedToolbar:
            raise _common.ToolkitException(
                'Old header has no custom back button')
        try:
            custom_back_button = self.select_single(
                'AbstractButton', objectName='customBackButton')
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(
                'Missing custom back button in header')
        if not custom_back_button.visible:
            raise _common.ToolkitException(
                'Custom back button in header not visible')
        self.pointing_device.click_object(custom_back_button)

    def _get_animating(self):
        if self.useDeprecatedToolbar:
            tab_bar_style = self.select_single('TabBarStyle')
            return tab_bar_style.animating
        else:
            return False

    @autopilot_logging.log_action(logger.info)
    def switch_to_next_tab(self):
        """Open the next tab.

        :raise ToolkitEmulatorException: If the main view has no tabs.

        """
        self._show_if_not_visible()

        if self.useDeprecatedToolbar:
            self._switch_to_next_tab_in_deprecated_tabbar()
        else:
            self._switch_to_next_tab_in_drawer()

    def _switch_to_next_tab_in_deprecated_tabbar(self):
        try:
            tab_bar = self.select_single(_tabbar.TabBar)
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(_NO_TABS_ERROR)
        tab_bar.switch_to_next_tab()
        self._get_animating().wait_for(False)

    def _switch_to_next_tab_in_drawer(self):
        tabs_model_properties = self.select_single(
            'QQuickItem', objectName='tabsModelProperties')
        next_tab_index = (tabs_model_properties.selectedIndex
                          + 1) % tabs_model_properties.count
        self._switch_to_tab_in_drawer_by_index(next_tab_index)

    @autopilot_logging.log_action(logger.info)
    def switch_to_tab_by_index(self, index):
        """Open a tab. This only supports the new tabs in the header

        :parameter index: The index of the tab to open.
        :raise ToolkitEmulatorException: If the tab index is out of range or
                useDeprecatedToolbar is set.

        """
        self._show_if_not_visible()

        if self.useDeprecatedToolbar:
            raise _common.ToolkitException(
                "AppHeader.switch_to_tab_by_index only works with new header")
        else:
            self._switch_to_tab_in_drawer_by_index(index)

    def _switch_to_tab_in_drawer_by_index(self, index):
        try:
            tabs_drawer_button = self.select_single(
                'AbstractButton', objectName='tabsButton')
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(_NO_TABS_ERROR)
        self.pointing_device.click_object(tabs_drawer_button)

        tabs_model_properties = self.select_single(
            'QQuickItem', objectName='tabsModelProperties')

        if tabs_model_properties.selectedIndex == index:
            return

        try:
            tab_button = self.get_root_instance().select_single(
                'Standard', objectName='tabButton' + str(index))
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(
                "Tab button {0} not found.".format(index))

        self.pointing_device.click_object(tab_button)

    def click_action_button(self, action_object_name):
        """Click an action button of the header.

        :parameter object_name: The QML objectName property of the action
        :raise ToolkitException: If there is no action button with that object
            name.

        """
        self._show_if_not_visible()

        button = self._get_action_button(action_object_name)
        self.pointing_device.click_object(button)

    def _get_action_button(self, action_object_name):
        try:
            object_name = action_object_name + "_header_button"
            button = self.select_single(
                'AbstractButton', objectName=object_name)
        except dbus.StateNotFoundError:
            # the button is not in the header, but it may be in the overflow
            try:
                button = self._get_action_button_in_overflow(
                    action_object_name)
            except dbus.StateNotFoundError:
                raise _common.ToolkitException(
                    'Button not found in header or overflow')

        return button

    def _get_action_button_in_overflow(self, action_object_name):
        actions_overflow_button = self.select_single(
            'AbstractButton',
            objectName='actions_overflow_button')

        if not actions_overflow_button.visible:
            raise _common.ToolkitException('No actions in overflow')

        # open the popover
        self.pointing_device.click_object(actions_overflow_button)
        object_name = action_object_name + "_header_overflow_button"

        # the popover is not a child of the header, so use the root object
        # to find the requested button
        return self.get_root_instance().select_single(
            'Standard', objectName=object_name)


class Header(AppHeader):
    """Autopilot helper for the deprecated Header."""

    def __init__(self, *args):
        logger.warning(
            'Header is an internal QML component of Ubuntu.Components and '
            'its API may change or be removed at any moment. Please use '
            'MainView and Page instead.')
        super(Header, self).__init__(*args)
