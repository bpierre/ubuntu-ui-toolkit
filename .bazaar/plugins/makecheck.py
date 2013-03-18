#!/bin/bash
# Copyright 2013 Canonical Ltd.
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
#
# Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>

from bzrlib import branch

def execute_makecheck(local_branch, master_branch, old_revision_number, old_revision_id, future_revision_number, future_revision_id, tree_delta, future_tree):
    import os,subprocess
    from bzrlib import errors
 
    if (subprocess.call("make check", shell=True) != 0):
        raise errors.BzrError("Tests failed, fix them before commit!")

branch.Branch.hooks.install_named_hook('pre_commit', execute_makecheck, 'make check pre-commit')
