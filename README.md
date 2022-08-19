# VGED-ENGINE
<br>
A C++ Game Engine<br>
<br>
Clone or download the repo and from the top level directory build using ./build.sh<br>

## file structure and engine structure
|-application<br>
|-engine<br>
|-editor<br>
|-resources<br>
|-scripts<br>
|-vendor<br>
<br>
Main project domains: render engine, game engine, scene/game editor, application<br>
Users add their games as an external Git submodule under ~/application<br>
An example application is provided under ~/application/Scabb<br>

## code formatting and coding guide
CamelCase for structs and classes. snakecase for variables, functions, and file names<BR>
vk stuff will have vk prefix, language: American English, indention: four spaces<BR>
Everything goes into namespaces<BR>
header files are included with path+filename; some predefined paths are provided:<BR>
~/engine<BR>
~/engine/platform/Vulkan<BR>
~/vendor<BR>
Try to avoid heap allocating per frame, rather stack allocate memory or preallocate memory in pools<bre>
Use references to pass arguments as much as possible<br>
Use smart pointers to own data and pass references of those or raw pointers around to share this data<br>
Keep files short and sweet (one class per file if possible)<br>

## types
using u8 = std::uint8_t;<br>
using u16 = std::uint16_t;<br>
using u32 = std::uint32_t;<br>
using u64 = std::uint64_t;<br>
using usize = std::size_t;<br>
<br>
using i8 = std::int8_t;<br>
using i16 = std::int16_t;<br>
using i32 = std::int32_t;<br>
using i64 = std::int64_t;<br>
using isize = std::ptrdiff_t;<br>
<br>
using f32 = float;<br>
using f64 = double;<br>

## Render Engine Features
One default render sequence with a lighting renderpass (geometry, lighting, transparency subpasses) and GUI renderpass (2D subpass)<br>
Abstraction for custom render sequences<br>
Abstraction layer for different APIs via pure virtual functions<br>
PBR lighting<br>
Support for custom shaders<br>
Ambient occlusion and global illumination<br>
Anti-aliasing<br>
Shadow Mapping and Ambient Occlusion (AO)<br>
Skybox<br>
Bloom<br>
Reflections<br>
GPU ray tracing support is detected: If a feature is implemented with ray tracing, the same feature should be provided for the raster pipeline<br>
A debug GUI based on ImGUI is provided for developers<br>
God rays, haze, dust, mist, vapor, steam, smoke, and fog<br>
Particle system<br>

## Game Engine Features
Entity-Component-System (ECS)<br>
Scene and prefab loading/unloading<br>
Supported file types for 3D models: glTF, Obj Wavefront, FBX<br>
Built-in movement (physics)<br>
Menu/in-game GUI<br>
Game controller support<br>
Sound support<br>
Resource system for crucial assets (app icon, minimal shader to display warning messages, gamecontroller DB)<br>
Event system<br>

## scripting language
Native C++ scripting and/or c# and/or Python<br>
Hot-reloading<br>
