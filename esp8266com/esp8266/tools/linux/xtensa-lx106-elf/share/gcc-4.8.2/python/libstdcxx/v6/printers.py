# Pretty-printers for libstc++.

# Copyright (C) 2008-2013 Free Software Foundation, Inc.

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

import gdb
import itertools
import re

# Try to use the new-style pretty-printing if available.
_use_gdb_pp = True
try:
    import gdb.printing
except ImportError:
    _use_gdb_pp = False

# Try to install type-printers.
_use_type_printing = False
try:
    import gdb.types
    if hasattr(gdb.types, 'TypePrinter'):
        _use_type_printing = True
except ImportError:
    pass

# Starting with the type ORIG, search for the member type NAME.  This
# handles searching upward through superclasses.  This is needed to
# work around http://sourceware.org/bugzilla/show_bug.cgi?id=13615.
def find_type(orig, name):
    typ = orig.strip_typedefs()
    while True:
        search = str(typ) + '::' + name
        try:
            return gdb.lookup_type(search)
        except RuntimeError:
            pass
        # The type was not found, so try the superclass.  We only need
        # to check the first superclass, so we don't bother with
        # anything fancier here.
        field = typ.fields()[0]
        if not field.is_base_class:
            raise ValueError, "Cannot find type %s::%s" % (str(orig), name)
        typ = field.type

class SharedPointerPrinter:
    "Print a shared_ptr or weak_ptr"

    def __init__ (self, typename, val):
        self.typename = typename
        self.val = val

    def to_string (self):
        state = 'empty'
        refcounts = self.val['_M_refcount']['_M_pi']
        if refcounts != 0:
            usecount = refcounts['_M_use_count']
            weakcount = refcounts['_M_weak_count']
            if usecount == 0:
                state = 'expired, weak %d' % weakcount
            else:
                state = 'count %d, weak %d' % (usecount, weakcount - 1)
        return '%s (%s) %s' % (self.typename, state, self.val['_M_ptr'])

class UniquePointerPrinter:
    "Print a unique_ptr"

    def __init__ (self, typename, val):
        self.val = val

    def to_string (self):
        v = self.val['_M_t']['_M_head_impl']
        return ('std::unique_ptr<%s> containing %s' % (str(v.type.target()),
                                                       str(v)))

class StdListPrinter:
    "Print a std::list"

    class _iterator:
        def __init__(self, nodetype, head):
            self.nodetype = nodetype
            self.base = head['_M_next']
            self.head = head.address
            self.count = 0

        def __iter__(self):
            return self

        def next(self):
            if self.base == self.head:
                raise StopIteration
            elt = self.base.cast(self.nodetype).dereference()
            self.base = elt['_M_next']
            count = self.count
            self.count = self.count + 1
            return ('[%d]' % count, elt['_M_data'])

    def __init__(self, typename, val):
        self.typename = typename
        self.val = val

    def children(self):
        nodetype = find_type(self.val.type, '_Node')
        nodetype = nodetype.strip_typedefs().pointer()
        return self._iterator(nodetype, self.val['_M_impl']['_M_node'])

    def to_string(self):
        if self.val['_M_impl']['_M_node'].address == self.val['_M_impl']['_M_node']['_M_next']:
            return 'empty %s' % (self.typename)
        return '%s' % (self.typename)

class StdListIteratorPrinter:
    "Print std::list::iterator"

    def __init__(self, typename, val):
        self.val = val
        self.typename = typename

    def to_string(self):
        nodetype = find_type(self.val.type, '_Node')
        nodetype = nodetype.strip_typedefs().pointer()
        return self.val['_M_node'].cast(nodetype).dereference()['_M_data']

class StdSlistPrinter:
    "Print a __gnu_cxx::slist"

    class _iterator:
        def __init__(self, nodetype, head):
            self.nodetype = nodetype
            self.base = head['_M_head']['_M_next']
            self.count = 0

        def __iter__(self):
            return self

        def next(self):
            if self.base == 0:
                raise StopIteration
            elt = self.base.cast(self.nodetype).dereference()
            self.base = elt['_M_next']
            count = self.count
            self.count = self.count + 1
            return ('[%d]' % count, elt['_M_data'])

    def __init__(self, typename, val):
        self.val = val

    def children(self):
        nodetype = find_type(self.val.type, '_Node')
        nodetype = nodetype.strip_typedefs().pointer()
        return self._iterator(nodetype, self.val)

    def to_string(self):
        if self.val['_M_head']['_M_next'] == 0:
            return 'empty __gnu_cxx::slist'
        return '__gnu_cxx::slist'

