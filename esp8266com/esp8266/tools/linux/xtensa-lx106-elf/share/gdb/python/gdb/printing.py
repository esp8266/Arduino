# Pretty-printer utilities.
# Copyright (C) 2010-2012 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Utilities for working with pretty-printers."""

import gdb
import gdb.types
import re


class PrettyPrinter(object):
    """A basic pretty-printer.

    Attributes:
        name: A unique string among all printers for the context in which
            it is defined (objfile, progspace, or global(gdb)), and should
            meaningfully describe what can be pretty-printed.
            E.g., "StringPiece" or "protobufs".
        subprinters: An iterable object with each element having a `name'
            attribute, and, potentially, "enabled" attribute.
            Or this is None if there are no subprinters.
        enabled: A boolean indicating if the printer is enabled.

    Subprinters are for situations where "one" pretty-printer is actually a
    collection of several printers.  E.g., The libstdc++ pretty-printer has
    a pretty-printer for each of several different types, based on regexps.
    """

    # While one might want to push subprinters into the subclass, it's
    # present here to formalize such support to simplify
    # commands/pretty_printers.py.

    def __init__(self, name, subprinters=None):
        self.name = name
        self.subprinters = subprinters
        self.enabled = True

    def __call__(self, val):
        # The subclass must define this.
        raise NotImplementedError("PrettyPrinter __call__")


class SubPrettyPrinter(object):
    """Baseclass for sub-pretty-printers.

    Sub-pretty-printers needn't use this, but it formalizes what's needed.

    Attributes:
        name: The name of the subprinter.
        enabled: A boolean indicating if the subprinter is enabled.
    """

    def __init__(self, name):
        self.name = name
        self.enabled = True


def register_pretty_printer(obj, printer, replace=False):
    """Register pretty-printer PRINTER with OBJ.

    The printer is added to the front of the search list, thus one can override
    an existing printer if one needs to.  Use a different name when overriding
    an existing printer, otherwise an exception will be raised; multiple
    printers with the same name are disallowed.

    Arguments:
        obj: Either an objfile, progspace, or None (in which case the printer
            is registered globally).
        printer: Either a function of one argument (old way) or any object
            which has attributes: name, enabled, __call__.
        replace: If True replace any existing copy of the printer.
            Otherwise if the printer already exists raise an exception.

    Returns:
        Nothing.

    Raises:
        TypeError: A problem with the type of the printer.
        ValueError: The printer's name contains a semicolon ";".
        RuntimeError: A printer with the same name is already registered.

    If the caller wants the printer to be listable and disableable, it must
    follow the PrettyPrinter API.  This applies to the old way (functions) too.
    If printer is an object, __call__ is a method of two arguments:
    self, and the value to be pretty-printed.  See PrettyPrinter.
    """

    # Watch for both __name__ and name.
    # Functions get the former for free, but we don't want to use an
    # attribute named __foo__ for pretty-printers-as-objects.
    # If printer has both, we use `name'.
    if not hasattr(printer, "__name__") and not hasattr(printer, "name"):
        raise TypeError("printer missing attribute: name")
    if hasattr(printer, "name") and not hasattr(printer, "enabled"):
        raise TypeError("printer missing attribute: enabled") 
    if not hasattr(printer, "__call__"):
        raise TypeError("printer missing attribute: __call__")

    if obj is None:
        if gdb.parameter("verbose"):
            gdb.write("Registering global %s pretty-printer ...\n" % name)
        obj = gdb
    else:
        if gdb.parameter("verbose"):
            gdb.write("Registering %s pretty-printer for %s ...\n" %
                      (printer.name, obj.filename))

    if hasattr(printer, "name"):
        if not isinstance(printer.name, basestring):
            raise TypeError("printer name is not a string")
        # If printer provides a name, make sure it doesn't contain ";".
        # Semicolon is used by the info/enable/disable pretty-printer commands
        # to delimit subprinters.
        if printer.name.find(";") >= 0:
            raise ValueError("semicolon ';' in printer name")
        # Also make sure the name is unique.
        # Alas, we can't do the same for functions and __name__, they could
        # all have a canonical name like "lookup_function".
        # PERF: gdb records printers in a list, making this inefficient.
        i = 0
        for p in obj.pretty_printers:
            if hasattr(p, "name") and p.name == printer.name:
                if replace:
                    del obj.pretty_printers[i]
                    break
                else:
                  raise RuntimeError("pretty-printer already registered: %s" %
                                     printer.name)
            i = i + 1

    obj.pretty_printers.insert(0, printer)


