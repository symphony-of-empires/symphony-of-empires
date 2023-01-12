--- GRAPH

A GlobalGraph is an undirected, weighted graph representation of the world trade network
A GlobalGraph is made up of RegionNodes & ConnectionEdges. Nodes cannot be added or removed, but edges can be added through major events (e.g. Suez canal).

Each RegionNode is an abstraction of a region, it contains:
    an integer ID
    a set of province IDs, the factories and pops in those province IDs are a part of the region, 
        (in the future the ownerTAG of all the provinces should be the owner of the region)
        not every region needs a set of provinces, we do not need to keep track of a RegionNode in the middle of the ocean that doesn't produce or consume anything 
    latitude and longitude coordinates of the region (centroid?)
    a set of region IDs that denote a connection between RegionNodes

Each ConnectionEdge is an abstraction of a connection between regions, it contains:
    a pair of region IDs that denote the endpoints of the connection
    a interger that represents connection type (0=ocean, 1=river, 2=canal, 3=land_unimproved, 4=railroad, etc...)
    a distance in km
    a cost value 
        the cost value is a function of ( distance , connection type ). e.g. how much money does it take to move 1 metric ton via connection_type C over distance D
        the cost value is the "weight" of the edge
        the capacity of an edge is (amortized?) by cost so we do not need to worry about it :)

--- COST MATRIX

Using a shortest paths algorithm (Dijkstra's, Bellman-Ford, Johnson's, etc...) and a GlobalGraph G a Matrix (node x node) can be constructed and periodically updated:
    this matrix stores the cost of the best path from n1 to n2, e.g. M[n1][n2] = some_cost

Why is this important?
    The main focus is to determine which countries have been blessed by geography and which ones haven't. 
    Let's take one example, for the sake of simplicity there are only 3 important regions in the world.
        -USA, Bolivia, and Saudi Arabia are the only regions. Only Saudi Arabia produces oil and only USA and Bolivia consume oil.
        -A Global price is a good starting point, but we also need to take into account the cost to transport oil from point A to B 
        -USAs cost for oil is lower because its path is entirely over the ocean and shorter
        -Bolivia's cost for oil is higher, it is a landlocked nation and its path is also longer
        -So the USA should focus on factories that require lots of oil, and Bolivia should not. The USA has a natural advantadge.
        -Bolivia can try to improve its path by updating edge connectionts that it can build infrastructure (railroads) on
        -But Bolivia will still probably have to pay more for oil
        -If it wants to build factories that require oil, the government can expect costly subsidizes
    The primary role of the graph and the matrix is to determine prices around the world rather than a simple (but very unrealistic) global price for a good.

-- SIMULATION MANAGER

Simulation of Global Economy
Every economy tick, perform some operations on the matrix or graph

--- TO DO

Fix general sloppy code!!!

A Graph can easily be constructed manually, but for a world of thousands of provinces this is tedious
    a way to make it procedurally or based on a graph configuration / manifest file is required

A Graph structure is an intuitive way to create blockades and embargoes. This can easily be done BUT it could be terrible for performance.
    testing needs to be done to see if this is the case

LOTS OF TESTING

-- TESTING

(non-parallel)
current vg ms for week
100: 65
200: 268
300: 622
400: 1000
500: 1700
600: 2596
700: 3248
800:
900:
1000: 6894
