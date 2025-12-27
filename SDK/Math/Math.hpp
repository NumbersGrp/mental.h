#pragma once

#include <memory>
namespace mentalsdk
{

template <typename T>
struct Vector3 {
    T x;
    T y;
    T z;
};

template <typename T>
struct Vector2 {
    T x;
    T y;
};

template <typename T>
std::shared_ptr<Vector3<T>> vec3(T first_value = 1.0F, T second_value = 1.0F, T third_value = 1.0F)
{
    return std::make_shared<Vector3<T>>(Vector3<T>{first_value, second_value, third_value});
}

template <typename T>
std::shared_ptr<Vector2<T>> vec2(T first_value = 1.0F, T second_value = 1.0F)
{
    return std::make_shared<Vector2<T>>(Vector2<T>{first_value, second_value});
}





} // mentalsdk