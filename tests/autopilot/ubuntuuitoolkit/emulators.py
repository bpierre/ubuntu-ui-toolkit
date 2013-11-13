# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013 Canonical Ltd.
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
from distutils import version

import autopilot
from autopilot import input, platform
from autopilot.introspection import dbus
from time import sleep

_NO_TABS_ERROR = 'The MainView has no Tabs.'

logger = logging.getLogger(__name__)


class ToolkitEmulatorException(Exception):
    """Exception raised when there is an error with the emulator."""


def get_pointing_device():
    """Return the pointing device depending on the platform.

    If the platform is `Desktop`, the pointing device will be a `Mouse`.
    If not, the pointing device will be `Touch`.

    """
    if platform.model() == 'Desktop':
        input_device_class = input.Mouse
    else:
        input_device_class = input.Touch
    return input.Pointer(device=input_device_class.create())


def check_autopilot_version():
    """Check that the Autopilot installed version matches the one required.

    :raise ToolkitEmulatorException: If the installed Autopilot version does't
        match the required by the emulators.

    """
    installed_version = version.LooseVersion(autopilot.version)
    if installed_version < version.LooseVersion('1.4'):
        raise ToolkitEmulatorException(
            'The emulators need Autopilot 1.4 or higher.')


class UbuntuUIToolkitEmulatorBase(dbus.CustomEmulatorBase):
    """A base class for all the Ubuntu UI Toolkit emulators."""

    def __init__(self, *args):
        check_autopilot_version()
        super(UbuntuUIToolkitEmulatorBase, self).__init__(*args)
        self.pointing_device = get_pointing_device()
        # TODO it would be nice to have access to the screen keyboard if we are
        # on the touch UI -- elopio - 2013-09-04


class MainView(UbuntuUIToolkitEmulatorBase):
    """MainView Autopilot emulator."""

    def get_header(self):
        """Return the Header emulator of the MainView."""
        try:
            return self.select_single('Header', objectName='MainView_Header')
        except dbus.StateNotFoundError:
            raise ToolkitEmulatorException('The main view has no header.')

    def get_toolbar(self):
        """Return the Toolbar emulator of the MainView."""
        return self.select_single(Toolbar)

    def open_toolbar(self):
        """Open the toolbar if it's not already opened.

        :return: The toolbar.

        """
        return self.get_toolbar().open()

    def close_toolbar(self):
        """Close the toolbar if it's opened."""
        self.get_toolbar().close()

    def get_tabs(self):
        """Return the Tabs emulator of the MainView.

        :raise ToolkitEmulatorException: If the main view has no tabs.

        """
        try:
            return self.select_single(Tabs)
        except dbus.StateNotFoundError:
            raise ToolkitEmulatorException(_NO_TABS_ERROR)

    def switch_to_next_tab(self):
        """Open the next tab.

        :return: The newly opened tab.

        """
        logger.debug('Switch to next tab.')
        self.get_header().switch_to_next_tab()
        current_tab = self.get_tabs().get_current_tab()
        current_tab.visible.wait_for(True)
        return current_tab

    def switch_to_tab_by_index(self, index):
        """Open a tab.

        :parameter index: The index of the tab to open.
        :return: The newly opened tab.
        :raise ToolkitEmulatorException: If the tab index is out of range.

        """
        logger.debug('Switch to tab with index {0}.'.format(index))
        tabs = self.get_tabs()
        number_of_tabs = tabs.get_number_of_tabs()
        if index >= number_of_tabs:
            raise ToolkitEmulatorException('Tab index out of range.')
        current_tab = tabs.get_current_tab()
        number_of_switches = 0
        while not tabs.selectedTabIndex == index:
            logger.debug(
                'Current tab index: {0}.'.format(tabs.selectedTabIndex))
            if number_of_switches >= number_of_tabs - 1:
                # This prevents a loop. But if this error is ever raised, it's
                # likely there's a bug on the emulator or on the QML Tab.
                raise ToolkitEmulatorException(
                    'The tab with index {0} was not selected.'.format(index))
            current_tab = self.switch_to_next_tab()
            number_of_switches += 1
        return current_tab

    def switch_to_previous_tab(self):
        """Open the previous tab.

        :return: The newly opened tab.

        """
        tabs = self.get_tabs()
        if tabs.selectedTabIndex == 0:
            previous_tab_index = tabs.get_number_of_tabs() - 1
        else:
            previous_tab_index = tabs.selectedTabIndex - 1
        return self.switch_to_tab_by_index(previous_tab_index)

    def switch_to_tab(self, object_name):
        """Open a tab.

        :parameter object_name: The QML objectName property of the tab.
        :return: The newly opened tab.
        :raise ToolkitEmulatorException: If there is no tab with that object
            name.

        """
        tabs = self.get_tabs()
        for index, tab in enumerate(tabs.select_many('Tab')):
            if tab.objectName == object_name:
                return self.switch_to_tab_by_index(tab.index)
        raise ToolkitEmulatorException(
            'Tab with objectName "{0}" not found.'.format(object_name))

    def get_action_selection_popover(self, object_name):
        """Return an ActionSelectionPopover emulator.

        :parameter object_name: The QML objectName property of the popover.

        """
        return self.select_single(
            ActionSelectionPopover, objectName=object_name)

    def go_back(self):
        """Go to the previous page."""
        toolbar = self.open_toolbar()
        toolbar.click_back_button()