class StdSlistIteratorPrinter:
    "Print __gnu_cxx::slist::iterator"

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        nodetype = find_type(self.val.type, '_Node')
        nodetype = nodetype.strip_typedefs().pointer()
        return self.val['_M_node'].cast(nodetype).dereference()['_M_data']

class StdVectorPrinter:
    "Print a std::vector"

    class _iterator:
        def __init__ (self, start, finish, bitvec):
            self.bitvec = bitvec
            if bitvec:
                self.item   = start['_M_p']
                self.so     = start['_M_offset']
                self.finish = finish['_M_p']
                self.fo     = finish['_M_offset']
                itype = self.item.dereference().type
                self.isize = 8 * itype.sizeof
            else:
                self.item = start
                self.finish = finish
            self.count = 0

        def __iter__(self):
            return self

        def next(self):
            count = self.count
            self.count = self.count + 1
            if self.bitvec:
                if self.item == self.finish and self.so >= self.fo:
                    raise StopIteration
                elt = self.item.dereference()
                if elt & (1 << self.so):
                    obit = 1
                else:
                    obit = 0
                self.so = self.so + 1
                if self.so >= self.isize:
                    self.item = self.item + 1
                    self.so = 0
                return ('[%d]' % count, obit)
            else:
                if self.item == self.finish:
                    raise StopIteration
                elt = self.item.dereference()
                self.item = self.item + 1
                return ('[%d]' % count, elt)

    def __init__(self, typename, val):
        self.typename = typename
        self.val = val
        self.is_bool = val.type.template_argument(0).code  == gdb.TYPE_CODE_BOOL

    def children(self):
        return self._iterator(self.val['_M_impl']['_M_start'],
                              self.val['_M_impl']['_M_finish'],
                              self.is_bool)

    def to_string(self):
        start = self.val['_M_impl']['_M_start']
        finish = self.val['_M_impl']['_M_finish']
        end = self.val['_M_impl']['_M_end_of_storage']
        if self.is_bool:
            start = self.val['_M_impl']['_M_start']['_M_p']
            so    = self.val['_M_impl']['_M_start']['_M_offset']
            finish = self.val['_M_impl']['_M_finish']['_M_p']
            fo     = self.val['_M_impl']['_M_finish']['_M_offset']
            itype = start.dereference().type
            bl = 8 * itype.sizeof
            length   = (bl - so) + bl * ((finish - start) - 1) + fo
            capacity = bl * (end - start)
            return ('%s<bool> of length %d, capacity %d'
                    % (self.typename, int (length), int (capacity)))
        else:
            return ('%s of length %d, capacity %d'
                    % (self.typename, int (finish - start), int (end - start)))

    def display_hint(self):
        return 'array'

class StdVectorIteratorPrinter:
    "Print std::vector::iterator"

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        return self.val['_M_current'].dereference()

class StdTuplePrinter:
    "Print a std::tuple"

    class _iterator:
        def __init__ (self, head):
            self.head = head

            # Set the base class as the initial head of the
            # tuple.
            nodes = self.head.type.fields ()
            if len (nodes) == 1:
                # Set the actual head to the first pair.
                self.head  = self.head.cast (nodes[0].type)
            elif len (nodes) != 0:
                raise ValueError, "Top of tuple tree does not consist of a single node."
            self.count = 0

        def __iter__ (self):
            return self

        def next (self):
            nodes = self.head.type.fields ()
            # Check for further recursions in the inheritance tree.
            if len (nodes) == 0:
                raise StopIteration
            # Check that this iteration has an expected structure.
            if len (nodes) != 2:
                raise ValueError, "Cannot parse more than 2 nodes in a tuple tree."

            # - Left node is the next recursion parent.
            # - Right node is the actual class contained in the tuple.

            # Process right node.
            impl = self.head.cast (nodes[1].type)

            # Process left node and set it as head.
            self.head  = self.head.cast (nodes[0].type)
            self.count = self.count + 1

            # Finally, check the implementation.  If it is
            # wrapped in _M_head_impl return that, otherwise return
            # the value "as is".
            fields = impl.type.fields ()
            if len (fields) < 1 or fields[0].name != "_M_head_impl":
                return ('[%d]' % self.count, impl)
            else:
                return ('[%d]' % self.count, impl['_M_head_impl'])

    def __init__ (self, typename, val):
        self.typename = typename
        self.val = val;

    def children (self):
        return self._iterator (self.val)

    def to_string (self):
        if len (self.val.type.fields ()) == 0:
            return 'empty %s' % (self.typename)
        return '%s containing' % (self.typename)

