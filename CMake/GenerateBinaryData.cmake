# cmake/GenerateBinaryData.cmake

# Function to convert a file to a C++ array
function(file_to_cpp_array filename varname output_file)
    file(READ "${filename}" hex_content HEX)
    string(LENGTH "${hex_content}" hex_length)
    math(EXPR array_size "${hex_length} / 2")

    # Convert hex string to array initialization
    set(array_content "")
    string(REGEX MATCHALL ".." hex_bytes "${hex_content}")
    foreach(byte ${hex_bytes})
        string(APPEND array_content "0x${byte}, ")
    endforeach()

    # Write to output file
    file(APPEND "${output_file}" "static const unsigned char ${varname}[] = {${array_content}};\n")
    file(APPEND "${output_file}" "static const unsigned int ${varname}Size = ${array_size};\n\n")
endfunction()

# Clear previous content
file(WRITE "${BINARY_DATA_DIR}/BinaryData.h" "#pragma once\n\nnamespace BinaryData {\n")
file(WRITE "${BINARY_DATA_DIR}/BinaryData.cpp" "#include \"BinaryData.h\"\n\nnamespace BinaryData {\n")

# Process each resource file
foreach(resource_file ${RESOURCE_FILES})
    # Generate variable name from filename
    get_filename_component(filename "${resource_file}" NAME)
    string(MAKE_C_IDENTIFIER "${filename}" varname)

    # Add declaration to header
    file(APPEND "${BINARY_DATA_DIR}/BinaryData.h" "    extern const unsigned char ${varname}[];\n")
    file(APPEND "${BINARY_DATA_DIR}/BinaryData.h" "    extern const unsigned int ${varname}Size;\n\n")

    # Generate array in cpp file
    file_to_cpp_array("${resource_file}" "${varname}" "${BINARY_DATA_DIR}/BinaryData.cpp")
endforeach()

# Close namespace
file(APPEND "${BINARY_DATA_DIR}/BinaryData.h" "}\n")
file(APPEND "${BINARY_DATA_DIR}/BinaryData.cpp" "}\n")
