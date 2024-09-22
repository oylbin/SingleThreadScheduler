from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps

class SingleThreadSchedulerConan(ConanFile):
    name = "singlethreadscheduler"
    version = "0.1.0"
    
    # 可选：许可证、作者等信息
    license = "MIT"
    author = "oylbin<oylbin@gmail.com>"
    url = "https://github.com/oylbin/SingleThreadScheduler"
    description = "A single thread scheduler"
    
    # 设置和选项
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "test": [True, False]  # 新增选项
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "test": False  # 默认不构建测试
    }
    
    # 定义源代码位置
    exports_sources = "src/*", "include/*", "test/*", "CMakeLists.txt"
    
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    # def requirements(self):
    #     self.requires("gtest/1.11.0")

    def build_requirements(self):
        if self.options.test:
            self.test_requires("gtest/1.11.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_TESTING"] = self.options.test
        tc.generate()
        
        # 添加这一行来生成 CMakeDeps
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["SingleThreadScheduler"]