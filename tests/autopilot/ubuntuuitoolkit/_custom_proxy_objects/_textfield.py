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

from ubuntuuitoolkit._custom_proxy_objects import (
    _common,
    _mainview
)

logger = logging.getLogger(__name__)


class TextField(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """TextField Autopilot custom proxy object."""

    def __init__(self, *args):
        super().__init__(*args)
        self.keyboard = _common.get_keyboard()

    @autopilot_logging.log_action(logger.info)
    def write(self, text, clear=True):
        """Write into the text field.

        :parameter text: The text to write.
        :parameter clear: If True, the text field will be cleared before
            writing the text. If False, the text will be appended at the end
            of the text field. Default is True.

        """
        self._ensure_focused()
        if clear:
            self.clear()
        else:
            if not self.is_empty():
                self._go_to_end()
        self.keyboard.type(text)

    @autopilot_logging.log_action(logger.info)
    def clear(self):
        """Clear the text field."""
        self._ensure_focused()
        if not self.is_empty():
            if self.hasClearButton:
                self._click_clear_button()
            else:
                self._clear_with_keys()
            self.displayText.wait_for('')

    def is_empty(self):
        """Return True if the text field is empty. False otherwise."""
        return self.displayText == ''

    @autopilot_logging.log_action(logger.debug)
    def _click_clear_button(self):
        clear_button = self.select_single(
            'AbstractButton', objectName='clear_button')
        if not clear_button.visible:
            self.pointing_device.click_object(self)
        self.pointing_device.click_object(clear_button)

    @autopilot_logging.log_action(logger.debug)
    def _clear_with_keys(self):
        if self._is_keyboard_osk():
            # Touch tap currently doesn't have a press_duration parameter, so
            # we can't select all the text.
            # Reported as bug http://pad.lv/1268782 --elopio - 2014-01-13
            self._go_to_end()
            while self.displayText != '':
                self._delete_one_character_using_osk()
        else:
            self._select_all()
            self.keyboard.press_and_release('BackSpace')
        if not self.is_empty():
            raise _common.ToolkitException('Failed to clear the text field.')

    @autopilot_logging.log_action(logger.debug)
    def _select_all(self):
        if not self._is_all_text_selected():
            # right click is needed
            self.pointing_device.click_object(self, button=3)
            root = self.get_root_instance()
            main_view = root.select_single(_mainview.MainView)
            popover = main_view.get_text_input_context_menu(
                'text_input_contextmenu')
            popover.click_option_by_text('Select All')

    def _is_all_text_selected(self):
        return self.displayText == self.selectedText

    def _is_keyboard_osk(self):
        """Return True if the keyboard instance is the OSK."""
        from autopilot.input import _osk
        return isinstance(self.keyboard, _osk.Keyboard)

    @autopilot_logging.log_action(logger.debug)
    def _go_to_end(self):
        from autopilot import input
        if self._is_keyboard_osk():
            # XXX Here we are cheating because the on-screen keyboard doesn't
            # have an END key. --elopio - 2014-08-20
            keyboard = input.Keyboard.create()
            keyboard.press_and_release('End')
        else:
            self.keyboard.press_and_release('End')

    @autopilot_logging.log_action(logger.debug)
    def _delete_one_character_using_osk(self):
        original_text = self.displayText
        # We delete with backspace because the on-screen keyboard has
        # that key.
        self.keyboard.press_and_release('backspace')
        if len(self.displayText) != len(original_text) - 1:
            raise _common.ToolkitException('Failed to delete one character.')

    @autopilot_logging.log_action(logger.debug)
    def _ensure_focused(self):
        if not self.focus:
            self.pointing_device.click_object(self)
            self.focus.wait_for(True)
