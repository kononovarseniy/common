from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy


class KaCommonRecipe(ConanFile):
    name = "ka_common"
    version = "0.1.0"
    package_type = "library"

    license = "MIT"
    author = "kononovarseniy@gmail.com"
    url = "https://github.com/kononovarseniy/common"
    description = "Functions and types that are frequently used in my C++ projects"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def requirements(self):
        self.requires("fmt/9.1.0")
        
    def build_requirements(self):
        self.test_requires("gtest/1.17.0")

    def export_sources(self):
        copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
        copy(self, "cmake/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "include/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "src/*", self.recipe_folder, self.export_sources_folder)

    def validate(self):
        check_min_cppstd(self, "20")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if not self.conf.get("tools.build:skip_test", default=False):
            cmake.ctest(cli_args=["--output-on-failure"])

    def package(self):
        cmake = CMake(self)
        cmake.install()
  
        copy(self, "cmake/*", src=self.source_folder, dst=self.package_folder)

    def package_info(self):
        self.cpp_info.builddirs = ["cmake"]
        self.cpp_info.set_property("cmake_target_name", "ka::common")
        self.cpp_info.libs = ["ka_common"]
