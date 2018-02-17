'''
    Nicholas Serrano EB1
    Maxwell McEvoy
    binary_heap.py
    used for heaps in server.py
'''
class BinaryHeap:
    """
    An implementation of a binary heap using a list.
    """

    def __init__(self):
        """
        Initialize an empty heap.
        """

        self.nodes = list()

    def __len__(self):
        """
        Returns the number of items in the heap.
        """

        return len(self.nodes)

    def min(self):
        """
        Returns the minimum-key item and its key as a pair,
        e.g. ("dog", 1)
        """

        if len(self.nodes) == 0:
            raise IndexError("getting min from an empty heap")

        # min key item is always at the top of the heap
        return self.nodes[0]

    def _lchild(self, index):
        """
        Returns the index of the left child.
        No bounds checking.
        """
        return 2*index+1

    def _rchild(self, index):
        """
        Returns the index of the right child.
        No bounds checking.
        """
        return 2*index+2

    def _parent(self, index):
        """
        Returns the index of the parent.
        No bounds checking.
        """
        return (index-1)//2 # use integer division

    def insert(self, item, key):
        """
        Inserts the item with the given key.

        >>> heap = BinaryHeap()
        >>> heap.insert("cat", 4)
        >>> heap.insert("dog", 1)
        >>> heap.insert("pig", 2)
        >>> heap.min() == ("dog", 1)
        True
        >>> heap.insert("bear", 0)
        >>> heap.min() == ("bear", 0)
        True
        """

        index = len(self.nodes) # invariant: will be the index of the new item
        self.nodes.append((item, key))

        while index > 0:
            parent = self._parent(index) # get parent index
            if self.nodes[parent][1] > key:
                self.nodes[parent], self.nodes[index] = \
                  self.nodes[index], self.nodes[parent] # multiline expression
            index = parent



    def popmin(self):
        """
        Pop and return the item/key pair with minimum key.
        Assumes the heap is not empty.

        Returns the pair as a tuple.

        >>> heap = BinaryHeap()
        >>> heap.insert('A', 4)
        >>> heap.insert('B', 5)
        >>> heap.insert('C', 3)
        >>> heap.popmin()
        ('C', 3)
        >>> heap.popmin()
        ('A', 4)
        >>> heap.insert('D', 1)
        >>> heap.popmin()
        ('D', 1)
        >>> heap.popmin()
        ('B', 5)
        >>> len(heap)
        0
        """

        min_item = self.min()

        if len(self) == 1:
            # it would still work without this, but
            # let's add it for good style
            self.nodes.pop()
            return min_item

        # move the last item to the root
        self.nodes[0] = self.nodes[-1]
        self.nodes.pop()

        index = 0

        while True:
            lc, rc = self._lchild(index), self._rchild(index)
            if lc >= len(self):
                return min_item

            # get the index of the child with minimum key
            if rc >= len(self) or self.nodes[lc][1] <= self.nodes[rc][1]:
                min_child = lc
            else:
                min_child = rc

            # check if there really is a heap property violation
            # if not, quit
            if self.nodes[index][1] <= self.nodes[min_child][1]:
                return min_item

            # swap the current vertex with the min_child vertex
            self.nodes[index], self.nodes[min_child] = \
              self.nodes[min_child], self.nodes[index]

            index = min_child

def heapsort(items):
    """
    Returns the sorted list of items.

    >>> heapsort([5,4,2,1,2,3])
    [1, 2, 2, 3, 4, 5]
    >>> heapsort([1])
    [1]
    >>> heapsort([])
    []
    """

    heap = BinaryHeap()
    for x in items:
        heap.insert(None, x)
    sorted = []
    while heap:
        sorted.append(heap.popmin()[1])
    return sorted

