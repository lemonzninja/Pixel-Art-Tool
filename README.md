Technical Architecture Plan for a Pixel Drawing Program
Overview of the Pixel Art Editor

We are building a lightweight pixel art editor (similar in spirit to Aseprite) using C and the raylib framework. Raylib is a cross-platform, open-source graphics library that handles window creation, input, and drawing primitives. It includes all necessary dependencies internally and supports multiple platforms (Windows, Linux, etc.) out-of-the-box

. This makes it an ideal choice for our program’s foundation. The application will allow users to draw on a pixel-based canvas with basic tools and manage colors, then save or load their artwork as PNG images. In this plan, we outline the key components of the program’s architecture and how they work together to fulfill the required features.

Core Application Structure and Main Loop

At the heart of the program is a main loop that runs until the user closes the window. We will initialize the window using InitWindow() (specifying a default width, height, and title) and set an appropriate target frame rate (e.g. 60 FPS) for smooth input handling. Each iteration of the loop will consist of the following phases, using raylib’s functions:

Input Handling: Poll for user input (mouse, keyboard) using raylib’s input functions (e.g. IsMouseButtonDown(), IsKeyPressed())

. This will update the program state (current tool, selected color, canvas changes, etc.) based on user actions.

Updating State: Apply any changes to the canvas data (for example, drawing a pixel if the mouse is dragging with the pencil tool, or erasing if using the eraser). Also handle UI state changes (like switching tools or selecting a new color from the palette).

Rendering: Clear the screen and draw the latest state of the canvas and UI. We will call BeginDrawing(), then draw the canvas (scaled according to zoom level) and the palette/tool UI, and end with EndDrawing() to display the new frame. The drawing is done with raylib’s drawing APIs (for example, DrawTextureEx for the canvas texture and DrawRectangle for palette swatches).

This structure follows the typical game loop pattern. By keeping the input, update, and render steps separate, the design remains organized and easy to maintain. The use of raylib ensures that platform-specific details (windowing, OpenGL context, input polling) are abstracted away

, so the same code runs on both Windows and Linux with minimal changes.

Canvas Representation and Drawing Mechanism

Canvas Data: We represent the drawing canvas as a 2D grid of pixels, stored in memory. A convenient approach is to use raylib’s Image structure, which holds pixel data on the CPU in an RGBA format

. We can allocate an Image for the canvas (for example, using GenImageColor(width, height, color) to create a blank canvas filled with a default background color). Each pixel’s color is a Color struct (holding r, g, b, a channels). Using an Image gives us access to raylib’s image manipulation functions for pixel-level operations.

Drawing Pixels: To modify the canvas (e.g. when the user draws), we will use functions like ImageDrawPixel(&canvasImage, x, y, drawColor) to set an individual pixel in the Image

. Raylib provides a suite of similar functions (ImageDrawLine, ImageDrawRectangle, etc.) that operate on the Image in CPU memory

. This makes it straightforward to implement drawing tools.

Canvas Texture: While the Image holds the authoritative pixel data, we use a GPU texture (Texture2D) for efficient rendering to the window. After creating the canvasImage, we load it into a texture once via LoadTextureFromImage(canvasImage)

. Each time the canvas Image is updated (pixels drawn or erased), we call UpdateTexture(canvasTexture, canvasImage.data) to push the updated pixel data to the GPU

. This way, the canvas on screen reflects the Image’s current pixels. Using a texture for rendering is important for performance – it lets the GPU handle scaling and drawing in a single call, rather than drawing thousands of individual pixel rectangles on the CPU.

Rendering the Canvas: We will draw the canvas texture to the screen at the desired position and scale. For example, using DrawTextureEx(canvasTexture, (Vector2){offsetX, offsetY}, 0.0f, scaleFactor, WHITE) will draw the texture at a given offset with a uniform scale

