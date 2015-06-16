#!/usr/bin/env python
'''
  This is a fairly lazy way to add a general path for this module
'''
from os import getenv

ecosystem_path = getenv("ECOSYSTEM_PATH")
if ecosystem_path == None:
  print
  print "****************************************************************"
  print "You must export ECOSYSTEM_PATH in your .bashrc.  See the README."
  print "****************************************************************"
  print