class RegexpCollectionPrettyPrinter(PrettyPrinter):
    """Class for implementing a collection of regular-expression based pretty-printers.

    Intended usage:

    pretty_printer = RegexpCollectionPrettyPrinter("my_library")
    pretty_printer.add_printer("myclass1", "^myclass1$", MyClass1Printer)
    ...
    pretty_printer.add_printer("myclassN", "^myclassN$", MyClassNPrinter)
    register_pretty_printer(obj, pretty_printer)
    """

    class RegexpSubprinter(SubPrettyPrinter):
        def __init__(self, name, regexp, gen_printer):
            super(RegexpCollectionPrettyPrinter.RegexpSubprinter, self).__init__(name)
            self.regexp = regexp
            self.gen_printer = gen_printer
            self.compiled_re = re.compile(regexp)

    def __init__(self, name):
        super(RegexpCollectionPrettyPrinter, self).__init__(name, [])

    def add_printer(self, name, regexp, gen_printer):
        """Add a printer to the list.

        The printer is added to the end of the list.

        Arguments:
            name: The name of the subprinter.
            regexp: The regular expression, as a string.
            gen_printer: A function/method that given a value returns an
                object to pretty-print it.

        Returns:
            Nothing.
        """

        # NOTE: A previous version made the name of each printer the regexp.
        # That makes it awkward to pass to the enable/disable commands (it's
        # cumbersome to make a regexp of a regexp).  So now the name is a
        # separate parameter.

        self.subprinters.append(self.RegexpSubprinter(name, regexp,
                                                      gen_printer))

    def __call__(self, val):
        """Lookup the pretty-printer for the provided value."""

        # Get the type name.
        typename = gdb.types.get_basic_type(val.type).tag
        if not typename:
            return None

        # Iterate over table of type regexps to determine
        # if a printer is registered for that type.
        # Return an instantiation of the printer if found.
        for printer in self.subprinters:
            if printer.enabled and printer.compiled_re.search(typename):
                return printer.gen_printer(val)

        # Cannot find a pretty printer.  Return None.
        return None

# A helper class for printing enum types.  This class is instantiated
# with a list of enumerators to print a particular Value.
class _EnumInstance:
    def __init__(self, enumerators, val):
        self.enumerators = enumerators
        self.val = val

    def to_string(self):
        flag_list = []
        v = long(self.val)
        any_found = False
        for (e_name, e_value) in self.enumerators:
            if v & e_value != 0:
                flag_list.append(e_name)
                v = v & ~e_value
                any_found = True
        if not any_found or v != 0:
            # Leftover value.
            flag_list.append('<unknown: 0x%x>' % v)
        return "0x%x [%s]" % (self.val, " | ".join(flag_list))

class FlagEnumerationPrinter(PrettyPrinter):
    """A pretty-printer which can be used to print a flag-style enumeration.
    A flag-style enumeration is one where the enumerators are or'd
    together to create values.  The new printer will print these
    symbolically using '|' notation.  The printer must be registered
    manually.  This printer is most useful when an enum is flag-like,
    but has some overlap.  GDB's built-in printing will not handle
    this case, but this printer will attempt to."""

    def __init__(self, enum_type):
        super(FlagEnumerationPrinter, self).__init__(enum_type)
        self.initialized = False

    def __call__(self, val):
        if not self.initialized:
            self.initialized = True
            flags = gdb.lookup_type(self.name)
            self.enumerators = []
            for field in flags.fields():
                self.enumerators.append((field.name, field.enumval))
            # Sorting the enumerators by value usually does the right
            # thing.
            self.enumerators.sort(key = lambda x: x.enumval)

        if self.enabled:
            return _EnumInstance(self.enumerators, val)
        else:
            return None