. The scaleFactor corresponds to the “pixel size” or zoom level (an integer scale like 1x, 2x, 5x, etc.), allowing the pixel art to be magnified. To maintain a crisp pixelated look when scaling up, we will disable texture filtering on the canvas texture. Raylib allows setting nearest-neighbor sampling via SetTextureFilter(canvasTexture, TEXTURE_FILTER_POINT), which ensures no blurry interpolation (each pixel is enlarged as a sharp block)
. By default, raylib’s textures might use bilinear filtering when scaled, so this step is crucial for an authentic pixel-art appearance.

Double-Buffering Consideration: Raylib’s framework already handles double buffering in BeginDrawing()/EndDrawing(), so we don’t need to manage our own framebuffers for basic drawing. The canvas texture drawing will be done between those calls. If we wanted to allow very large canvases or more advanced effects, we could consider using a RenderTexture2D (an off-screen render target) to draw portions of the canvas, but for our base program the simpler approach above is sufficient.

Drawing Tools: Pencil and Eraser

We will implement two primary tools for editing pixels: a Pencil and an Eraser. The program will track the current tool selection (for example, an enum or simple boolean flag for pencil vs eraser) and the current drawing color (for the pencil). The basic behavior of these tools is:

Pencil Tool: Draws with the currently selected color. When this tool is active, any mouse click or drag on the canvas will set those pixel positions to the selected Color value.

Eraser Tool: Erases content by drawing with a special “background” color. Erasing can be implemented in one of two ways, depending on how we handle transparency:

If the canvas supports transparency (alpha channel), the eraser can set the pixel’s alpha to 0 (making it fully transparent). This effectively removes the pixel’s color, revealing the background (which could be shown as a checkerboard or a solid background in the UI).

If we choose not to deal with transparency in the first version, we can define a background color (say white or another preset) and use the eraser to reset pixels to that color, simulating an erase. In that case, the canvas might be entirely opaque, and the eraser is just painting with the background color.
Either approach is valid for a basic implementation; supporting true transparency would be more in line with how Aseprite’s eraser works (making pixels transparent), but it requires minor additional handling in the UI (for example, showing a checkerboard pattern behind transparent pixels).

Input Handling for Tools: We will use raylib’s mouse input functions to apply the tools. For example, if the user is holding down the left mouse button (IsMouseButtonDown(MOUSE_LEFT_BUTTON) each frame) and the cursor is over the canvas area, we will draw or erase at the corresponding canvas coordinates. The program will continuously draw as the mouse moves, allowing smooth freehand lines. We might also use IsMouseButtonPressed for single-click events if needed (though for drawing, continuous application on drag is key). Optionally, one mouse button could draw and another could erase without switching tools – e.g. left-click for pencil, right-click for eraser – which is a convenient shortcut. In our architecture, we can choose one of two schemes for tool usage:

Mode Selection: The user explicitly selects “pencil” or “eraser” (via a UI button or a keyboard shortcut like pressing P or E). Then left-clicking will perform that tool’s action. This matches how many drawing programs toggle tools.

Two-Button Simultaneous: Assign left-click to draw (with current color) and right-click to erase, so both tools are always available. This is similar to some pixel editors where primary/secondary mouse buttons correspond to two colors (often foreground and background color). For simplicity, we might use this approach initially (left = draw, right = erase), as it avoids needing an extra UI control for toggling tools.

Internally, applying a tool means calculating which pixel in the canvas corresponds to the mouse position and setting its color. We will translate screen coordinates to canvas coordinates by accounting for the canvas offset and scale. For example, canvasX = (mouseX - canvasOffsetX) / scaleFactor (and similarly for Y) gives the target pixel indices. We’ll clamp these to the canvas bounds and then use ImageDrawPixel on the canvas Image. Drawing a single pixel per input event is fine for slow mouse movement; however, if the user drags very fast, there could be gaps. To handle that, we could implement a simple line drawing between the last and current mouse positions (using ImageDrawLine for example)
, ensuring no gaps if the mouse skips pixels. This is a minor enhancement that can make freehand drawing smoother.

