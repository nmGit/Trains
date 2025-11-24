======
Shapes
======

Shapes are the building blocks of the UI. They represent drawable (or hidden) elements that can be composed to create interfaces. 
Shapes have a simple set of properties, such as position and size. Beyond that, shapes can contain other shapes in which case they act as containers.

Each shape has exactly one parent shape, except for the Drafter::Canvas which is the root shape and has no parent.

.. mermaid::

    flowchart RL
    subgraph Canvas
        subgraph Container 1
            a[Shape 1]
            b[Shape 2]
        end
        subgraph Container 2
            c[Shape 3]
            subgraph Container 3
                d[Shape 4]
            end
        end
    end


.. doxygenclass:: Drafter::Shape
    :project: TrainGame
