// clang-format off
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cJSON.h"

///*///
struct JSON{Vector<String> parsed_strings};

namespace JSON

function __init__()
    this.parsed_strings.__init__(1)
endfunction

c_function Parse(json_string : str, len : int) -> bool:
    // Parse the JSON and store the strings in 'parsed_strings'.
    Vector_Stringclear(&this->parsed_strings);

    // 1. Input Validation
    if (!json_string || len <= 0) {
        fprintf(stderr, "JSON Error: Invalid input parameters.\n");
        return false;
    }

    // 2. Create Null-Terminated Copy
    // cJSON_Parse requires a null-terminated string.
    char* temp_json_buffer = (char*)malloc(len + 1);
    if (!temp_json_buffer) {
        fprintf(stderr, "JSON Error: Failed to allocate memory for JSON buffer.\n");
        return false;
    }

    memcpy(temp_json_buffer, json_string, len);
    temp_json_buffer[len] = '\0';

    // 3. Parse JSON 
    cJSON *root = cJSON_Parse(temp_json_buffer);

    // 4. Free Temporary Buffer (no longer needed after parsing)
    free(temp_json_buffer);
    temp_json_buffer = NULL; // Avoid dangling pointer use

    // 5. Check for Parse Errors
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr(); // Get error location if available
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON Error: JSON parsing failed near: %s\n", error_ptr);
        } else {
            fprintf(stderr, "JSON Error: JSON parsing failed (unknown location).\n");
        }
        // root is already NULL, nothing to delete
        return false;
    }

    // 6. Validate Root Type (must be an array)
    if (!cJSON_IsArray(root)) {
        fprintf(stderr, "JSON Error: Root JSON element is not an array.\n");
        cJSON_Delete(root); // Free the parsed JSON structure
        return false;
    }

    // 7. Iterate Through JSON Array
    int array_size = cJSON_GetArraySize(root);
    bool success = true; // Assume success initially

    for (int i = 0; i < array_size; i++) {
        cJSON *item = cJSON_GetArrayItem(root, i);

        // 8. Validate Item Type (must be a string)
        if (cJSON_IsString(item) && (item->valuestring != NULL)) {
            // 9. Create a String object and add it to the vector
            struct String todo_str;
            String__init__OVDstr(&todo_str, item->valuestring);
            Vector_Stringpush(&this->parsed_strings, todo_str);
            String__del__(&todo_str);
        } else {
             // Handle items that are not strings (e.g., numbers, null, objects)
             fprintf(stderr, "JSON Warning: Item at index %d is not a string. Skipping.\n", i);
             // Depending on requirements, we might want to set success = false here
             // success = false;
             // break; // Or stop processing immediately on invalid item
        }
    } // End of loop

    // 10. Cleanup cJSON object
    cJSON_Delete(root);

    // Return true if parsing and iteration were generally successful
    return success; 
endc_function
endnamespace
///*///
