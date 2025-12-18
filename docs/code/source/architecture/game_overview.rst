==========================
Game Architecture Overview
==========================

.. contents::


.. mermaid::

    flowchart LR

    subgraph Graphics
        direction TB
        
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

View Model
==========

.. doxygenclass:: Trains::ViewModel
    

WorldView
=========

.. doxygenclass:: Trains::WorldView

