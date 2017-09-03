# DHT_Chord
### Date: Mar, 2015

Implmented a scalable, robust distributed systems Chord, using peer-to-peer and consistant-hashing ideas. 

Chord is a scalable P2P lookup protocol that can join, search and delete nodes in chord ring efficiently, and answer queries even if the system is continuously changing. I used consistent hash to keep the load balance and achieved Scalable Key location with O(logN) search cost.

This whole project is the implementation of The Chord project from MIT in C++. Here is the MIT project official address: <a>https://github.com/sit/dht/wiki.</a>

### Feature:
* Using hash to map keys to nodes in a linear space.
* Using the finger table so the search cost could reach O(logN).
