#ifndef _OPTIONAL_UTILS_H_
#define _OPTIONAL_UTILS_H_

namespace NeuroEvo {

//Check whether type T is an optional
template <typename T, typename Enable = void>
struct is_optional : std::false_type {}

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {}

} // namespace NeuroEvo

#endif