class StdStackOrQueuePrinter:
    "Print a std::stack or std::queue"

    def __init__ (self, typename, val):
        self.typename = typename
        self.visualizer = gdb.default_visualizer(val['c'])

    def children (self):
        return self.visualizer.children()

    def to_string (self):
        return '%s wrapping: %s' % (self.typename,
                                    self.visualizer.to_string())

    def display_hint (self):
        if hasattr (self.visualizer, 'display_hint'):
            return self.visualizer.display_hint ()
        return None

class RbtreeIterator:
    def __init__(self, rbtree):
        self.size = rbtree['_M_t']['_M_impl']['_M_node_count']
        self.node = rbtree['_M_t']['_M_impl']['_M_header']['_M_left']
        self.count = 0

    def __iter__(self):
        return self

    def __len__(self):
        return int (self.size)

    def next(self):
        if self.count == self.size:
            raise StopIteration
        result = self.node
        self.count = self.count + 1
        if self.count < self.size:
            # Compute the next node.
            node = self.node
            if node.dereference()['_M_right']:
                node = node.dereference()['_M_right']
                while node.dereference()['_M_left']:
                    node = node.dereference()['_M_left']
            else:
                parent = node.dereference()['_M_parent']
                while node == parent.dereference()['_M_right']:
                    node = parent
                    parent = parent.dereference()['_M_parent']
                if node.dereference()['_M_right'] != parent:
                    node = parent
            self.node = node
        return result

# This is a pretty printer for std::_Rb_tree_iterator (which is
# std::map::iterator), and has nothing to do with the RbtreeIterator
# class above.
class StdRbtreeIteratorPrinter:
    "Print std::map::iterator"

    def __init__ (self, typename, val):
        self.val = val

    def to_string (self):
        typename = str(self.val.type.strip_typedefs()) + '::_Link_type'
        nodetype = gdb.lookup_type(typename).strip_typedefs()
        return self.val.cast(nodetype).dereference()['_M_value_field']

class StdDebugIteratorPrinter:
    "Print a debug enabled version of an iterator"

    def __init__ (self, typename, val):
        self.val = val

    # Just strip away the encapsulating __gnu_debug::_Safe_iterator
    # and return the wrapped iterator value.
    def to_string (self):
        itype = self.val.type.template_argument(0)
        return self.val['_M_current'].cast(itype)

class StdMapPrinter:
    "Print a std::map or std::multimap"

    # Turn an RbtreeIterator into a pretty-print iterator.
    class _iter:
        def __init__(self, rbiter, type):
            self.rbiter = rbiter
            self.count = 0
            self.type = type

        def __iter__(self):
            return self

        def next(self):
            if self.count % 2 == 0:
                n = self.rbiter.next()
                n = n.cast(self.type).dereference()['_M_value_field']
                self.pair = n
                item = n['first']
            else:
                item = self.pair['second']
            result = ('[%d]' % self.count, item)
            self.count = self.count + 1
            return result

    def __init__ (self, typename, val):
        self.typename = typename
        self.val = val

    def to_string (self):
        return '%s with %d elements' % (self.typename,
                                        len (RbtreeIterator (self.val)))

    def children (self):
        rep_type = find_type(self.val.type, '_Rep_type')
        node = find_type(rep_type, '_Link_type')
        node = node.strip_typedefs()
        return self._iter (RbtreeIterator (self.val), node)

    def display_hint (self):
        return 'map'

