================
Widget Lifecycle
================

.. mermaid::
    :caption: Widget Lifecycle

    stateDiagram
        Created: Widget Created
        [*] --> Created
        Created --> Prelude
        Prelude --> Active

        state Active {
            service_children: Service Children
            service_children --> Service
            Service --> service_children
        }

        Active --> Postlude
        Postlude --> [*]


Creation
========
Widgets are create by calling the constructor.

Prelude
=======
The prelude phase occurs before the widget becomes 
active, allowing the widget to set itself up before 
it starts receiving service calls. 

All child's prelude() methods are called before the
parent's prelude() is called.

Service
=======
The parent of a widget calls the service method of 
its children. 

Postlude
========
The postlude function is called right before the widget is
destroyed. This allows the widget to clean up after itself.