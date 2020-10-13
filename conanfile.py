from conans import ConanFile, CMake, tools


class SaisConan(ConanFile):
    name = "sais"
    license = "GPL"
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        'sdl2/2.0.12@bincrafters/stable',
        'sdl2_mixer/2.0.4@bincrafters/stable',
        'physfs/3.0.2@xsquawkbox/testing',
    ]
    default_options = {
        "*:shared": False,
        "sdl2:iconv": False,
        "sdl2_mixer:fluidsynth": False,
        "sdl2_mixer:mad": False,
        "sdl2_mixer:mikmod": False,
        "sdl2_mixer:modplug": False,
        "sdl2_mixer:mpg123": False,
        "sdl2_mixer:opus": False,
        "sdl2_mixer:flac": False,
        "sdl2_mixer*:tinymidi": False,
        "sdl2_mixer:wav": True,
    }
    generators = "cmake"

    def configure(self):
        # SDL2 on macOS requires iconv
        if self.settings.os == "Macos":
            self.options["sdl2"].iconv = True
