#!/usr/bin/env python
#
# This file is part of the TrackStar package.
# Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
# License: MIT License. See LICENSE in top-level directory
# at: https://github.com/giganano/trackstar.git.
#
# Configuration file for sphinx documentation builder.

try:
	import sphinx
except (ModuleNotFoundError, ImportError):
	raise RuntimeError("Sphinx >= 6.0.0 is required to compile documentation.")
if tuple([int(_) for _ in sphinx.__version__.split('.')[:2]]) < (6, 0):
	raise RuntimeError("Sphinx >= 6.0.0 is required to compile documentation. \
Current version: %s""" % (sphinx.__version__))
else: pass
try:
	import trackstar
except (ModuleNotFoundError, ImportError):
	raise RuntimeError("""trackstar must be installed before its \
documentation can be compiled.""")

# -- Project information -----------------------------------------------------
project = "trackstar"
copyright = "2023, James W. Johnson"
author = trackstar.__author__
release = trackstar.__version__


# -- General configuration ---------------------------------------------------
extensions = ["sphinx.ext.todo", "sphinx.ext.intersphinx", "nbsphinx"]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------
html_theme = "sphinx_rtd_theme"
html_style = "css/custom.css"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

