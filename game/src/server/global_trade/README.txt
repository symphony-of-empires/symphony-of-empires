Global Trade V2

An algorithm for doing path calculations on a boost graph quickly. 
It can currently compute near-optimal cost calculations for a 'realistic procedural' graph of 1000 nodes with an average tick speed of ~0.3s
An entire yearly simulation can run in ~120s (2min)

It uses a combination of A* and Dijkstras and it requires designating some vertices as hubs H and a mapping from {vertex i, set{h1, h2...}}

Todo
Integrate it with the rest of the economy

