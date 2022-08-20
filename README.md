# VGED-ENGINE
<br>
A C++ Game Engine for Linux, Windows, and macOS<br>
<br>
Clone or download the repo and from the top level directory build using "./build.sh" or "./build.sh release"<br>

## File structure and engine structure
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
All assets go into seperate Git submodules, e.g. ~/application/Scabb/assets<br>

## Code formatting and coding guide
PascalCase for structs and classes. snakecase for variables, functions, and file names<BR>
vk stuff will have vk prefix, language: American English, indention: four spaces<BR>
Everything goes into namespaces<BR>
Header files are included with path+filename; some predefined paths are provided:<BR>
~/engine<BR>
~/engine/platform/Vulkan<BR>
~/vendor<BR>
Try to avoid heap allocating per frame, rather stack allocate memory or preallocate memory in pools<br>
Use references to pass arguments as much as possible<br>
Use smart pointers to own data and pass references of those or raw pointers around to share this data when possible<br>
Keep files short and sweet (one class per file if possible)<br>
Member fields of classes and structs start with m_ and continue with PascalCase<br>
The formatting of opening and closing parentheses, brackets, and braces () [] {} is as follows <br>
<pre>
void Layer::setTag(const std::string& tag) {
    m_Tag = tag;
}
</pre>
## Types
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

## Planned render engine features
One default render sequence with a lighting renderpass (geometry, lighting, transparency subpasses) and GUI renderpass (2D subpass)<br>
Abstraction for custom render sequences<br>
Abstraction layer for different APIs via pure virtual functions<br>
PBR lighting<br>
Material system<br>
Support for custom shaders<br>
Anti-aliasing<br>
Shadow Mapping, Ambient Occlusion (AO), and Global Illumination (GI)<br>
Skybox<br>
Bloom<br>
Reflections<br>
GPU ray tracing support is detected: If a feature is implemented with ray tracing, the same feature should be provided for the raster pipeline<br>
A debug GUI based on ImGUI is provided for developers<br>
God rays, haze, dust, mist, vapor, steam, smoke, and fog<br>
Particle system<br>

## Planned game engine features
Entity-Component-System (ECS)<br>
Scene and prefab loading/unloading<br>
Supported file types for 3D models: glTF, Obj Wavefront, FBX<br>
Built-in movement (physics)<br>
Menu/in-game GUI<br>
Texture atlas support and texture atlas generator, spritesheet abstraction/animation<br>
Game controller support<br>
Sound support<br>
Profiling/Instrumentation<br>
Resource system for crucial assets (app icon, minimal shader to display warning messages, gamecontroller DB)<br>
Settings manager<br>
Event system<br>
Layer stack<br>
Debug logger (spdlog)<br>
  
## Planned Sripting languages
Native C++ scripting and/or C# and/or Python<br>
Hot-reloading<br>
