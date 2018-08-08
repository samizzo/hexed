hexed - a Windows console-based hex editor

[![Build status](https://ci.appveyor.com/api/projects/status/1mjm8674l9ardj0t?svg=true)](https://ci.appveyor.com/project/izzo/hexed)

## Keyboard shortcuts
* Navigation: **Left**, **Right**, **Up**, **Down**

  vi-style keybings (**i, j, k, l**) can also be used in view and byte editing modes.

  **Home/End** to jump to the _start_/_end_ of current row, or the entire file if in combination with **Ctrl**
  
  **PageUp**/**PageDown** advances or rewinds the view of the file a page at a time, or if in combination with **Ctrl**, the _first_/_last_ row of the currently displayed page. Column alignment is preserved
* Redraw display: **F5**
* Enter/Exit edit mode: **Insert/Escape**, goes into _byte_ (Hex) editing mode by default
* Switch between edit modes: **Tab**, currently two modes are supported, _byte_ (Hex) and _char_ (ASCII)
* Quit: **Escape**, when not in edit mode

## Sample session
![hexed.gif](/hexed.gif)
