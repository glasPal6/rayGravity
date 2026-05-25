#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#include <string.h>
// #define DEBUG
#define BUILD_DIR "build/"
#define SRC_DIR "src/"
#define INCLUDE_DIR "include/"
#define LIBRARY_DIR "lib/"

void compile_flags(Nob_Cmd* cmd) {
    cmd_append(cmd, "-Wall", "-Wextra", "-Wno-unused-parameter");
    cmd_append(cmd, "-DGRAPHICS_API_OPENGL_ES2");
}

void profile_debug_flags(Nob_Cmd* cmd) {
#ifdef DEBUG
    cmd_append(cmd, "-ggdb");
#endif /* ifdef DEBUG */
}

void include_dirs(Nob_Cmd* cmd) {
    cmd_append(cmd, "-I", INCLUDE_DIR "raylib");
    cmd_append(cmd, "-I", INCLUDE_DIR "clay");
}

bool copy_dynamic_libs(char* library_name) {
    Nob_Cmd copyLib = {0};
    cmd_append(&copyLib, "cp", library_name, BUILD_DIR);
    cmd_run_sync(copyLib);
    if (!cmd_run_sync(copyLib)) {
        nob_log(NOB_ERROR, "Failed to copy EGL library.\n");
        return false;
    }
    cmd_free(copyLib);
    return true;
}

bool compile_main(const char* main_output, const char* main_src) {
    Nob_Cmd cmd = {0};
    cmd_append(&cmd, "gcc");
    compile_flags(&cmd);
    profile_debug_flags(&cmd);
    include_dirs(&cmd);
    cmd_append(&cmd, "-o", main_output, main_src, "-O3");
    cmd_append(&cmd, LIBRARY_DIR "libraylib.a", "-L" LIBRARY_DIR, "-lEGL",
               "-L" LIBRARY_DIR, "-lGLESv2");
    cmd_append(&cmd, "-Wl,-rpath,../" LIBRARY_DIR);
    cmd_append(&cmd, "-lm", "-lpthread");
    cmd_append(&cmd, "-framework", "CoreVideo", "-framework", "IOKit",
               "-framework", "CoreFoundation", "-framework", "Cocoa");
    bool cmd_result = cmd_run_sync(cmd);
    if (!cmd_result) {
        nob_log(NOB_ERROR, "Error in compiling the main");
        return false;
    }
    cmd_free(cmd);
    return true;
}

int main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    // Create the build directory
    if (!mkdir_if_not_exists(BUILD_DIR)) return 1;
    if (!copy_dynamic_libs(LIBRARY_DIR "libEGL.dylib")) return 1;
    if (!copy_dynamic_libs(LIBRARY_DIR "libGLESv2.dylib")) return 1;
    // Primary build command
    if (!compile_main(BUILD_DIR "main", SRC_DIR "main.c")) return 1;
    return 0;
}
