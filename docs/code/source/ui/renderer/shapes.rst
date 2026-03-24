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

View Frustum Culling
====================

``Shape::Draw()`` accepts a ``bounds_t`` parameter describing the visible
world-space rectangle (computed from the :doc:`Camera <camera>` or defaulting
to the full image rect when no camera is attached).

Shapes may use this bounds rectangle to skip geometry that lies entirely outside
the visible area.  The base implementation ignores the parameter, so shapes that
do not yet implement culling continue to work without changes.

.. mermaid::

    flowchart LR
        Canvas -- "GetViewBounds()" --> bounds["bounds_t"]
        bounds --> Draw["Shape::Draw(ctx, view_bounds)"]
        Draw --> Cull{"Intersects<br/>view?"}
        Cull -- "Yes" --> Render["Draw geometry"]
        Cull -- "No" --> Skip["Skip"]

Shapes that currently implement culling:

- **HexGrid** -- rebuilds its path each frame to include only hexagons within
  the visible column/row range.
- **CityView** -- tests each tile's bounding circle against the view bounds
  before drawing.

.. doxygenclass:: Drafter::Shape
    :project: TrainGame
