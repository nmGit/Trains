======
Cities
======

Summary
=======

A city is an entity that exists within a region. 

- It grows (or shrinks) over time. 
- It produces and consumes various resources

State and Behavior
------------------

A city occupies a tiles within a region. The number of tiles that a city occupies affects both
its resource production and consumption. 

A city with more resources can grow faster, while a city starved of resources will shrink.

Growth
~~~~~~

A city grows based on the following algorithm:

.. math::
   \begin{align}
      p = p' + b \cdot n \\
      \text{where} \\
      p &:= \text{probability of growth into tile} \space T \\
      n &:= \text{Number of adjacent tiles occupied by the city} \\
      b &:= \text{Neighbor bias} \\
      p' &:= \text{Base growth probability} \\
   \end{align}



Role of the Region
------------------

:ref:`region_description`

All cities exist within a region. The properties of a region can also affect properties 
of the cities within.

Growth Within Region
~~~~~~~~~~~~~~~~~~~~

A city's growth is constrained in part by the region's borders. In other words, a city cannot grow 
beyond the border of the region that it is in.

The cities grow phase is handled within the region's business logic.


.. doxygenclass:: CityPlanner::City
   :members:
   :undoc-members:
   :allow-dot-graphs: