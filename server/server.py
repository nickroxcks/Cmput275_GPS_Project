"""
    Nicholas Serrano EB1
    Maxwell McEvoy EB1
    Cmput 275 Assignment 1 part 1 server.py

    This program will load the Edmonton graph and
    take in a user input, and output wavepoints. Also
    includes function least_cost_path which can be used
    to find the shortest path between to vertices

    Note: if you are importing this file elsewhere,
    this program will not wait for user input. It will
    instead only load the map data in the edmonton text
    file and you can then test functions in your script.
"""
from graph import Graph
from binary_heap import BinaryHeap
from serial import Serial
from time import sleep
import math


def load_edmonton_graph(filename):
    """
        Loads the graph of Edmonton from the given file.

        Returns two items
            graph: the instance of the class Graph() corresponding to the
                directed graph from edmonton-roads-2.0.1.txt
            location: a dictionary mapping the identifier of a vertex to
                the pair (lat, lon) of geographic coordinates for that vertex.
                These should be integers measuring the lat/lon in 100000-ths
                of a degree.

        In particular, the return statement in your code should be
        return graph, location
        (or whatever name you use for the variables).

        Note: the vertex identifiers should be converted to integers
        before being added to the graph and the dictionary.

        >>> edmonton,location = load_edmonton_graph("edmonton-roads-2.0.1.txt")
        >>> print(location['1393031188'])
        (5350811, -11344289)

    """
    edmonton = Graph()
    location = {}
    infile = open(filename, "r")
    lines = infile.readlines()
    for l in lines:
        words = l.split(",")
        if words[0] == "V":
            edmonton.add_vertex(words[1])
            location[words[1]] = (int(float(words[2]) * 100000),
                                  int(float(words[3]) * 100000))
        if words[0] == "E":
            edmonton.add_edge([words[1], words[2]])
    return edmonton, location


class CostDistance:
    """
        A class with a method called distance that will return the Euclidean
        between two given vertices.
    """

    def __init__(self, location):
        """
            Creates an instance of the CostDistance class and stores the
            dictionary "location" as a member of this class.
        """
        self.alist = {}
        self.l = location

    def distance(self, e):
        """
            Here e is a pair (u,v) of vertices.
            Returns the Euclidean distance between the two vertices u and v.

            >>> location = {1:(10,12),3:(12,14),5:(12,15),7:(14,15)}
            >>> cost = CostDistance(location)
            >>> print(cost.distance((1,7))) # sqrt((10 - 14)^2 + (12 - 15)^2)=5
            5.0
            >>> print(cost.distance((5,3))) # sqrt((12 - 12)^2 + (15 - 11)^2)=1
            1.0
        """

        location = self.l
        x, y = e[0], e[1]
        x_diff = math.pow(location[x][0] - location[y][0], 2)
        y_diff = math.pow(location[x][1] - location[y][1], 2)
        euclidean = math.sqrt(x_diff + y_diff)

        return euclidean


