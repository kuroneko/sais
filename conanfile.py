from conan import ConanFile
from conan.tools.cmake import cmake_layout

class SaisConan(ConanFile):
    name = "sais"
    version = "1.7.0"
    license = "GPL"
    settings = "os", "compiler", "build_type", "arch"
    tool_requires = [
        'cmake/4.2.3',
    ]
    requires = [
        'sdl/2.32.10',
        'sdl_mixer/2.8.1',
        'physfs/3.2.0',
    ]
    default_options = {
        "*/*:shared": False,
        "sdl/*:iconv": False,
        "sdl_mixer/*:fluidsynth": False,
        "sdl_mixer/*:mad": False,
        "sdl_mixer/*:mikmod": False,
        "sdl_mixer/*:modplug": False,
        "sdl_mixer/*:mpg123": False,
        "sdl_mixer/*:opus": False,
        "sdl_mixer/*:flac": False,
        "sdl_mixer/*:tinymidi": False,
        "sdl_mixer/*:wav": True,
    }
    generators = "CMakeConfigDeps", "CMakeToolchain"

    def configure(self):
        # SDL2 on macOS requires iconv
        if self.settings.os == "Macos":
            self.options["sdl2"].iconv = True

    def layout(self):
        cmake_layout(self)