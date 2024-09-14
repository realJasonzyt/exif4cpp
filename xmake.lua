add_requires("gtest")

target("exif4cpp")
    set_kind("static")
    add_files("src/*.cpp")
    add_headerfiles("src/include/*.h")
    add_includedirs("src/include")

target("exif4cpp_test")
    set_kind("binary")
    add_deps("exif4cpp", "gtest")
    add_files("test/*.cpp")
