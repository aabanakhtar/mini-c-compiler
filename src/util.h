#ifndef UTIL_H
#define UTIL_H

#include <cassert>

template<typename Type, template<typename...> class Args>
struct is_specialization_of final : std::false_type {};

template<template<typename...> class PointerType, typename... Args>
struct is_specialization_of<PointerType<Args...>, PointerType> final: std::true_type {};

#endif