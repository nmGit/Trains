==========
Code Style
==========

Conventions
-----------

Events
^^^^^^
Events represent something that has happened in the system.

The if you have an event that you'd like to subscribe to, then the producer will have an ``OnEvent()`` method that produces that event.

.. mermaid::

    flowchart LR
        Event --> OnEvent 

        
.. mermaid::

    flowchart LR
        subgraph Examples
            direction LR
            Resize --> OnResize
            Click --> OnClick
        end