class StdSetPrinter:
    "Print a std::set or std::multiset"

    # Turn an RbtreeIterator into a pretty-print iterator.
    class _iter:
        def __init__(self, rbiter, type):
            self.rbiter = rbiter
            self.count = 0
            self.type = type

        def __iter__(self):
            return self

        def next(self):
            item = self.rbiter.next()
            item = item.cast(self.type).dereference()['_M_value_field']
            # FIXME: this is weird ... what to do?
            # Maybe a 'set' display hint?
            result = ('[%d]' % self.count, item)
            self.count = self.count + 1
            return result

    def __init__ (self, typename, val):
        self.typename = typename
        self.val = val

    def to_string (self):
        return '%s with %d elements' % (self.typename,
                                        len (RbtreeIterator (self.val)))

    def children (self):
        rep_type = find_type(self.val.type, '_Rep_type')
        node = find_type(rep_type, '_Link_type')
        node = node.strip_typedefs()
        return self._iter (RbtreeIterator (self.val), node)

class StdBitsetPrinter:
    "Print a std::bitset"

    def __init__(self, typename, val):
        self.typename = typename
        self.val = val

    def to_string (self):
        # If template_argument handled values, we could print the
        # size.  Or we could use a regexp on the type.
        return '%s' % (self.typename)

    def children (self):
        words = self.val['_M_w']
        wtype = words.type

        # The _M_w member can be either an unsigned long, or an
        # array.  This depends on the template specialization used.
        # If it is a single long, convert to a single element list.
        if wtype.code == gdb.TYPE_CODE_ARRAY:
            tsize = wtype.target ().sizeof
        else:
            words = [words]
            tsize = wtype.sizeof 

        nwords = wtype.sizeof / tsize
        result = []
        byte = 0
        while byte < nwords:
            w = words[byte]
            bit = 0
            while w != 0:
                if (w & 1) != 0:
                    # Another spot where we could use 'set'?
                    result.append(('[%d]' % (byte * tsize * 8 + bit), 1))
                bit = bit + 1
                w = w >> 1
            byte = byte + 1
        return result

class StdDequePrinter:
    "Print a std::deque"

    class _iter:
        def __init__(self, node, start, end, last, buffer_size):
            self.node = node
            self.p = start
            self.end = end
            self.last = last
            self.buffer_size = buffer_size
            self.count = 0

        def __iter__(self):
            return self

        def next(self):
            if self.p == self.last:
                raise StopIteration

            result = ('[%d]' % self.count, self.p.dereference())
            self.count = self.count + 1

            # Advance the 'cur' pointer.
            self.p = self.p + 1
            if self.p == self.end:
                # If we got to the end of this bucket, move to the
                # next bucket.
                self.node = self.node + 1
                self.p = self.node[0]
                self.end = self.p + self.buffer_size

            return result

    def __init__(self, typename, val):
        self.typename = typename
        self.val = val
        self.elttype = val.type.template_argument(0)
        size = self.elttype.sizeof
        if size < 512:
            self.buffer_size = int (512 / size)
        else:
            self.buffer_size = 1

    def to_string(self):
        start = self.val['_M_impl']['_M_start']
        end = self.val['_M_impl']['_M_finish']

        delta_n = end['_M_node'] - start['_M_node'] - 1
        delta_s = start['_M_last'] - start['_M_cur']
        delta_e = end['_M_cur'] - end['_M_first']

        size = self.buffer_size * delta_n + delta_s + delta_e

        return '%s with %d elements' % (self.typename, long (size))

    def children(self):
        start = self.val['_M_impl']['_M_start']
        end = self.val['_M_impl']['_M_finish']
        return self._iter(start['_M_node'], start['_M_cur'], start['_M_last'],
                          end['_M_cur'], self.buffer_size)

    def display_hint (self):
        return 'array'

class StdDequeIteratorPrinter:
    "Print std::deque::iterator"

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        return self.val['_M_cur'].dereference()

class StdStringPrinter:
    "Print a std::basic_string of some kind"

    def __init__(self, typename, val):
        self.val = val

    def to_string(self):
        # Make sure &string works, too.
        type = self.val.type
        if type.code == gdb.TYPE_CODE_REF:
            type = type.target ()

        # Calculate the length of the string so that to_string returns
        # the string according to length, not according to first null
        # encountered.
        ptr = self.val ['_M_dataplus']['_M_p']
        realtype = type.unqualified ().strip_typedefs ()
        reptype = gdb.lookup_type (str (realtype) + '::_Rep').pointer ()
        header = ptr.cast(reptype) - 1
        len = header.dereference ()['_M_length']
        if hasattr(ptr, "lazy_string"):
            return ptr.lazy_string (length = len)
        return ptr.string (length = len)

    def display_hint (self):
        return 'string'