After updating the Image with the pencil or eraser, we update the texture via UpdateTexture() as described earlier. This could be done every frame during a drag for simplicity. Since raylib’s UpdateTexture() will re-upload the whole image data, doing it for each pixel drawn is somewhat inefficient. However, for a reasonably-sized canvas (e.g. 64×64 or 256×256) this is not a big issue. If performance becomes a concern for larger canvases, we could batch updates (only update once after a full stroke, or update smaller sub-rectangles using UpdateTextureRec

). In the initial design, simplicity is prioritized over micro-optimization, given the relatively low pixel counts involved.

Color Palette Management

The program will include a simple color palette displayed on the UI, from which the user can select the active drawing color. We will maintain an array or list of Color values representing the palette. For example, a palette might have 16 predefined colors (which could be hard-coded or loaded from a config or palette file in the future). Each color will be shown as a small square swatch on the window.

Palette UI Layout: We need to decide where and how the palette is drawn. Two common approaches are:

A horizontal bar of color swatches at the top or bottom of the window.

A vertical panel of swatches on one side (left or right) of the window.

For this base program, we can use a horizontal bar at the bottom for simplicity (since the canvas width may be more flexible than height). Each color swatch can be, for example, a 20×20 or 30×30 pixel square drawn with DrawRectangle() using the palette color

. We will also draw a border around the currently selected color to highlight it (e.g., a white or black outline rectangle). The coordinates for these swatches will be in a fixed region outside the canvas area, so we can easily detect when the user clicks there without interfering with drawing on the canvas.

Color Selection: When a mouse click is detected in the palette area, we determine which swatch was clicked by its position. For instance, if the palette is a row at the bottom, we can divide the click X coordinate by the swatch width to get the index. Then we set the current drawing color to the palette’s color at that index. This will affect the Pencil tool’s color. We might also allow right-click on a color to set an “eraser color” or secondary color if we extend functionality, but initially a single current color is enough.

If needed, we can populate the palette with a basic set of colors (primary colors, black/white, etc.) or even a well-known palette for pixel art (like a PICO-8 or NES color palette, which are commonly used 8-bit style palettes). Since the palette is simple, we’ll not integrate a complex color picker UI at first – users will be limited to the provided colors. Later on, we could add a color picker dialog or allow customizing the palette, but it’s beyond the MVP scope.

Palette Data Structure: In code, this could be an array like Color palette[16] initialized with Color{R,G,B,A} values. Raylib has predefined Colors (e.g., RED, GREEN, BLUE, etc.), which we could use for convenience, or define our own. The palette and the current color index would be part of the program’s state.

One consideration: if we support loading an arbitrary image, that image might contain colors outside the preset palette. Our simple palette might not automatically include those. For now, the palette is just for the user to pick drawing colors; loading an image won’t alter the palette. The user could still draw on a loaded image with the nearest palette colors or we might allow picking a color from the image (eyedropper tool) as a later feature. Architecture-wise, this is a reminder that the color palette is separate from the canvas pixel data – the canvas can hold any color (since each pixel is stored as full 32-bit color), and the palette is just a UI convenience for choosing drawing colors.

Canvas Size Adjustments and Pixel Scaling

Our editor should support an adjustable canvas size. This means the user can start a new canvas with different dimensions (width and height), and also adjust the zoom level (display pixel size). These features involve a bit of careful handling to update related resources and layout:

Changing Canvas Dimensions: We will likely provide a way to create a new canvas of a user-specified size (for example, through a menu option or a key like N for "New"). When this happens, the program should:

Prompt or determine the new width and height (this could be hardcoded options initially, like toggling between a few preset sizes for simplicity).

Free or unload the current canvas Image and texture (UnloadImage() and UnloadTexture() in raylib) to avoid memory leaks

