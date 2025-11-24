==========================
Game Architecture Overview
==========================

.. contents::


.. mermaid::

    flowchart TD
    subgraph Graphics
        World --"Is Draw By"--> WorldView
        WorldView --"Draws On"--> Canvas

        UI
    end

    subgraph Simulation
        Core("Game Core")
        Core --"Modifies"--> World
    end

    UI --"Provides Input To"--> Core
    UI --"Renders On"--> Canvas

WorldView
=========

The WorldView draws the state of the game onto the Drafter::Canvase. It listens to all events emitted by the game and updates the visual representation of the world accordingly.
It does not own the canvas, it only draws on it.