class Tr1HashtableIterator:
    def __init__ (self, hash):
        self.node = hash['_M_bbegin']['_M_node']['_M_nxt']
        self.node_type = find_type(hash.type, '__node_type').pointer()

    def __iter__ (self):
        return self

    def next (self):
        if self.node == 0:
            raise StopIteration
        node = self.node.cast(self.node_type)
        result = node.dereference()['_M_v']
        self.node = node.dereference()['_M_nxt']
        return result

class Tr1UnorderedSetPrinter:
    "Print a tr1::unordered_set"

    def __init__ (self, typename, val):
        self.typename = typename
        self.val = val

    def hashtable (self):
        if self.typename.startswith('std::tr1'):
            return self.val
        return self.val['_M_h']

    def to_string (self):
        return '%s with %d elements' % (self.typename, self.hashtable()['_M_element_count'])

    @staticmethod
    def format_count (i):
        return '[%d]' % i

    def children (self):
        counter = itertools.imap (self.format_count, itertools.count())
        return itertools.izip (counter, Tr1HashtableIterator (self.hashtable()))

class Tr1UnorderedMapPrinter:
    "Print a tr1::unordered_map"

    def __init__ (self, typename, val):
        self.typename = typename
        self.val = val

    def hashtable (self):
        if self.typename.startswith('std::tr1'):
            return self.val
        return self.val['_M_h']

    def to_string (self):
        return '%s with %d elements' % (self.typename, self.hashtable()['_M_element_count'])

    @staticmethod
    def flatten (list):
        for elt in list:
            for i in elt:
                yield i

    @staticmethod
    def format_one (elt):
        return (elt['first'], elt['second'])

    @staticmethod
    def format_count (i):
        return '[%d]' % i

    def children (self):
        counter = itertools.imap (self.format_count, itertools.count())
        # Map over the hash table and flatten the result.
        data = self.flatten (itertools.imap (self.format_one, Tr1HashtableIterator (self.hashtable())))
        # Zip the two iterators together.
        return itertools.izip (counter, data)

    def display_hint (self):
        return 'map'

class StdForwardListPrinter:
    "Print a std::forward_list"

    class _iterator:
        def __init__(self, nodetype, head):
            self.nodetype = nodetype
            self.base = head['_M_next']
            self.count = 0

        def __iter__(self):
            return self

        def next(self):
            if self.base == 0:
                raise StopIteration
            elt = self.base.cast(self.nodetype).dereference()
            self.base = elt['_M_next']
            count = self.count
            self.count = self.count + 1
            valptr = elt['_M_storage'].address
            valptr = valptr.cast(elt.type.template_argument(0).pointer())
            return ('[%d]' % count, valptr.dereference())

    def __init__(self, typename, val):
        self.val = val
        self.typename = typename

    def children(self):
        nodetype = find_type(self.val.type, '_Node')
        nodetype = nodetype.strip_typedefs().pointer()
        return self._iterator(nodetype, self.val['_M_impl']['_M_head'])

    def to_string(self):
        if self.val['_M_impl']['_M_head']['_M_next'] == 0:
            return 'empty %s' % (self.typename)
        return '%s' % (self.typename)


# A "regular expression" printer which conforms to the
# "SubPrettyPrinter" protocol from gdb.printing.
class RxPrinter(object):
    def __init__(self, name, function):
        super(RxPrinter, self).__init__()
        self.name = name
        self.function = function
        self.enabled = True

    def invoke(self, value):
        if not self.enabled:
            return None
        return self.function(self.name, value)

