======
Camera
======

The ``Drafter::Camera`` provides pan and zoom for a :doc:`Canvas <canvas>`.
When attached, the camera subscribes to the canvas's SDL event stream and
translates user input into a world-space transform that the canvas applies
automatically each frame.

If no camera is attached to a canvas, the canvas draws with the identity
transform -- world coordinates map 1:1 to screen pixels with the origin at the
top-left corner.

Ownership
=========

The camera is **created by the caller** and attached to a canvas via
``Camera::Attach(canvas)``.  The canvas does not own the camera; it holds a
raw pointer that is cleared on ``Camera::Detach()`` or ``Canvas::SetCamera(nullptr)``.

.. mermaid::

    flowchart LR
        Caller["Caller (e.g. WorldView)"]
        Camera["Drafter::Camera"]
        Canvas["Drafter::Canvas"]

        Caller -- "owns" --> Camera
        Caller -- "calls Attach()" --> Camera
        Camera -- "subscribes to SDL events" --> Canvas
        Camera -- "SetCamera(this)" --> Canvas
        Canvas -- "calls Update() / ApplyTransform()" --> Camera

Frame Lifecycle
===============

With a camera attached, the per-frame rendering flow is:

.. mermaid::

    sequenceDiagram
        participant Main as Main Loop
        participant Canvas as Canvas
        participant Camera as Camera
        participant Shapes as Shapes / WorldView

        Main->>Canvas: Service()
        Canvas->>Canvas: Poll SDL events
        Canvas->>Camera: Update()
        Note over Camera: Advance pan velocity,<br/>interpolate zoom
        Canvas->>Canvas: ctx.save()
        Canvas->>Camera: ApplyTransform(ctx, geo)
        Note over Camera: translate(center) · scale(zoom) · translate(-cam)
        Canvas-->>Main: Continue / Quit

        Main->>Shapes: Draw / Service(canvas.GetViewBounds())
        Note over Shapes: All drawing happens<br/>in world coordinates

        Main->>Canvas: Present()
        Canvas->>Canvas: ctx.restore()
        Canvas->>Canvas: Flip pixels to screen

Pan
===

Pan is driven by two independent input sources that both update the camera
position in world space:

**Keyboard** -- WASD / arrow keys apply an acceleration vector each frame.
Velocity accumulates up to a maximum speed and decays via friction when no key
is held.  Diagonal input is normalized so it does not exceed the acceleration
magnitude.  Velocity is divided by the current zoom so the screen-space feel
remains consistent regardless of zoom level.

**Mouse drag** -- Clicking and dragging directly translates the camera.  The
mouse delta (in screen pixels) is divided by zoom to convert to world units.

Zoom
====

Zoom is driven by the mouse scroll wheel or touchpad pinch gesture.  SDL3
reports ``event.wheel.y`` as a float: discrete mice produce +/-1, while
touchpads produce smooth fractional values that track gesture velocity.

The camera accumulates scroll deltas into a **log-space target**
(``m_log_zoom_target``).  Each frame, the actual log-zoom interpolates toward
the target:

.. code-block:: text

    m_log_zoom += (m_log_zoom_target - m_log_zoom) * k_zoom_smoothing
    m_zoom = exp(m_log_zoom)
    m_zoom = clamp(m_zoom, k_zoom_min, k_zoom_max)

Working in log-space means equal scroll deltas produce equal *perceptual* zoom
steps (e.g., each click doubles the zoom rather than adding a fixed increment).
The interpolation provides smooth animation regardless of input device.

View Bounds
===========

``Camera::GetViewBounds(geo)`` returns the world-space rectangle currently
visible on screen.  This is the inverse of the camera transform applied to the
screen rectangle:

.. code-block:: text

    half_w = (screen_w / 2) / zoom
    half_h = (screen_h / 2) / zoom
    view_bounds = { cam_x - half_w, cam_y - half_h, half_w * 2, half_h * 2 }

Canvas exposes this via ``Canvas::GetViewBounds()``, which delegates to the
camera when one is attached or returns the full image rect otherwise.  Shapes
use this bounds rectangle for :doc:`view frustum culling <shapes>`.

.. doxygenclass:: Drafter::Camera
   :project: TrainGame
