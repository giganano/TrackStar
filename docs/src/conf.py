r"""
This file is part of the trackfitting package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/trackfitting.git.
"""
# Configuration file for sphinx documentation builder.

import sys
if sys.version_info[:2] < (3, 7): raise RuntimeError("""\
Python version >= 3.7.0 required to compile documentation.""")

try:
	import sphinx
except (ModuleNotFoundError, ImportError):
	raise RuntimeError("Sphinx >= 2.0.0 is required to compile documentation.")
if tuple([int(_) for _ in sphinx.__version__.split('.')[:2]]) < (2, 0):
	raise RuntimeError("Sphinx >= 2.0.0 is required to compile documentation. \
Current version: %s""" % (sphinx.__version__))
else: pass
try:
	import track_fitting
except (ModuleNotFoundError, ImportError):
	raise RuntimeError("""trackfitting must be installed before its \
documentation can be compiled.""")

# -- Project information -----------------------------------------------------
project = "trackfitting"
copyright = "2023, James W. Johnson"
author = track_fitting.__author__
release = track_fitting.__version__


# -- General configuration ---------------------------------------------------
extensions = []

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------
html_theme = "nature"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