# A pretty-printer that conforms to the "PrettyPrinter" protocol from
# gdb.printing.  It can also be used directly as an old-style printer.
class Printer(object):
    def __init__(self, name):
        super(Printer, self).__init__()
        self.name = name
        self.subprinters = []
        self.lookup = {}
        self.enabled = True
        self.compiled_rx = re.compile('^([a-zA-Z0-9_:]+)<.*>$')

    def add(self, name, function):
        # A small sanity check.
        # FIXME
        if not self.compiled_rx.match(name + '<>'):
            raise ValueError, 'libstdc++ programming error: "%s" does not match' % name
        printer = RxPrinter(name, function)
        self.subprinters.append(printer)
        self.lookup[name] = printer

    # Add a name using _GLIBCXX_BEGIN_NAMESPACE_VERSION.
    def add_version(self, base, name, function):
        self.add(base + name, function)
        self.add(base + '__7::' + name, function)

    # Add a name using _GLIBCXX_BEGIN_NAMESPACE_CONTAINER.
    def add_container(self, base, name, function):
        self.add_version(base, name, function)
        self.add_version(base + '__cxx1998::', name, function)

    @staticmethod
    def get_basic_type(type):
        # If it points to a reference, get the reference.
        if type.code == gdb.TYPE_CODE_REF:
            type = type.target ()

        # Get the unqualified type, stripped of typedefs.
        type = type.unqualified ().strip_typedefs ()

        return type.tag

    def __call__(self, val):
        typename = self.get_basic_type(val.type)
        if not typename:
            return None

        # All the types we match are template types, so we can use a
        # dictionary.
        match = self.compiled_rx.match(typename)
        if not match:
            return None

        basename = match.group(1)
        if basename in self.lookup:
            return self.lookup[basename].invoke(val)

        # Cannot find a pretty printer.  Return None.
        return None

libstdcxx_printer = None

class FilteringTypePrinter(object):
    def __init__(self, match, name):
        self.match = match
        self.name = name
        self.enabled = True

    class _recognizer(object):
        def __init__(self, match, name):
            self.match = match
            self.name = name
            self.type_obj = None

        def recognize(self, type_obj):
            if type_obj.tag is None:
                return None

            if self.type_obj is None:
                if not self.match in type_obj.tag:
                    # Filter didn't match.
                    return None
                try:
                    self.type_obj = gdb.lookup_type(self.name).strip_typedefs()
                except:
                    pass
            if self.type_obj == type_obj:
                return self.name
            return None

    def instantiate(self):
        return self._recognizer(self.match, self.name)

def add_one_type_printer(obj, match, name):
    printer = FilteringTypePrinter(match, 'std::' + name)
    gdb.types.register_type_printer(obj, printer)

def register_type_printers(obj):
    global _use_type_printing

    if not _use_type_printing:
        return

    for pfx in ('', 'w'):
        add_one_type_printer(obj, 'basic_string', pfx + 'string')
        add_one_type_printer(obj, 'basic_ios', pfx + 'ios')
        add_one_type_printer(obj, 'basic_streambuf', pfx + 'streambuf')
        add_one_type_printer(obj, 'basic_istream', pfx + 'istream')
        add_one_type_printer(obj, 'basic_ostream', pfx + 'ostream')
        add_one_type_printer(obj, 'basic_iostream', pfx + 'iostream')
        add_one_type_printer(obj, 'basic_stringbuf', pfx + 'stringbuf')
        add_one_type_printer(obj, 'basic_istringstream',
                                 pfx + 'istringstream')
        add_one_type_printer(obj, 'basic_ostringstream',
                                 pfx + 'ostringstream')
        add_one_type_printer(obj, 'basic_stringstream',
                                 pfx + 'stringstream')
        add_one_type_printer(obj, 'basic_filebuf', pfx + 'filebuf')
        add_one_type_printer(obj, 'basic_ifstream', pfx + 'ifstream')
        add_one_type_printer(obj, 'basic_ofstream', pfx + 'ofstream')
        add_one_type_printer(obj, 'basic_fstream', pfx + 'fstream')
        add_one_type_printer(obj, 'basic_regex', pfx + 'regex')
        add_one_type_printer(obj, 'sub_match', pfx + 'csub_match')
        add_one_type_printer(obj, 'sub_match', pfx + 'ssub_match')
        add_one_type_printer(obj, 'match_results', pfx + 'cmatch')
        add_one_type_printer(obj, 'match_results', pfx + 'smatch')
        add_one_type_printer(obj, 'regex_iterator', pfx + 'cregex_iterator')
        add_one_type_printer(obj, 'regex_iterator', pfx + 'sregex_iterator')
        add_one_type_printer(obj, 'regex_token_iterator',
                                 pfx + 'cregex_token_iterator')
        add_one_type_printer(obj, 'regex_token_iterator',
                                 pfx + 'sregex_token_iterator')

    # Note that we can't have a printer for std::wstreampos, because
    # it shares the same underlying type as std::streampos.
    add_one_type_printer(obj, 'fpos', 'streampos')
    add_one_type_printer(obj, 'basic_string', 'u16string')
    add_one_type_printer(obj, 'basic_string', 'u32string')

    for dur in ('nanoseconds', 'microseconds', 'milliseconds',
                'seconds', 'minutes', 'hours'):
        add_one_type_printer(obj, 'duration', dur)

    add_one_type_printer(obj, 'linear_congruential_engine', 'minstd_rand0')
    add_one_type_printer(obj, 'linear_congruential_engine', 'minstd_rand')
    add_one_type_printer(obj, 'mersenne_twister_engine', 'mt19937')
    add_one_type_printer(obj, 'mersenne_twister_engine', 'mt19937_64')
    add_one_type_printer(obj, 'subtract_with_carry_engine', 'ranlux24_base')
    add_one_type_printer(obj, 'subtract_with_carry_engine', 'ranlux48_base')
    add_one_type_printer(obj, 'discard_block_engine', 'ranlux24')
    add_one_type_printer(obj, 'discard_block_engine', 'ranlux48')
    add_one_type_printer(obj, 'shuffle_order_engine', 'knuth_b')

