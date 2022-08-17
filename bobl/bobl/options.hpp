// Copyright (c) 2015-2018 Serge Klimov serge.klim@outlook.com

#pragma once
#include "bobl/utility/utils.hpp"
#include <boost/fusion/algorithm/transformation/flatten.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/mpl/contains.hpp>
#include <type_traits>

namespace bobl{ 

template<typename ...Params>
struct Options {};

namespace options {
	using None = Options<>;
	struct RelaxedIntegers {};
	struct RelaxedFloats {};
    struct ExacMatch {};
	struct StructAsDictionary {}; 
	struct UsePositionAsName {};
	template<typename T> struct ByteType {};
	using IntegerOptimizeSize = RelaxedIntegers;
	using FloatOptimizeSize = RelaxedFloats;
	template<typename T> struct HeterogeneousArray {};
	template<typename T> using NonUniformArray = HeterogeneousArray<T>;
	struct OptionalAsNull {};
	template<typename T> struct UseTypeName { static_assert(utility::IsVariant<T>::value, "valid only for variants"); };
}// namespace options

template<typename T, typename ...Options>
struct EffectiveOptions
{	
	using type = bobl::Options<Options...>;
};

} /*namespace bobl*/
