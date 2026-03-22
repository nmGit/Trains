================
World View Layer
================

The bottom layer is the world view layer, which is solely a view of the game world.
It displays the terrain, cities, and other world features *without* any user interface elements.

Navigation
==========

The user may pan and zoom to explore the world. The world view layer is responsible for rendering the 
world at different zoom levels, showing more or less detail as appropriate.

Pan
---
``w``, ``a``, ``s``, ``d``, ``up``, ``left``, ``down``, and ``right`` arrow keys can be 
used to pan the view up, left, down, and right respectively. 

Zoom
----
The mouse scroll wheel or touchpad gestures can be used to zoom in and out. Show more detail when zoomed in, and less detail when zoomed out.

.. doxygenclass:: Trains::WorldView
    :project: TrainGame