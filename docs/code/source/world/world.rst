=====
World
=====

The world is made up of one or more regions, which each contain one or more cities.

.. mermaid::
   
    graph TD
        W[World] --> R1[Region 1]
        W[World] --> R2[Region 2]
        R1[Region 1] --> C1[City 1]
        R1 --> C2[City 2]
        R2[Region 2] --> C3[City 3]
        R2 --> C4[City 4]

.. doxygenclass:: CityPlanner::World
   :project: TrainGame