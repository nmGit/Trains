================
World View Layer
================

The bottom layer is the world view layer, which is solely a view of the game world.
It displays the terrain, cities, and other world features *without* any user interface elements.

Navigation
==========

The user may pan and zoom to explore the world. Navigation is handled by a
:doc:`Drafter::Camera <renderer/camera>` that WorldView creates and attaches to the canvas
at startup.  Because the camera lives in the Drafter library and manages its
own input handling, WorldView contains no camera state or input code -- it only
draws world content in world coordinates.

.. mermaid::

    flowchart TD
        WorldView -- "owns" --> Camera["Drafter::Camera"]
        WorldView -- "draws on" --> Canvas["Drafter::Canvas"]
        Camera -- "attached to" --> Canvas
        Canvas -- "applies transform from" --> Camera
        Canvas -- "forwards SDL events to" --> Camera

Pan
---
``w``, ``a``, ``s``, ``d``, ``up``, ``left``, ``down``, and ``right`` arrow keys can be
used to pan the view up, left, down, and right respectively. Keyboard pan uses acceleration
and friction for smooth movement.

The map can also be clicked and dragged to pan the view.

Zoom
----
The mouse scroll wheel or touchpad gestures can be used to zoom in and out. Zoom is
interpolated in log-space so it responds smoothly to both discrete scroll clicks and
continuous touchpad gestures. Show more detail when zoomed in, and less detail when zoomed out.

View Frustum Culling
====================

Each frame, WorldView reads the visible world-space rectangle from
``Canvas::GetViewBounds()`` and passes it to all shapes.  Shapes use this to
skip geometry that falls outside the view:

- **HexGrid** rebuilds its path each frame, including only hexagons whose
  column and row fall within the visible range.
- **CityView** skips individual tiles whose bounding circle does not intersect
  the view bounds.

This ensures rendering cost scales with the number of *visible* tiles rather
than the total world size.

.. doxygenclass:: Trains::WorldView
    :project: TrainGame
