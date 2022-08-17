### bobl

[![Build Status](https://travis-ci.com/serge-klim/bobl.svg?branch=master)](https://travis-ci.com/serge-klim/bobl)
[![Build status](https://ci.appveyor.com/api/projects/status/pj023kupejhaccr7?svg=true)](https://ci.appveyor.com/project/serge-klim/bobl)
[![codecov](https://codecov.io/gh/serge-klim/bobl/branch/master/graph/badge.svg)](https://codecov.io/gh/serge-klim/bobl)


This is an attempt to serialize/desirialize C++ types in well defined binary format, as simple as this:
```	
	namespace protocol = bobl::bson;
	// auto value = Type{...};
	std::vecor<std::uint8_t>  data = protocol::encode(value);
	auto begin = cbegin(data);
	Type value = protocol::decode<Type>(begin, cend(data));
```
As for now library supports basic [bson](http://bsonspec.org/spec.html) and [cbor](http://cbor.io/spec.html) encoding/decoding.

### Requirements
Library is header only library therefore no separately-compiled library binaries or special treatment required. However it's using following [boost](http://www.boost.org) libraries:

 - [MPL](http://www.boost.org/doc/libs/1_66_0/libs/mpl/doc/index.html)
 - [Fusion](http://www.boost.org/doc/libs/1_66_0/libs/fusion/doc/html/)
 - [Uuid](https://www.boost.org/doc/libs/1_67_0/libs/uuid/doc/index.html)
 - [Endian](https://www.boost.org/doc/libs/1_67_0/libs/endian/doc/index.html)
 - [NumericConversion](https://www.boost.org/doc/libs/1_67_0/libs/numeric/conversion/doc/html/index.html)
 - [Range](https://www.boost.org/doc/libs/1_67_0/libs/range/doc/html/index.html)
 - [TypeIndex](https://www.boost.org/doc/libs/1_67_0/doc/html/boost_typeindex.html)
 - [Format](https://www.boost.org/doc/libs/1_67_0/libs/format/)

these are also header only libraries so just make sure that compiler can find them.

C++11 compatible compiler (clang 3.6+, gcc 4.8.5+, msvc-14.1+).

### How it works:

lets say that there is encoded bson object:
```
	//{'enabled': True, 'id': 100, 'name': 'the name', 'theEnum': 2}
	//(55) : b'7\x00\x00\x00\x08enabled\x00\x01\x10id\x00d\x00\x00\x00\x02name\x00\t\x00\x00\x00the name\x00\x10theEnum\x00\x02\x00\x00\x00\x00'
```
it can be decoded into std::tuple like this:
```
    	std::tuple<bool, int, std::string, int> res = protocol::decode<std::tuple<bool, int, std::string, int>>(begin, end);
```
or std::tuple in decode can be omited:
```
    	std::tuple<bool, int, std::string, int> res = protocol::decode<bool, int, std::string, int>(begin, end);
```
BSON complete example: [simple1.cpp](https://github.com/serge-klim/bobl/blob/master/examples/bson/simple1.cpp)    
CBOR complete example: [simple1.cpp](https://github.com/serge-klim/bobl/blob/master/examples/cbor/simple1.cpp)
```
	std::uint8_t data[] = {
		0x37, 0x0,  0x0,  0x0,  0x8,  0x65, 0x6e, 0x61, 0x62, 0x6c, 0x65,
		0x64, 0x0,  0x1,  0x10, 0x69, 0x64, 0x0,  0x64, 0x0,  0x0,  0x0,
		0x2,  0x6e, 0x61, 0x6d, 0x65, 0x0,  0x9,  0x0,  0x0,  0x0,  0x74,
		0x68, 0x65, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x0,  0x10, 0x74, 0x68,
		0x65, 0x45, 0x6e, 0x75, 0x6d, 0x0,  0x2,  0x0,  0x0,  0x0,  0x0 };

	uint8_t const* begin = data;
	uint8_t const* end = begin + sizeof(data) / sizeof(data[0]);
	auto res = protocol::decode<bool, int, std::string, EnumClass>(begin, end);
```
using tuples for complex types might be not really good idea that's where [Boost.Fusion](https://www.boost.org/doc/libs/1_50_0/libs/fusion/doc/html/fusion/adapted/adapt_struct.html) can be very useful, it allows adapt structure to heterogeneous container. So above could be decoded like this:
```
enum class EnumClass { None, One, Two, Three };

struct Simple
{
  bool enabled;
  int id;
  std::string name;
  EnumClass theEnum;
};

BOOST_FUSION_ADAPT_STRUCT(
  Simple,
  enabled,
  id,
  name,
  theEnum)

Simple simple = protocol::decode<Simple>(begin, end);
```
as well as encoded:

```
std::vector<std::uint8_t> blob = protocol::encode(simple);
```
BSON complete example: [simple3.cpp](https://github.com/serge-klim/bobl/blob/master/examples/bson/simple3.cpp)  
CBOR complete example: [simple3.cpp](https://github.com/serge-klim/bobl/blob/master/examples/cbor/simple3.cpp)


std::tuple also can be encoded, but BSON requires names for objects. In case of adapted structures, member name became appropriate object name. With tuples some naming needed, to solve it there is few options:
1. position of tuple element can be used as an element name:
```
	auto value = std::make_tuple(true, 100, std::string{ "the name" }, EnumClass::Two);
	auto data = protocol::encode<bobl::options::UsePositionAsName>(value);
```
The resulting object will look like this `{"_0": True, "_1": 100, "_2": "the name", "_3": 2}` pseudo-json representation.  
2. another way to name tupple element is specialize `bobl::MemberName` class declared in [`bobl/names.hpp`](https://github.com/serge-klim/bobl/blob/master/bobl/names.hpp) header :
```
#include "bobl/names.hpp"

namespace bobl {
  template<typename Type, typename MemberType, std::size_t Position, typename Options> class MemberName;
}
``` 
like this:
```
namespace bobl{

	template<typename MemberType, typename Options> class MemberName <SimpleTuple, MemberType, 0, Options>
	{
	public:
		constexpr char const* operator()() const { return "enabled"; }
	};

	template<typename Options> class MemberName <SimpleTuple, int, 1, Options>
	{
	public:
		constexpr char const* operator()() const { return "id"; }
	};

	template<std::size_t Position, typename Options> class MemberName <SimpleTuple, std::string, Position , Options>
	{
	public:
		constexpr char const* operator()() const { return "name"; }
	};

	template<typename MemberType, typename Options> class MemberName <SimpleTuple, MemberType, 3, Options>
	{
	public:
		constexpr char const* operator()() const { return "enm"; }
	};

}//namespace bobl

``` 
so encode on tuple would work the same way as it does on [Boost.Fusion](https://www.boost.org/doc/libs/1_50_0/libs/fusion/doc/html/fusion/adapted/adapt_struct.html) adapted structures:

``` 
  auto tuple = std::make_tuple(true, 100, std::string{ "the name" }, Enum::Two);
  auto data = protocol::encode(tuple);
```
BSON complete example: [named\_tuple.cpp](https://github.com/serge-klim/bobl/blob/master/examples/bson/named_tuple.cpp)  
CBOR complete example: [named\_tuple.cpp](https://github.com/serge-klim/bobl/blob/master/examples/cbor/named_tuple.cpp)


the library could handle more complex types, for example:

```
struct Extended
{
	int id;
	Simple simple;
	std::vector<int> ints;
	std::vector<Simple> simples;
	boost::variant<int,Simple, std::string, std::vector<Simple>> var;
	boost::uuids::uuid uuid;
	boost::optional<Enum> enm;
	std::vector<std::uint8_t> binary; // this will be encoded as binary object
	std::chrono::system_clock::time_point tp;
};

BOOST_FUSION_ADAPT_STRUCT(
	Extended,
	id,
	simple,
	ints,
	simples,
	var,
	uuid,
	enm,
	binary,
	tp)

	auto extended = Extended{};
	std::vector<std::uint8_t> encoded =  protocol::encode(extended)
```
BSON complete example: [extended.cpp](https://github.com/serge-klim/bobl/blob/master/examples/bson/extended.cpp)  
CBOR complete example: [extended.cpp](https://github.com/serge-klim/bobl/blob/master/examples/cbor/extended.cpp)

#### Supported types

 - [integers](#integers)
 - [floating point](#floating-point)
 - [enum and enum class](#enum-and-enum-class)
 - [std::string](#stdstring)
 - [std::vector](#stdvector)
 - boost::uuid
 - std::chrono::system_clock::time_point
 - [boost::optional](#boostoptional)
 - [boost::variant](#boostvariant)
 - [Boost.Fusion adapted structures](#boostfusion-adapted-structures)

#### Options.

Options allows to controls encoding/decoding. Options are defined in [options.hpp](https://github.com/serge-klim/bobl/blob/master/bobl/options.hpp):
```
    struct RelaxedIntegers {};
    struct RelaxedFloats {};
    struct ExacMatch {};
    struct StructAsDictionary {}; 
    struct UsePositionAsName {};
    template<typename T> struct ByteType {};
    using IntegerOptimizeSize = RelaxedIntegers;
    template<typename T> struct HeterogeneousArray {};
    template<typename T> using NonUniformArray = HeterogeneousArray<T>;
    struct OptionalAsNull{};
    template<typename T> struct UseTypeName{};
```
This options can be used as explicitly set encode/decode functions template parameters and/or could be set per specific type by specializing `bobl::EffectiveOptions` structure:
```
	template<typename T, typename ...Options>
	struct EffectiveOptions
	{   
		using type = bobl::Options<Options...>;
	};
```
for example to encode tuple as object using tuples element position as an object member name following specialization of `bobl::EffectiveOptions` structure can be used:

```
namespace bobl{
   template<typename ...Types, typename ...Options>
   struct EffectiveOptions<std::tuple<Types...>, Options...>
   {   
	   using type = bobl::Options<bobl::options::UsePositionAsName, Options...>;
   };
} //namespace bobl
```
Also if such options has to be set for specific protocol(BSON or CBOR) `bobl::<protocol name>` namespace can be used, following will set `bobl::options::UsePositionAsName` for tuples used with cbor encode/decode functions:

```
namespace bobl{
  namespace cbor{
	template<typename ...Types, typename ...Options>
	struct EffectiveOptions<std::tuple<Types...>, Options...>
	{   
		using type = bobl::Options<bobl::options::UsePositionAsName, Options...>;
	};
  } //namespace cbor
} //namespace bobl
```

##### Integers
Any integer type for which `std::is_integral<T>::value` is true.
By default encoded/decoded integer type based on its C++ type (not on its value size) which means that `std::uint64_t` containing value 1 will be encoded as [BSON](http://bsonspec.org/spec.html) - "\x12" (int64) type, it makes encoding/decoding bit faster. Using `bobl::option::IntegerOptimizeSize` option during encoding and `bobl::option::RelaxedIntegers` option during decoding allows change such behavior.

```
	//{'int': 1}
	//(14) : b'\x0e\x00\x00\x00\x10int\x00\x01\x00\x00\x00\x00'
	//                          ^^^ int32
	std::uint8_t data[] = { 0xe, 0x0, 0x0, 0x0, 0x10, 0x69, 0x6e, 0x74, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 };
	uint8_t const *begin = data;
	uint8_t const* end = begin + sizeof(data) / sizeof(data[0]);
	std::tuple<std::uint64_t> res = protocol::decode<std::uint64_t, bobl::Options<bobl::options::RelaxedIntegers>>(begin, end);
	assert(std::get<0>(res) == 1);
```
##### Floating point
Any floating point type for which `std::is_floating_point<T>::value` is true.

##### Enum and enum class
Enums end enum classes are encoded/decoded as underlying [integer type](#integers).

##### std::string
std::string encoded/decoded as raw UTF-8 string.

##### std::vector
`std::vector` can be used with any [supported types](#supported-types) and encoded/decoded as [bson](http://bsonspec.org/spec.html)/[cbor](http://cbor.io/spec.html) arrays. Except `std::vector<std::uint8_t>` which is encoded/decoded as byte string([CBOR](https://tools.ietf.org/html/rfc7049)  Major type 2) / binary data ([BSON](http://bsonspec.org/spec.html) - "\x05"). bobl::option::ByteType allows to encode/decode `std::vector` specialized with any other types(for which sizeof(T) is equal to `sizeof(std::uint8_t)`) as byte string. 

```
    std::vector<char> binary =  {100, 110, 120};
    bobl::cbor::encode<bobl::Options<bobl::options::ByteType<char>>>(...)
	// ...
    std::vector<char> = bobl::cbor::decode<std::vector<char>, bobl::Options<bobl::options::ByteType<char>>>(begin, end);

```
#### boost::optional
`boost::optional` can be used with any [supported types](#supported-types). By default encode skips std::optional to reduce size of encoded object which might leads to incorrect decoding. If decoded `std::tuple` with more than one type(encoded to same protocol specific type) following each other and at least one of these types is optional.
Here is an example:
```
enum class Type { One, Two, Three };

struct Data
{
	boost::optional<Type> type; //will be encoded as int
	int id;
};

BOOST_FUSION_ADAPT_STRUCT(Data, type, id)

	auto data  = Data { {}, 123};
	std::vector<std::uint8_t> encoded =  protocol::encode(data);
	auto begin = encoded.data();
	auto end = begin + encoded.size();
```
this will work as expected:
```
	auto decoded =  protocol::decode<Data>(begin, end);	
```
this on the other hand will throw an exception:
```
	protocol::decode<boost::optional<Type>, int>(begin, end);
```
Data structure with empty optional member type will be encoded as `{"id":123}` pseudo-json representation. Therefore decoding to unnamed tuple will fail because of integer 123 will be decoded as optional enum class type and required id needs value.  

BSON complete example: [optional.cpp](https://github.com/serge-klim/bobl/blob/master/examples/bson/optional.cpp)  
CBOR complete example: [optional.cpp](https://github.com/serge-klim/bobl/blob/master/examples/cbor/optional.cpp)

if decoding in unnamed tuple is required `bobl::options::OptionalAsNull` can be used at encoding:
```
	auto data  = Data { {}, 123};
	auto encoded =  protocol::encode<bobl::options::OptionalAsNull>(data);
```
it will produce: `{"type":null, "id":123}` pseudo-json representation. Which can be decoded in unnamed tuple just fine.

```
	auto res = protocol::decode<boost::optional<Type>, int>(begin, end); // ok
```

#### boost::variant
`boost::variant` can be used with any [supported types](#supported-types), except `boost::optional` which wouldn't make much sense.
When decoded `boost::variant` decode try to decode types in order of declaration. So if two types encoded as same type for example [integers](#integers) and [enums](#enum-and-enum-class) it will be decoded as first declared type.

for complex similar types probing to decode over each variant type might be sub optimal. To avoid it `bobl::options::UseTypeName<>` can be used. It makes encoder overrides member name with type-name which allows to decode faster and avoid similar types issue described above.  
For example:

```
	auto value =boost::variant<int, Enum>{100};
	auto data = protocol::encode<bobl::Options<bobl::options::UseTypeName<boost::variant<int, Enum>>>>(value);
```

It will be encoded as `{"int":123}` pseudo-json representation. Type name ("int" in this case) is compiler dependant and generated with `boost::typeindex::type_id<T>().pretty_name()` to customize type-name `bobl::TypeName` class declared in [`bobl/names.hpp`](https://github.com/serge-klim/bobl/blob/master/bobl/names.hpp) header can be specialized.


If using `bobl::options::UseTypeName<>` is to verbose, the same effect can be achieved  by adding bobl::UseTypeName tag to list of variant types, like this:
```
	auto value = boost::variant<int, Enum, bobl::UseTypeName> {100};
	auto data = protocol::encode(value);
```

BSON complete example: [variant.cpp](https://github.com/serge-klim/bobl/blob/master/examples/bson/nvariant.cpp)  
CBOR complete example: [variant.cpp](https://github.com/serge-klim/bobl/blob/master/examples/cbor/nvariant.cpp)

#### Boost.Fusion adapted structures

[Boost.Fusion](https://www.boost.org/doc/libs/1_50_0/libs/fusion/doc/html/fusion/adapted/adapt_struct.html) adapted structures are encoded/decoded as bson/cbor objects, also called tables, dictionaries, hashes or maps of name-values pairs. By default member of structures decoded/encoded in they declaration order. It is possible to decode such objects encoded in different order if resulting C++ object is default constructible and `bobl::options::StructAsDictionary` option is specified. 

Also decoding ignores extra object members at the end of object if `bobl::options::StructAsDictionary option` is not used. And any extra members if `bobl::options::StructAsDictionary` is used. This behavior allows, to certain point, extend protocols without breaking existing implementations. To suppress such behavior `bobl::option::ExacMatch` can be used. It makes decode throw bobl::InvalidObject exception if any extra object members found during decoding.


#### Adapting types

Type can be encoded/decode as another type by specializing `bobl::Adapter`
```
namespace bobl{ 
	template<typename T, typename Enabled = boost::mpl::true_>
	class Adapter {
		using type = typename std::underlying_type<T>::type;
		T operator()(type x) const;
		type operator()(T const& x) const;

	};

} /*namespace bobl*/
```
or `bobl::bobl::Adapter`/`bobl::cbor::Adapter` if it should be protocol specific, for example:
```
	class X
	{
	public:
		explicit X(int persistent) : persistent_{ persistent } {}
		int persistent() const { return persistent_;}
	private:
		int persistent_;
		int notso_ = 0;
	};

	namespace bobl { namespace bson {
		template<>
		class Adapter<X, boost::mpl::true_> 
		{
		public:
			using type = int;
			X operator()(int x) const { return X{ x }; }
			int operator()(X const& x) const { return x.persistent(); }
		};
 	} /*namespace bson*/ } /*namespace bobl*/

```
this will encode class X as an integer

BSON complete example: [adapt.cpp](https://github.com/serge-klim/bobl/blob/master/examples/bson/adapt.cpp)  
CBOR complete example: [adapt.cpp](https://github.com/serge-klim/bobl/blob/master/examples/cbor/adapt.cpp)
