==============
User Interface
==============

.. toctree::
   :maxdepth: 2
   :caption: User Interface

   World View <world_view_layer.rst>
   Interaction <interaction_layer.rst>

The user interface is immediate mode. The UI is redrawn every frame, no UI elements have state. 
They all take a canvas, draw on the canvas when instantiated, and can be destroyed immediately after.