Create a fresh Image with the new dimensions (using GenImageColor or ImageCreate if available, filled with a background color or transparent) and a new Texture from it (LoadTextureFromImage()).

Reset relevant state, such as current tool (default to pencil perhaps), and possibly clear the undo history if we had one (not in this base plan).

Update any rendering offsets if the window size or UI layout depends on canvas size. For instance, if the window is fixed size and the new canvas is smaller, we might center it; if larger, we might need to enable scrolling or scale it down. As a first step, we might constrain the maximum canvas size to the window size divided by the minimum zoom (to avoid dealing with scrolling). Alternatively, we can simply allow the window itself to resize accordingly (on desktop this could be acceptable, or we make the window resizable and let the OS handle scrolling – though in raylib typically the window is a fixed size unless we implement camera panning).

For now, we assume the canvas will generally fit in the window at 1x zoom. If the user chooses a huge canvas, we could automatically set a smaller zoom or just document that large canvases might not be fully visible. A more advanced solution would be implementing panning (holding spacebar to drag the view, etc.), but that’s beyond the initial requirements.

Pixel Scale (Zoom): The pixel size refers to how large each canvas pixel is displayed. We manage this with a scale factor (integer). A scale of 1 means each canvas pixel is 1 screen pixel (no zoom, potentially very small if the canvas is low-res). A scale of 8 would mean each canvas pixel is drawn as an 8×8 block of screen pixels (zoomed in 8x). The program should allow increasing or decreasing this scale. We can map this to user input, for example:

Press + or - keys (or perhaps the  mouse wheel) to zoom in and out.

Clamp the scale to a reasonable range (maybe 1 to 20, to avoid going too far).

Ensure that changing the scale doesn’t change the actual image data, only how it’s rendered. The scaleFactor will be used in DrawTextureEx or equivalent to draw the texture larger or smaller.

When the scale changes, we should also reposition the palette or other UI elements if needed, so they don’t overlap the canvas. If the palette is at the bottom and we zoom the canvas, making it taller, it might start to cover the palette. A simple solution is to always reserve a portion of the window for UI. For example, if the window height is H, we could dedicate a fixed 50px at the bottom for the palette, and draw the canvas in the area above that (scaling it to fit if necessary). Alternatively, place the palette to the side where an increase in canvas height won’t interfere with it. These are UI design choices – for now we assume a sufficiently sized window that can accommodate the maximum expected canvas size at max zoom plus the palette area.

After adjusting the scale, the drawing logic uses the new scale for coordinate transforms. E.g., if scale was 4 and now it’s 8, a given canvas pixel will occupy twice the screen area, and our conversion from mouse coordinate to canvas coordinate becomes (mouseX - offsetX) / 8 (instead of /4). We will update this calculation dynamically based on the current scaleFactor variable. Raylib provides GetMouseX() and GetMouseY() to get the cursor position
raylib.com
, which we can use for these calculations each frame.

Additionally, as mentioned earlier, we make sure the rendered texture uses point filtering so that scaling by non-1 factors doesn’t blur. The single call SetTextureFilter(canvasTexture, TEXTURE_FILTER_POINT) after creating the texture is sufficient
nes.is-a.dev
. Once set, the texture will preserve hard edges at any magnification.

Saving and Loading PNG Images

One of the essential features is the ability to save the canvas drawing to a PNG file and load an existing image (PNG) into the canvas. We will leverage raylib’s built-in image I/O functionality for this, which relies on the library’s integration of stb_image / stb_image_write or its own rpng module for PNG support
. This avoids the need for external dependencies like libpng. The plan for implementing save/load is as follows:

Saving (Export): Raylib provides an ExportImage(Image image, const char *fileName) function that writes an Image to file (format inferred from the filename extension, e.g. “.png”)
raylib.com
. We will call this when the user triggers a save (perhaps by pressing S or choosing a menu option). For the base version, we can hard-code a filename (like "drawing.png") or have a simple mechanism to get a filename. To keep things simple initially, using a default name or overwriting a previous file is acceptable. We’ll ensure to print a message or log that the file was saved (for user feedback). On Windows and Linux, writing to the working directory is typically fine. (If using an IDE, we note that the working directory might differ, but that’s an external detail.)

