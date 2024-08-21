#include <print>
#include <iostream>
#include <RxHorkEye/Core/ARC.h>


auto main() -> int
{
    try
    {
        ZQF::RxHorkEye::ARC arc;
        arc.Import("z/", "arc6.dat");
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
