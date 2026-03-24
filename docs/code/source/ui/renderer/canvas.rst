======
Canvas
======

The canvas is the root rendering surface.  It owns the SDL window, the
``BLImage`` pixel buffer, and the ``BLContext`` rendering session.  All shapes
ultimately draw into the canvas's context.

Camera Integration
==================

A :doc:`Camera <camera>` can be attached to the canvas via
``Canvas::SetCamera(camera)``.  When a camera is present, the canvas
automatically applies the camera transform at the start of each frame and
restores it before flipping to screen.  This means all shape drawing happens in
**world coordinates** without shapes needing to know about the camera.

When no camera is attached, the canvas uses the identity transform -- world
coordinates equal screen pixels with the origin at the top-left.

Frame Lifecycle
===============

The canvas splits its per-frame work into two calls:

``Service()``
    Polls SDL events, emits them to subscribers, and -- if a camera is attached --
    calls ``Camera::Update()`` and applies the camera transform.  Returns
    ``Quit`` if the window was closed.

``Present()``
    Restores the rendering context (undoing the camera transform) and copies the
    rendered image to the screen.

The caller draws between these two calls:

.. code-block:: cpp

    while (canvas.Service() == Canvas::ServiceResult::Continue) {
        worldview.Service();   // draws in world space
        canvas.Present();      // flip to screen
    }

View Bounds
===========

``Canvas::GetViewBounds()`` returns the visible area as a ``bounds_t``:

- With a camera: delegates to ``Camera::GetViewBounds()``, returning the
  world-space rectangle on screen.
- Without a camera: returns ``{0, 0, pixel_width, pixel_height}``.

Shapes receive this bounds rectangle and can use it for view frustum culling.

.. doxygenclass:: Drafter::Canvas
   :project: TrainGame