When saving, we will pass our current canvas Image to ExportImage. If our canvas contains transparency (alpha), the PNG will retain that (as a PNG alpha channel). If not, the PNG will be fully opaque. Raylib’s export handles both cases. One consideration: if the user never saved before and closes the program, changes are lost. In future, we might prompt to save on exit, but for now it’s manual.

Loading (Import): Raylib’s LoadImage(const char *fileName) can load image files (PNG, BMP, JPG, etc.) from disk into an Image struct
raylib.com
. We will implement loading (e.g. triggered by pressing L or via a menu) to allow the user to open an existing PNG file as their canvas. The steps for loading are:

(Optional) Prompt for a filename or use a default path. In a minimal approach, we might assume a file name (like "import.png") or allow the user to drag-and-drop a file onto the window (raylib does support file drop events via IsFileDropped() and LoadDroppedFiles()).

Use LoadImage() to read the image file. This returns a new Image in memory. We should check if the returned Image is valid (raylib might return a zero-size image if loading failed, which we can detect via width/height).

If valid, unload the current canvas Image/Texture and replace them with the new image and a new texture via LoadTextureFromImage.

If the loaded image dimensions differ from our previous canvas, update any related state (canvas width/height variables). The UI may also need adjustment if the window was sized for the old canvas – for now, we might simply allow the window to resize or not worry if parts go off-screen. Ideally, we could resize the window to fit the new image if it’s small, or scale it down if it’s too large. As a first pass, it’s okay to assume the images loaded are not enormous. We will, however, apply the same point filtering to the new texture to keep it pixelated when zoomed.

Loading an image with transparency: if the PNG has transparent pixels, raylib’s Image will preserve those (the Color. a < 255 for those pixels). When we display it, by default our window’s background is a solid color (say white or gray via ClearBackground()). Transparent parts of the image will show that background. This is fine, but the user might not realize those areas are transparent. In a more advanced UI we’d draw a checkerboard behind the canvas texture to indicate transparency. We can note that as a potential improvement. For now, the user will see transparent areas as just the window clear color.

File Dialogs and User Input: The current plan avoids dealing with graphical file dialogs. On Windows and Linux, implementing a file picker is non-trivial in pure C without a library. We could integrate a small cross-platform dialog library (e.g. tinyfiledialogs or use the OS native calls) if needed. However, since this is a base program, we might simply use fixed file paths or require the user to type a filename in the console (if the program is run from a console, we could read stdin after a prompt). Another approach is to use raylib’s drop-file support: the user can drag a PNG onto the window to load it. Raylib’s function LoadDroppedFiles() provides dropped filenames. This is quite user-friendly on desktop platforms without needing an actual dialog. We could plan to support that for loading.

Error Handling: We should handle errors like failing to load or save (e.g., if the path is wrong or permission is denied). Raylib’s ExportImage returns a bool (true on success)
raylib.com
 which we can check. Similarly, after LoadImage we can check if image.width and image.height are > 0 to confirm success. On failure, we can output an error message (maybe to the console or a small on-screen notification text).

Platform Support (Windows & Linux) and Libraries

One of our goals is to run on both Windows and Linux. Thanks to raylib’s design, we can use the same C codebase for both, with minimal differences in compilation:

Raylib on Windows: We can include raylib.h and link against the raylib library (provided as a .lib or .a file depending on using MSVC or MinGW). Raylib includes all needed dependencies (OpenGL, GLFW, stb_image, etc.) internally

, so we don’t have to manage those separately. When building, we just need to ensure the linker finds raylib and the few system libraries it needs (on Windows, usually linking against winmm.lib, kernel32.lib, etc., is handled by raylib’s provided build info).