def least_cost_path(graph, start, dest, cost):
    """
        Find and return a least cost path in graph from start
        vertex to dest vertex.

        Efficiency: If E is the number of edges, the run-time is
        O( E log(E) ).

        Args:
            graph (Graph): The digraph defining the edges between the
            vertices.
            start: The vertex where the path starts. It is assumed
                that start is a vertex of graph.
            dest:  The vertex where the path ends. It is assumed
                that dest is a vertex of graph.
            cost:  A class with a method called "distance" that takes
                as input an edge (a pair of vertices) and returns the cost
                of the edge. For more details, see the CostDistance class
                description below.

        Returns:
            least: A potentially empty list (if no path can be found) of
            the vertices in the graph. If there was a path, the first
            vertex is always start, the last is always dest in the list.
            Any two consecutive vertices correspond to some
            edge in graph.

    >>> g = Graph({1, 2, 3, 4, 5}, [(1, 4), (2, 3), (4, 2)])
    >>> location = {1: (1, 0), 2: (4, 0), 3: (10, 0), 4: (2, 0), 5: (10, 10)}
    >>> cost = CostDistance(location)
    >>> print(least_cost_path(g, 1, 2, cost))
    [1, 4, 2]
    >>> print(least_cost_path(g, 1, 3, cost))
    [1, 4, 2, 3]
    >>> print(least_cost_path(g, 1, 5, cost))
    []
    >>> H = Graph({1, 2, 3, 4, 5, 6}, [(1, 2), (2, 3), (2, 4), (4, 3), (3, 5), (1, 6), (6, 5)])
    >>> locationtwo = {1: (0, 0), 2: (1, 1), 3: (3, 1), 4: (2, 2), 5: (0, 6), 6: (-100, 100)}
    >>> testcost = CostDistance(locationtwo)
    >>> print(least_cost_path(H, 1, 5, testcost))
    [1, 2, 3, 5]
    """

    reached = {}
    events = BinaryHeap()
    events.insert((start, start), 0)

    while len(events) > 0:
        (u, v), time = events.popmin()

        if v not in reached.keys():
            reached[v] = u
            for w in graph.neighbours(v):
                events.insert((v, w), time + cost.distance((v, w)))
    '''
        reached is a dictionary of all visted verices. The value of each key is
        represented by the previous vertice visited to get to that vertice.
        If we cannot find our destination in reached, then that means there is
        no such path in the given graph to get from start to dest, and so we
        would return an empty array. otherwise, we make our list least by
        working in reverse from our dest, all the way back to start by using
        the keys in reached to find the
        previous vertice
    '''
    if dest not in reached:
        return []


    least = [dest]
    step = dest

    while step != start:
        least.append(reached[step])
        step = reached[step]

    least.reverse()
    return least


def nearest_vertices(location, lat_long):
    """
        This will take our raw input start and stop locations and
        return a vertices that is closests to that those values

        Input:
            location is a dictionary with all vertices as key and lat long
            as values

            lat_long is (lat,long) raw input coordinates

        Output:
            return a vertex that is close to our raw input ex '232323432'

        >>> edmonton,location = load_edmonton_graph("edmonton-roads-2.0.1.txt")
        >>> cost = CostDistance(location)
        >>> print(nearest_vertices(location, (5365486, -11333915)))
        314088878
        >>> print(nearest_vertices(location, (5365488, -11333914)))
        314088878

    """
    if lat_long in list(location.values()):
        return list({k: v for k, v in location.items()
                     if v == lat_long}.keys())[0]
    check = float("inf")
    ver = (0, 0)
    for v in location:
        xsum = math.fabs(int(location[v][0]) - int(lat_long[0]))
        ysum = math.fabs(int(location[v][1]) - int(lat_long[1]))
        if check > (xsum + ysum):
            check = (xsum + ysum)
            ver = v
    return ver


def talk_in():
    '''
        receive data from arduino

    '''
    with Serial("/dev/tty.usbmodemFA131", baudrate=9600, timeout=5) as ser:
        iteration = 0
        while True:
            # infinite loop that echoes all messages from
            # the arduino to the terminal
            line = ser.readline()
            # print("I read byte string:", line)

            if not line:
                print("timeout, restarting...")
                continue

            line_string = line.decode("ASCII")
            # print("This is the actual string:", line_string)
            # print("Stripping off the newline and carriage return")
            stripped = line_string.rstrip("\r\n")
            print("I read line: ", stripped)

            # print(len(line_string), len(stripped))

            # construct the line you want to print to the
            # Arduino, don't forget the newline
            out_line = "Iteration " + str(iteration) + "\n"
            iteration += 1

            encoded = out_line.encode("ASCII")
            # now encoded is a byte object we can
            # write to the arduino

            ser.write(encoded)

            # rest a bit between rounds of communication
            sleep(2)

    return received


edmonton, location = load_edmonton_graph("edmonton-roads-2.0.1.txt")


# if running this specific script, await user command
if __name__ == "__main__":
    import doctest
    doctest.testmod()
    cost = CostDistance(location)
    # received = input().split()
    received = talk_in()
    v1 = nearest_vertices(location, (received[1], received[2]))
    v2 = nearest_vertices(location, (received[3], received[4]))

    path = least_cost_path(edmonton, v1, v2, cost)

    print("N", len(path))
    for i in path:

        print("W", location[i][0], location[i][1])
    if len(path) > 0:
        print('E')
