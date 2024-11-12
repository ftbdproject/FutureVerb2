# Convert comma-separated string back to list
string(REPLACE "," ";" RESOURCE_FILES "${RESOURCE_FILES_STR}")

# Function to convert file to C++ array
function(file_to_cpp_array filename varname output_file)
    file(READ "${filename}" hex_content HEX)
    string(LENGTH "${hex_content}" hex_length)
    math(EXPR array_size "${hex_length} / 2")
    
    # Convert hex to array
    set(array_content "")
    string(REGEX MATCHALL ".." hex_bytes "${hex_content}")
    foreach(byte ${hex_bytes})
        string(APPEND array_content "0x${byte},")
        math(EXPR count "${count}+1")
        if(count EQUAL 16)
            string(APPEND array_content "\n    ")
            set(count 0)
        else()
            string(APPEND array_content " ")
        endif()
    endforeach()
    
    # Write to output
    file(APPEND "${output_file}" "static const unsigned char ${varname}[] = {\n    ${array_content}\n};\n")
    file(APPEND "${output_file}" "static const unsigned int ${varname}Size = ${array_size};\n\n")
endfunction()

# Initialize files
file(WRITE "${BINARY_DATA_DIR}/BinaryData.h" 
"#pragma once
namespace BinaryData
{
")

file(WRITE "${BINARY_DATA_DIR}/BinaryData.cpp"
"#include \"BinaryData.h\"
namespace BinaryData
{
")

# Process each resource file
foreach(resource_file ${RESOURCE_FILES})
    get_filename_component(filename "${resource_file}" NAME)
    string(MAKE_C_IDENTIFIER "${filename}" varname)
    
    # Add declaration to header
    file(APPEND "${BINARY_DATA_DIR}/BinaryData.h"
        "    extern const unsigned char ${varname}[];\n"
        "    extern const unsigned int ${varname}Size;\n\n"
    )
    
    # Generate array in cpp file
    file_to_cpp_array("${resource_file}" "${varname}" "${BINARY_DATA_DIR}/BinaryData.cpp")
endforeach()

# Close namespace
file(APPEND "${BINARY_DATA_DIR}/BinaryData.h" "}\n")
file(APPEND "${BINARY_DATA_DIR}/BinaryData.cpp" "}\n")
