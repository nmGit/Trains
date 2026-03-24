==========================
Game Architecture Overview
==========================

.. contents::


.. mermaid::

    flowchart LR

    subgraph Graphics
        direction TB

        Camera -- "attached to" --> Canvas
        CityView -- "Draws On"--> Canvas
        WorldView -- "owns" --> Camera
        WorldView --"Draws On"--> Canvas

        UI
    end

    subgraph Simulation
        direction TB

        subgraph CityPlanner
            direction TB
            World --> Regions
            Regions --> Cities
        end

        Core("Game Core") --> CityPlanner

    end

    vm("View Model") <--"Is Presented By"--> Graphics
    vm <--> Simulation
    UI --"Renders On"--> Canvas

Rendering Pipeline
==================

Each frame follows a three-phase pipeline managed by the Canvas:

.. mermaid::

    sequenceDiagram
        participant Main as Main Loop
        participant Canvas as Canvas
        participant Camera as Camera
        participant WV as WorldView

        Main->>Canvas: Service()
        Note over Canvas: Poll SDL events
        Canvas->>Camera: Update()
        Canvas->>Camera: ApplyTransform(ctx)
        Canvas-->>Main: Continue

        Main->>WV: Service()
        Note over WV: Draw hex grid and cities in world coordinates

        Main->>Canvas: Present()
        Note over Canvas: Restore transform and flip to screen

1. **Service()** -- Canvas polls SDL events and, if a camera is attached,
   updates camera state and applies its world-to-screen transform.
2. **Draw** -- WorldView (and other layers) draw in world coordinates.
   ``Canvas::GetViewBounds()`` provides the visible rect for culling.
3. **Present()** -- Canvas restores the context and copies pixels to screen.

View Model
==========

.. doxygenclass:: Trains::ViewModel


WorldView
=========

.. doxygenclass:: Trains::WorldView

