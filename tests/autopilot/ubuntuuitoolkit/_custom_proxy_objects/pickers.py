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

import datetime
import logging

from autopilot import logging as autopilot_logging
from autopilot.introspection import dbus

from ubuntuuitoolkit._custom_proxy_objects import (
    _common,
    _flickable,
    _qquicklistview
)


logger = logging.getLogger(__name__)


class DatePicker(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Autopilot helper for the DatePicker component."""

    @autopilot_logging.log_action(logger.info)
    def pick_date(self, date):
        """Pick a date from the date picker.

        :parameter date: The date to pick.
        :type date: An object with year, month and day attributes, like
            python's datetime.date.
        :raises ubuntuuitoolkit.ToolkitException if the mode of the picker
            doesn't let select a date.

        """
        if not self._is_date_picker():
            raise _common.ToolkitException(
                "Can't pick date. The picker mode is: {!r}.".format(self.mode))
        if 'Years' in self.mode:
            self._pick_year(date.year)
            self.year.wait_for(date.year)
        if 'Month' in self.mode:
            # Python's date object starts at one. The model in the date picker
            # at 0.
            self._pick_month(date.month - 1)
            self.month.wait_for(date.month - 1)
        if 'Day' in self.mode:
            self._pick_day(date.day)
            self.day.wait_for(date.day)

    def _is_date_picker(self):
        mode = self.mode
        if 'Years' in mode or 'Months' in mode or 'Days' in mode:
            return True
        else:
            return False

    @autopilot_logging.log_action(logger.info)
    def _pick_year(self, year):
        picker = self.select_single(
            'Picker', objectName='PickerRow_YearPicker')
        list_view = picker.select_single(
            _qquicklistview.QQuickListView, objectName='Picker_Linear')
        self._pick_date_value(self.year, year, list_view)

    @autopilot_logging.log_action(logger.info)
    def _pick_month(self, month):
        picker = self.select_single(
            'Picker', objectName='PickerRow_MonthPicker')
        path_view = picker.select_single(
            QQuickPathView, objectName='Picker_WrapAround')
        self._pick_date_value(self.month, month, path_view)

    def _pick_day(self, day):
        picker = self.select_single(
            'Picker', objectName='PickerRow_DayPicker')
        path_view = picker.select_single(
            QQuickPathView, objectName='Picker_WrapAround')
        # Python's date object starts at one. The model in the date picker
        # at 0.
        self._pick_date_value(self.get_date().day - 1, day - 1, path_view)

    def _pick_date_value(self, current_value, new_value, scrollable):
        if new_value > current_value:
            direction = 'below'
        elif new_value < current_value:
            direction = 'above'
        else:
            logger.debug('The value is already selected.')
            return
        scrollable.click_element(
            object_name='PickerRow_PickerLabel{}'.format(new_value),
            direction=direction)

    def get_date(self):
        """Return the currently selected date.

        :return: a python datetime.date object with the selected date.

        """
        # Python's date object starts at one. The model in the date picker
        # at 0.
        return datetime.date(
            self.year, self.month + 1, self.day)


class QQuickPathView(_flickable.Scrollable):

    # TODO make it more general and move it to its own module.
    # --elopio - 2014-05-06

    @autopilot_logging.log_action(logger.info)
    def click_element(self, object_name, direction='below'):
        try:
            element = self.select_single(objectName=object_name)
        except dbus.StateNotFoundError:
            # The element might be on a part of the list that hasn't been
            # created yet. We have to search for it scrolling.
            element = self._find_element(object_name, direction)
        self.swipe_child_into_view(element)
        self.pointing_device.click_object(element)

    @autopilot_logging.log_action(logger.info)
    def _find_element(self, object_name, direction):
        containers = self._get_containers()
        for index in range(self.count):
            if direction == 'below':
                swipe_method = self._swipe_to_show_one_more_below
            elif direction == 'above':
                swipe_method = self._swipe_to_show_one_more_above
            else:
                raise _common.ToolkitException(
                    'Invalid direction: {}'.format(direction))

            swipe_method(containers)

            try:
                return self.select_single(objectName=object_name)
            except dbus.StateNotFoundError:
                pass
        raise _common.ToolkitException(
            'List element with objectName "{}" not found.'.format(object_name))

    @autopilot_logging.log_action(logger.info)
    def _swipe_to_show_one_more_above(self, containers):
        self._swipe_to_show_one_more('above', containers)

    @autopilot_logging.log_action(logger.info)
    def _swipe_to_show_one_more_below(self, containers):
        self._swipe_to_show_one_more('below', containers)

    def _swipe_to_show_one_more(self, direction, containers):
        start_x = stop_x = self.globalRect.x + (self.globalRect.width // 2)
        center_y = self.globalRect.y + (self.globalRect.height // 2)
        # XXX This assumes all the children are of the same height
        child = self.get_children_by_type('PickerDelegate')[0]
        top = center_y - (child.globalRect.height // 2)
        bottom = center_y + (child.globalRect.height // 2)
        if direction == 'below':
            start_y = bottom
            stop_y = top
        elif direction == 'above':
            start_y = top
            stop_y = bottom
        else:
            raise _common.ToolkitException(
                'Invalid direction {}.'.format(direction))
        self._slow_drag(start_x, stop_x, start_y, stop_y)
        self.dragging.wait_for(False)
        self.moving.wait_for(False)

    @autopilot_logging.log_action(logger.info)
    def swipe_child_into_view(self, child):
        """Make the child visible.

        Currently it works only when the object needs to be swiped vertically.
        TODO implement horizontal swiping. --elopio - 2014-03-21

        """
        containers = self._get_containers()
        if not self._is_child_visible(child, containers):
            self._swipe_non_visible_child_into_view(child, containers)
        else:
            logger.debug('The element is already visible.')

    @autopilot_logging.log_action(logger.info)
    def _swipe_non_visible_child_into_view(self, child, containers):
        while not self._is_child_visible(child, containers):
            # Check the direction of the swipe based on the position of the
            # child relative to the immediate flickable container.
            if child.globalRect.y < self.globalRect.y:
                self._swipe_to_show_one_more_above(containers)
            else:
                self._swipe_to_show_one_more_below(containers)
