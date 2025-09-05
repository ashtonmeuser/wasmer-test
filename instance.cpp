#include <stdio.h>
#include "wasm.h"

#define FAIL(message) do { printf("> Error %s!\n", message); return 1; } while (0)

int main(int argc, const char* argv[]) {
    // Read WebAssembly binary file
    FILE* file = fopen("test.wasm", "rb");
    if (!file) {
        FAIL("loading module file");
    }

    // Get file size
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    // Read file into wasm_bytes
    wasm_byte_vec_t wasm_bytes;
    wasm_byte_vec_new_uninitialized(&wasm_bytes, file_size);
    if (fread(wasm_bytes.data, 1, file_size, file) != file_size) {
        fclose(file);
        FAIL("loading module contents");
    }
    fclose(file);

    printf("Creating the store...\n");
    wasm_engine_t* engine = wasm_engine_new();
    wasm_store_t* store = wasm_store_new(engine);

    printf("Compiling module...\n");
    wasm_module_t* module = wasm_module_new(store, &wasm_bytes);

    if (!module) {
        FAIL("compiling module");
    }

    wasm_byte_vec_delete(&wasm_bytes);

    printf("Creating imports...\n");
    wasm_extern_vec_t imports = WASM_EMPTY_VEC;

    printf("Instantiating module...\n");
    wasm_instance_t* instance = wasm_instance_new(store, module, &imports, NULL);

    if (!instance) {
        FAIL("instantiating module");
    }

    printf("Retrieving exports...\n");
    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);

    if (exports.size == 0) {
        FAIL("accessing exports");
    }

    const wasm_func_t* add_one_func = wasm_extern_as_func(exports.data[0]);
    if (add_one_func == NULL) {
        FAIL("accessing export");
    }

    wasm_module_delete(module);
    wasm_instance_delete(instance);

    printf("Calling `add_one` function...\n");
    wasm_val_t args_val[1] = { WASM_I32_VAL(1) };
    wasm_val_t results_val[1] = { WASM_INIT_VAL };
    wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);
    wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

    if (wasm_func_call(add_one_func, &args, &results)) {
        FAIL("calling function");
    }

    printf("Results of `add_one`: %d\n", results_val[0].of.i32);

    wasm_extern_vec_delete(&exports);
    wasm_store_delete(store);
    wasm_engine_delete(engine);
}