def register_libstdcxx_printers (obj):
    "Register libstdc++ pretty-printers with objfile Obj."

    global _use_gdb_pp
    global libstdcxx_printer

    if _use_gdb_pp:
        gdb.printing.register_pretty_printer(obj, libstdcxx_printer)
    else:
        if obj is None:
            obj = gdb
        obj.pretty_printers.append(libstdcxx_printer)

    register_type_printers(obj)

def build_libstdcxx_dictionary ():
    global libstdcxx_printer

    libstdcxx_printer = Printer("libstdc++-v6")

    # For _GLIBCXX_BEGIN_NAMESPACE_VERSION.
    vers = '(__7::)?'
    # For _GLIBCXX_BEGIN_NAMESPACE_CONTAINER.
    container = '(__cxx1998::' + vers + ')?'

    # libstdc++ objects requiring pretty-printing.
    # In order from:
    # http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/a01847.html
    libstdcxx_printer.add_version('std::', 'basic_string', StdStringPrinter)
    libstdcxx_printer.add_container('std::', 'bitset', StdBitsetPrinter)
    libstdcxx_printer.add_container('std::', 'deque', StdDequePrinter)
    libstdcxx_printer.add_container('std::', 'list', StdListPrinter)
    libstdcxx_printer.add_container('std::', 'map', StdMapPrinter)
    libstdcxx_printer.add_container('std::', 'multimap', StdMapPrinter)
    libstdcxx_printer.add_container('std::', 'multiset', StdSetPrinter)
    libstdcxx_printer.add_version('std::', 'priority_queue',
                                  StdStackOrQueuePrinter)
    libstdcxx_printer.add_version('std::', 'queue', StdStackOrQueuePrinter)
    libstdcxx_printer.add_version('std::', 'tuple', StdTuplePrinter)
    libstdcxx_printer.add_container('std::', 'set', StdSetPrinter)
    libstdcxx_printer.add_version('std::', 'stack', StdStackOrQueuePrinter)
    libstdcxx_printer.add_version('std::', 'unique_ptr', UniquePointerPrinter)
    libstdcxx_printer.add_container('std::', 'vector', StdVectorPrinter)
    # vector<bool>

    # Printer registrations for classes compiled with -D_GLIBCXX_DEBUG.
    libstdcxx_printer.add('std::__debug::bitset', StdBitsetPrinter)
    libstdcxx_printer.add('std::__debug::deque', StdDequePrinter)
    libstdcxx_printer.add('std::__debug::list', StdListPrinter)
    libstdcxx_printer.add('std::__debug::map', StdMapPrinter)
    libstdcxx_printer.add('std::__debug::multimap', StdMapPrinter)
    libstdcxx_printer.add('std::__debug::multiset', StdSetPrinter)
    libstdcxx_printer.add('std::__debug::priority_queue',
                          StdStackOrQueuePrinter)
    libstdcxx_printer.add('std::__debug::queue', StdStackOrQueuePrinter)
    libstdcxx_printer.add('std::__debug::set', StdSetPrinter)
    libstdcxx_printer.add('std::__debug::stack', StdStackOrQueuePrinter)
    libstdcxx_printer.add('std::__debug::unique_ptr', UniquePointerPrinter)
    libstdcxx_printer.add('std::__debug::vector', StdVectorPrinter)

    # These are the TR1 and C++0x printers.
    # For array - the default GDB pretty-printer seems reasonable.
    libstdcxx_printer.add_version('std::', 'shared_ptr', SharedPointerPrinter)
    libstdcxx_printer.add_version('std::', 'weak_ptr', SharedPointerPrinter)
    libstdcxx_printer.add_container('std::', 'unordered_map',
                                    Tr1UnorderedMapPrinter)
    libstdcxx_printer.add_container('std::', 'unordered_set',
                                    Tr1UnorderedSetPrinter)
    libstdcxx_printer.add_container('std::', 'unordered_multimap',
                                    Tr1UnorderedMapPrinter)
    libstdcxx_printer.add_container('std::', 'unordered_multiset',
                                    Tr1UnorderedSetPrinter)
    libstdcxx_printer.add_container('std::', 'forward_list',
                                    StdForwardListPrinter)

    libstdcxx_printer.add_version('std::tr1::', 'shared_ptr', SharedPointerPrinter)
    libstdcxx_printer.add_version('std::tr1::', 'weak_ptr', SharedPointerPrinter)
    libstdcxx_printer.add_version('std::tr1::', 'unordered_map',
                                  Tr1UnorderedMapPrinter)
    libstdcxx_printer.add_version('std::tr1::', 'unordered_set',
                                  Tr1UnorderedSetPrinter)
    libstdcxx_printer.add_version('std::tr1::', 'unordered_multimap',
                                  Tr1UnorderedMapPrinter)
    libstdcxx_printer.add_version('std::tr1::', 'unordered_multiset',
                                  Tr1UnorderedSetPrinter)

    # These are the C++0x printer registrations for -D_GLIBCXX_DEBUG cases.
    # The tr1 namespace printers do not seem to have any debug
    # equivalents, so do no register them.
    libstdcxx_printer.add('std::__debug::unordered_map',
                          Tr1UnorderedMapPrinter)
    libstdcxx_printer.add('std::__debug::unordered_set',
                          Tr1UnorderedSetPrinter)
    libstdcxx_printer.add('std::__debug::unordered_multimap',
                          Tr1UnorderedMapPrinter)
    libstdcxx_printer.add('std::__debug::unordered_multiset',
                          Tr1UnorderedSetPrinter)
    libstdcxx_printer.add('std::__debug::forward_list',
                          StdForwardListPrinter)


    # Extensions.
    libstdcxx_printer.add_version('__gnu_cxx::', 'slist', StdSlistPrinter)

    if True:
        # These shouldn't be necessary, if GDB "print *i" worked.
        # But it often doesn't, so here they are.
        libstdcxx_printer.add_container('std::', '_List_iterator',
                                        StdListIteratorPrinter)
        libstdcxx_printer.add_container('std::', '_List_const_iterator',
                                        StdListIteratorPrinter)
        libstdcxx_printer.add_version('std::', '_Rb_tree_iterator',
                                      StdRbtreeIteratorPrinter)
        libstdcxx_printer.add_version('std::', '_Rb_tree_const_iterator',
                                      StdRbtreeIteratorPrinter)
        libstdcxx_printer.add_container('std::', '_Deque_iterator',
                                        StdDequeIteratorPrinter)
        libstdcxx_printer.add_container('std::', '_Deque_const_iterator',
                                        StdDequeIteratorPrinter)
        libstdcxx_printer.add_version('__gnu_cxx::', '__normal_iterator',
                                      StdVectorIteratorPrinter)
        libstdcxx_printer.add_version('__gnu_cxx::', '_Slist_iterator',
                                      StdSlistIteratorPrinter)

        # Debug (compiled with -D_GLIBCXX_DEBUG) printer
        # registrations.  The Rb_tree debug iterator when unwrapped
        # from the encapsulating __gnu_debug::_Safe_iterator does not
        # have the __norm namespace. Just use the existing printer
        # registration for that.
        libstdcxx_printer.add('__gnu_debug::_Safe_iterator',
                              StdDebugIteratorPrinter)
        libstdcxx_printer.add('std::__norm::_List_iterator',
                              StdListIteratorPrinter)
        libstdcxx_printer.add('std::__norm::_List_const_iterator',
                              StdListIteratorPrinter)
        libstdcxx_printer.add('std::__norm::_Deque_const_iterator',
                              StdDequeIteratorPrinter)
        libstdcxx_printer.add('std::__norm::_Deque_iterator',
                              StdDequeIteratorPrinter)

build_libstdcxx_dictionary ()