class Header(UbuntuUIToolkitEmulatorBase):
    """Header Autopilot emulator."""

    def __init__(self, *args):
        super(Header, self).__init__(*args)
        self.pointing_device = get_pointing_device()

    def _get_animating(self):
        tab_bar_style = self.select_single('TabBarStyle')
        return tab_bar_style.animating

    def switch_to_next_tab(self):
        """Open the next tab.

        :raise ToolkitEmulatorException: If the main view has no tabs.

        """
        try:
            tab_bar = self.select_single(TabBar)
        except dbus.StateNotFoundError:
            raise ToolkitEmulatorException(_NO_TABS_ERROR)
        tab_bar.switch_to_next_tab()
        self._get_animating().wait_for(False)


class Toolbar(UbuntuUIToolkitEmulatorBase):
    """Toolbar Autopilot emulator."""

    def open(self):
        """Open the toolbar if it's not already opened.

        :return: The toolbar.

        """
        self.animating.wait_for(False)
        if not self.opened:
            self._drag_to_open()
            self.opened.wait_for(True)
            self.animating.wait_for(False)

        return self

    def _drag_to_open(self):
        x, y, _, _ = self.globalRect
        line_x = x + self.width * 0.50
        start_y = y + self.height - 1
        stop_y = y

        self.pointing_device.drag(line_x, start_y, line_x, stop_y)

    def close(self):
        """Close the toolbar if it's opened."""
        self.animating.wait_for(False)
        if self.opened:
            self._drag_to_close()
            self.opened.wait_for(False)
            self.animating.wait_for(False)

    def _drag_to_close(self):
        x, y, _, _ = self.globalRect
        line_x = x + self.width * 0.50
        start_y = y
        stop_y = y + self.height - 1

        self.pointing_device.drag(line_x, start_y, line_x, stop_y)

    def click_button(self, object_name):
        """Click a button of the toolbar.

        The toolbar should be opened before clicking the button, or an
        exception will be raised. If the toolbar is closed for some reason
        (e.g., timer finishes) after moving the mouse cursor and before
        clicking the button, it is re-opened automatically by this function.

        :parameter object_name: The QML objectName property of the button.
        :raise ToolkitEmulatorException: If there is no button with that object
            name.

        """
        try:
            button = self._get_button(object_name)
        except dbus.StateNotFoundError:
            raise ToolkitEmulatorException(
                'Button with objectName "{0}" not found.'.format(object_name))
        # ensure the toolbar is open
        if not self.opened:
            raise ToolkitEmulatorException(
                'Toolbar must be opened before calling click_button().')
        self.pointing_device.move_to_object(button)
        # ensure the toolbar is still open (may have closed due to timeout)
        self.open()
        # click the button
        self.pointing_device.click_object(button)

    def _get_button(self, object_name):
        return self.select_single('ActionItem', objectName=object_name)

    def click_back_button(self):
        """Click the back button of the toolbar."""
        self.click_button('back_toolbar_button')


