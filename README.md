<!--
SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>

SPDX-License-Identifier: LGPL-3.0-or-later
-->

Provides a framework for hierarchic widgets based on SDL2. Most of the event handling and drawing is however abstracted away and should be easily replacable. The original purpose of this library is to supersede my quick and dirty *immediate GUI* implementation in my [touchscreen GUI](https://github.com/muesli4/mpd-touch-screen-gui) and at the same time provide GUI support for a remote control.

## Installation

Run `autoreconf -vfi` to generate the configure script. Then build as usual with `./configure`, `make`, and `make install`.

## Features

* Clean and easy to use (mostly declarative) GUI building with modern C++.
* Simple widget hierarchy with useful base classes. New widgets can be created by deriving.
* Flexible widget containers: (h/v)box and table.
* Support for widget selection. In addition selectable widgets can be navigated in 2 dimensions. The intention was to support faster and more intuitive keyboard or infra-red remote navigation.
* Optional dirty-based drawing: Only dirty widgets will be redrawn.

## Documentation

I uploaded an early snapshot of the API [here](https://muesli4.github.io/annotated.html).

## Current Shortcomings

* Only a basic dark hard-coded theme is provided.
* Missing widget to enter text.
* Keyboard events are not handled by widgets.
* Widgets do not get a new size when their content changes. There are several desirable behaviors how this could work.
* Containers do not have interfaces to add or remove children. This would also require redoing the allocation.
* The drawing primitives are lacking (should probably be refactored into low-level and design-based drawing).
* The font renderer is optimized on reoccuring words. Rendering arbitrary text with it might use a lot of memory (although it will trade off with rerendering).
* Backgrounds can not easily redrawn if a child is dirty (although no widgets need that yet).
* Widgets can't unselect themselves or others (bad if a selection gets covered).

## Examples
![widget test min](/examples/widget_test_min.png)
