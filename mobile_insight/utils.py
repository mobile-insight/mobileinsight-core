# -*- coding: utf-8 -*-
"""
utils.py
Include definitions of some utility functions and decorators.

Author: Jiayao Li
"""

__all__ = ["static_var"]


def static_var(varname, value):
    def decorate(func):
        setattr(func, varname, value)
        return func
    return decorate
