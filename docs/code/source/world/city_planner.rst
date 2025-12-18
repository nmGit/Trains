============
City Planner
============

.. toctree::
   :maxdepth: 2
   :caption: Game Entities
   
   World <world.rst>
   Regions <region.rst>
   Cities <cities.rst>


The world generator is responsible for creating the game world, including regions and cities.
It is part of the game engine's simulation that runs throughout a level. It generates cities 
and regions as the game progresses.

Cities, regions, and the world itself are more than just data containers; they are also responsible 
for advancing the simulation by updating their state over time.

.. mermaid::

    graph TD
        WGen[World Generator] --> W[World]
        WGen --> R[Region]
        WGen --> C[City]