class Tabs(UbuntuUIToolkitEmulatorBase):
    """Tabs Autopilot emulator."""

    def get_current_tab(self):
        """Return the currently selected tab."""
        return self._get_tab(self.selectedTabIndex)

    def _get_tab(self, index):
        tabs = self._get_tabs()
        for tab in tabs:
            if tab.index == index:
                return tab
        else:
            raise ToolkitEmulatorException(
                'There is no tab with index {0}.'.format(index))

    def _get_tabs(self):
        return self.select_many('Tab')

    def get_number_of_tabs(self):
        """Return the number of tabs."""
        return len(self._get_tabs())


class TabBar(UbuntuUIToolkitEmulatorBase):
    """TabBar Autopilot emulator."""

    def switch_to_next_tab(self):
        """Open the next tab."""
        self._activate_tab_bar()
        logger.debug('Click the next tab bar button.')
        self.pointing_device.click_object(self._get_next_tab_button())

    def _activate_tab_bar(self):
        if self.selectionMode:
            logger.debug('Already in selection mode.')
        else:
            # Click the tab bar to switch to selection mode.
            logger.debug('Click the tab bar to enable selection mode.')
            self.pointing_device.click_object(self)

    def _get_next_tab_button(self):
        current_index = self._get_selected_button_index()
        next_index = (current_index + 1) % self._get_number_of_tab_buttons()
        return self._get_tab_button(next_index)

    def _get_selected_button_index(self):
        return self.select_single('QQuickPathView').selectedButtonIndex

    def _get_number_of_tab_buttons(self):
        return len(self._get_tab_buttons())

    def _get_tab_buttons(self):
        return self.select_many('AbstractButton')

    def _get_tab_button(self, index):
        buttons = self._get_tab_buttons()
        for button in buttons:
            if button.buttonIndex == index:
                return button
        else:
            raise ToolkitEmulatorException(
                'There is no tab button with index {0}.'.format(index))


class ActionSelectionPopover(UbuntuUIToolkitEmulatorBase):
    """ActionSelectionPopover Autopilot emulator."""

    def click_button_by_text(self, text):
        """Click a button on the popover.

        XXX We are receiving the text because there's no way to set the
        objectName on the action. This is reported at
        https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205144
        --elopio - 2013-07-25

        :parameter text: The text of the button.
        :raise ToolkitEmulatorException: If the popover is not open.

        """
        if not self.visible:
            raise ToolkitEmulatorException('The popover is not open.')
        button = self._get_button(text)
        if button is None:
            raise ToolkitEmulatorException(
                'Button with text "{0}" not found.'.format(text))
        self.pointing_device.click_object(button)
        if self.autoClose:
            self.visible.wait_for(False)

    def _get_button(self, text):
        buttons = self.select_many('Empty')
        for button in buttons:
            if button.text == text:
                return button


class CheckBox(UbuntuUIToolkitEmulatorBase):
    """CheckBox Autopilot emulator."""

    def check(self, timeout=10):
        """Check a CheckBox, if its not already checked.

        :parameter timeout: number of seconds to wait for the CheckBox to be
            checked. Default is 10.

        """
        if not self.checked:
            self.change_state(timeout)

    def uncheck(self, timeout=10):
        """Uncheck a CheckBox, if its not already unchecked.

        :parameter timeout: number of seconds to wait for the CheckBox to be
            unchecked. Default is 10.

        """
        if self.checked:
            self.change_state(timeout)

    def change_state(self, timeout=10):
        """Change the state of a CheckBox.

        If it is checked, it will be unchecked. If it is unchecked, it will be
        checked.

        :parameter time_out: number of seconds to wait for the CheckBox state
            to change. Default is 10.

        """
        original_state = self.checked
        self.pointing_device.click_object(self)
        self.checked.wait_for(not original_state, timeout)


class OptionSelector(UbuntuUIToolkitEmulatorBase):
    """OptionSelector Autopilot emulator"""

    def get_option_count(self):
        """Gets the number of items in the option selector"""
        self.list_view = self.select_single("QQuickListView")
        return self.list_view.count

    def get_current_index(self):
        """Gets the current selected index of the QQuickListView"""
        self.list_view = self.select_single("QQuickListView")
        return self.list_view.currentIndex

    def expand(self):
        """Expand an optionselector if it's collapsed"""
        #if just collapsed it can think that the item is expanded
        # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1240288
        sleep(1)
        if not self.alwaysExpanded and not self.expanded:
            self.pointing_device.click_object(self)
            self.expanded.wait_for(True)
            #selecting the same item too quickly after expand
            #causes the wrong item to be selected
            #https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1231939
            sleep(1)

    def _get_delegate(self, *args, **kwargs):
        delegates = self.select_many('OptionSelectorDelegate')
        for delegate in delegates:
            try:
                if delegate.select_single(*args, **kwargs):
                    return delegate
            except dbus.StateNotFoundError:
                pass
        raise ToolkitEmulatorException(
            'could not find delegate with options args {} kwargs {}'.format(
                args,
                kwargs.items(),
            )
        )

    def get_current_selected_text(self):
        """gets the text of the currently selected item"""
        option_selector_delegate = self.select_single(
            'OptionSelectorDelegate', focus='True')
        current_label = option_selector_delegate.select_single(
            'Label', visible='True')
        return current_label.text

    def select_option(self, *args, **kwargs):
        """Select delegate in option selector

        """
        select_object = self._get_delegate(*args, **kwargs)
        self.expand()
        self.pointing_device.click_object(select_object)


class Empty(UbuntuUIToolkitEmulatorBase):
    """Base class to emulate swipe to delete"""

    def _get_confirm_button(self):
        return self.select_single(
            'QQuickItem', objectName='confirmRemovalDialog')

    def swipe_to_delete(self, direction='right'):
        """ Swipe the item in a specific direction """
        if (self.removable):
            x, y, w, h = self.globalRect
            tx = x + (w / 8)
            ty = y + (h / 2)

            if (direction == 'right'):
                self.pointing_device.drag(tx, ty, w, ty)
            elif (direction == 'left'):
                self.pointing_device.drag(w - (w*0.1), ty, x, ty)
            else:
                raise ToolkitEmulatorException(
                    'Invalid direction "{0}" used on swipe to delete function'
                    .format(direction))

            self.waitingConfirmationForRemoval.wait_for(True)
        else:
            raise ToolkitEmulatorException(
                'The item "{0}" is not removable'.format(self.objectName))

    def confirm_removal(self):
        """ Comfirm item removal if this was already swiped """
        if (self.waitingConfirmationForRemoval):
            deleteButton = self._get_confirm_button()
            self.pointing_device.click_object(deleteButton)
            self.implicitHeight.wait_for(0)
        else:
            raise ToolkitEmulatorException(
                'The item "{0}" is not waiting for removal confirmation'.
                format(self.objectName))


class Base(Empty):
    pass


class Standard(Empty):
    pass


class ItemSelector(Empty):
    pass


class SingleControl(Empty):
    pass


class MultiValue(Base):
    pass


class SingleValue(Base):
    pass


class Subtitled(Base):
    pass