Raylib on Linux: We similarly link with the raylib library (often installed via package manager or compiled from source). On Linux, we may need to link math (-lm), pthread, X11, and OpenGL (-lGL) libraries as well, but again raylib’s build documentation covers that. The source code doesn’t change between OSes.

Because we use raylib for windowing and input, we do not need any OS-specific GUI toolkit. The code handles input via raylib’s cross-platform API (which uses the same function calls for both platforms) and drawing via OpenGL calls under the hood, which are also cross-platform. For saving and loading files, we rely on raylib (and its included rpng/stb libraries) so we aren’t calling OS-specific file dialogs or using platform-dependent image libraries. This ensures our program remains portable.

For build configuration, we can create a CMake project or simple Makefile that can target both Windows and Linux. Raylib provides example CMake configs which we can emulate – essentially find raylib, then link to it, etc. The architecture of the code (modules and logic) does not change per platform, so we mainly ensure conditional compilation if any (for example, Windows might use a different way to get the current working directory if we implement that, but likely unnecessary). We should also be mindful of file path differences (Windows uses backslashes, Linux forward slashes), but since we’ll mostly use simple names and let the OS handle it, we likely won’t run into issues. If needed, the user can provide a full path and we can just pass that string to LoadImage/ExportImage.

Additional Libraries: The project scope is intentionally kept small to avoid external dependencies beyond raylib. As noted, image encoding/decoding is handled by raylib’s integration of stb_image and its own rpng (a simple PNG loader/saver)
github.com
. If we decide to add a GUI library for more complex widgets (buttons, menus) later, one option is raygui (Raylib’s immediate-mode GUI extension). Raygui could simplify making buttons for “Save”, “Load”, tool icons, etc. For now, however, we can draw our UI elements manually and handle input without an extra library. This keeps the architecture straightforward. The only potential small library we might consider is something like tinyfiledialogs for a file dialog, but as discussed, it’s optional and perhaps overkill for the first version.

In summary, our application will use raylib as the central framework for all functionality (rendering, input, and image I/O), leveraging its cross-platform nature. This results in a clean and modular code structure:

Main: initialization and game loop control.

Canvas Module: managing the Image/Texture and providing functions to modify pixels.

Tools Module: handling pencil/eraser logic and input binding.

UI Module: drawing the palette and any on-screen indicators, handling color selection input.

File Module: providing save/load functions (wrapping raylib calls).

Each module interacts through shared state (e.g., the canvas Image/Texture and current color/tool are global or passed around as needed), but we can keep the code organized by functionality. For example, the Tools code will call Canvas functions to set pixels; the UI code will call Tools or modify global state when a color is picked. This separation makes it easier to expand features independently.

Conclusion and Future Extensions

With the above architecture, we fulfill the base requirements: drawing individual pixels with a pencil tool, erasing, selecting colors from a palette, zooming the view, and saving/loading PNG images. The choice of C with raylib provides low-level control and efficiency while simplifying cross-platform concerns. Each feature was planned with simplicity in mind, but the architecture can be extended. In the future, we could add more tools (line tool, fill bucket, shape drawing), support layers and animation (like Aseprite does), or improve the UI (color pickers, on-screen tool icons, etc.). The current design is a solid foundation – by managing the canvas as an editable image and leveraging raylib’s powerful functions

We ensure that adding such features later will be manageable. The program is essentially a small paint application running in a custom window, and our careful planning of modules and data structures will make the implementation straightforward and maintainable.

Sources: The development will reference raylib’s official documentation and cheatsheet for function usage and best practices

. The raylib community has also created similar pixel editors (e.g. Jolly Paint) which validate that this approach (Image for canvas + texture, point filtering, etc.) is effective
autopawn.itch.io
. By following this architecture plan, we can proceed to implement the pixel drawing program systematically, ensuring each component works correctly before moving to the next. The result will be a minimal yet functional pixel art editor available on both Windows and Linux platforms.
