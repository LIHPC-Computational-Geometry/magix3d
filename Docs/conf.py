# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
sys.path.insert(0, os.path.abspath('./_ext'))

import variables

# -- Project information -----------------------------------------------------

project = 'magix3D'
copyright = '2023, LIHPC Computational-Geometry Team'
author = 'LIHPC Computational-Geometry Team'

# The full version, including alpha/beta/rc tags
version = variables.magix_version

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ['sphinx_rtd_theme', 'breathe', 'magix', 'sphinx_copybutton']

# Breathe Configuration
breathe_projects = {"Magix3D": variables.doxygen_build_path,}
breathe_default_project = "Magix3D"
breathe_doxygen_config_options = {
    'SHOW_USED_FILES': 'NO', 
    'SHOW_INCLUDE_FILES' : 'NO',
    'OUTPUT_LANGUAGE' : 'French'
}
breathe_show_include = False
breathe_separate_member_pages = True

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'
#html_theme = 'classic'


html_theme_options = {
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_css_files = [ 'custom.css']

html_scaled_image_link = False

html_title = "magix3D"
html_logo = "images/magix3d.png"
html_favicon = "images/magix3DIcone.png"

html_copy_source = False
html_show_sourcelink = False

language = "fr"

latex_elements = {
    'fontenc': r'\usepackage[LGR,T1]{fontenc}',
    'textgreek': r'\usepackage{textalpha,alphabeta}',
    'hyperref':r'\usepackage{hyperref}'
 }
 
latex_documents = [('pages/manuel-utilisateur', 'magix3d.tex', 'Manuel Utilisateur Magix3d', '', 'article')]
