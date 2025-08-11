# engine/CMakeLists.txt
project("VulkanEngine")

file(GLOB_RECURSE ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE ENGINE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hxx"
)

# Engine as a static library
add_library(VulkanEngine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Add engine source files
#add_subdirectory(src)
#add_subdirectory(include)
add_subdirectory(dependencies) # This may change, want to use submodules for dependencies

# Link Vulkan to the engine
target_link_libraries(VulkanEngine PUBLIC Vulkan::Vulkan)

# Make engine headers available to projects that link against it
target_include_directories(VulkanEngine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies
)

# Engine-specific compile features
target_compile_features(VulkanEngine PUBLIC cxx_std_23)

### Engine Shader Compilation
find_program(GLSL_VALIDATOR glslangValidator HINTS 
    /usr/bin 
    /usr/local/bin 
    $ENV{VULKAN_SDK}/Bin/ 
    $ENV{VULKAN_SDK}/Bin32/
)

if(NOT GLSL_VALIDATOR)
    message(WARNING "Could not find glslangValidator! Shaders will not be compiled.")
endif()

# Find engine shader files
file(GLOB_RECURSE ENGINE_GLSL_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.comp"
)

# Compile engine shaders
set(ENGINE_SPIRV_BINARY_FILES "")
foreach(GLSL ${ENGINE_GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
    
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
        COMMENT "Compiling engine shader ${FILE_NAME}"
    )
    list(APPEND ENGINE_SPIRV_BINARY_FILES ${SPIRV})
endforeach()

# Engine shader target
if(ENGINE_SPIRV_BINARY_FILES)
    add_custom_target(EngineShaders DEPENDS ${ENGINE_SPIRV_BINARY_FILES})
    add_dependencies(VulkanEngine EngineShaders)
endif()

# Function to help projects compile their own shaders
function(compile_project_shaders PROJECT_NAME PROJECT_DIR)
    file(GLOB_RECURSE PROJECT_GLSL_FILES
        "${PROJECT_DIR}/shaders/*.frag"
        "${PROJECT_DIR}/shaders/*.vert"
        "${PROJECT_DIR}/shaders/*.comp"
    )
    
    set(PROJECT_SPIRV_FILES "")
    foreach(GLSL ${PROJECT_GLSL_FILES})
        get_filename_component(FILE_NAME ${GLSL} NAME)
        set(SPIRV "${PROJECT_DIR}/shaders/${FILE_NAME}.spv")
        
        add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
            COMMENT "Compiling ${PROJECT_NAME} shader ${FILE_NAME}"
        )
        list(APPEND PROJECT_SPIRV_FILES ${SPIRV})
    endforeach()
    
    if(PROJECT_SPIRV_FILES)
        add_custom_target(${PROJECT_NAME}Shaders DEPENDS ${PROJECT_SPIRV_FILES})
        add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}Shaders)
    endif()
endfunction()