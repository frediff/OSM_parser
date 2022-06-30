# OSM_parser
<p align="justify"><i>
<a href="https://www.openstreetmap.org">Openstreetmap</a> is a free and open resource which provides high quality map data for all areas in the world. They provide the data in the OSM format which can be downloaded from their website. OSM is an XML-based format, which has various elements and the attributes for those elements. The purpose of this project is to develop a software which reads data about various artefacts on the map, and provide a text searchable interface to the map. </i></p>
<p align="justify">
Two particular elements of the OSM format, are of interest.
<ul>
  <li>
    <strong>node</strong>: This represents a particular place on the map, e.g. a shop, building, etc. It has an id, latitude, and longitude. Additionally, some node elements have a “name” attribute which describes the place.
  </li>
  <li> 
  <strong>way</strong>: This represents a path or a line through the map, and is denoted by a sequence of node elements. This element also has an id, and optionally a name describing it.
  </li>
</ul>
</p>

### Use case 1:
> This involves parsing a given OSM file and extracting the node and way elements from the file along with their attributes and then printing the total number nodes and ways discovered in the given file. It allows a user to search for a particular element by matching input string with sub-string of the name.

### Use case 2:
> This involves finding the k-closest nodes to a given node using the crow fly distance. Here k is a user input. The latitude and longitude information of nodes is used to calculate the distance. An algorithm is followed that uses heaps and runs in ```O(N + K * LOG(K))``` time.

### Use case 3:
> This involves calculating the shortest path between two node elements, through the way elements. The distance on a way is the sum of distances between consecutive node elements in the way. Also the path is printed. An efficient version of Djikstra algorithm is followed that runs very fast.
