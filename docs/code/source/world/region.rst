.. _region_description:

======
Region
======

A region is a large area that contains multiple cities or other geographical feature.

Each region's cities and land features are generated based on the region's parameters.

Generation
==========

City Growth
-----------

A city starts out occupying a single tile and grows based on the following algorithm:

Phase 1: Calculate Growth Probability
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Clear all tile occupation candidates for the region.

.. code-block:: pseudo

   for each city in the region:
      for each tile adjacent to the city:
         p = CalculateP()
         if(L.contains(tile) == false):
            L.append(p, tile, city)
         else if (p > L.getP(tile)):
            L.update(p, tile, city)
      end inner loop.
   end outer loop.


The ``CalculateP()`` function is defined as follows:

.. math::
   \begin{align}
      p = p' + b_n \cdot n + b_a \cdot A \\
      \text{where} \\
      p &:= \text{probability of growth into tile} \space T \\
      n &:= \text{Number of adjacent tiles occupied by the city} \\
      b_n &:= \text{Neighbor bias} \\
      b_a &:= \text{Area bias} \\
      A &:= \text{Area} \\
      p' &:= \text{Base growth probability} \\
   \end{align}

Phase 2: Occupy Tiles
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: pseudo

   for each tile in L:
      if(L.getP(tile) > random(0, 1)):
         tile.occupy(L.getCity(tile))
      end if.
   end loop.

.. doxygenclass:: CityPlanner::Region
   :project: TrainGame

Region Generation
-----------------

A region does not change size over time, but the cities within it may.
Regions are psuedorandomly generated, seeded by the tile coordinates of their 'seed tile'.
Regions and are generated when the level starts within the fixed world size.

