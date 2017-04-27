#include <iostream>
#include <tuple>
#include <string>
#include <type_traits>

namespace clayer {
    namespace util {
        template<class Tuple, std::size_t N, typename std::enable_if< (N<=1) >::type * = nullptr>
        std::ostream& to_string(std::ostream& ss, const Tuple& t) {
            return ss << std::get<N-1>(t) ;
        }

        // template<class Tuple, std::size_t N>
        template<class Tuple, std::size_t N, typename std::enable_if< (N>=2) >::type * = nullptr>
        std::ostream& to_string(std::ostream& ss, const Tuple& t) {
            return to_string<Tuple, N-1>(ss, t) << "," << std::get<N-1>(t);
        }

        template<class... Args>
        std::ostream& to_string(std::ostream& ss, const std::tuple<Args...>& t) {
            ss << "(";
            // ss << (sizeof...(Args)) << ":";
            to_string<decltype(t), sizeof...(Args)>(ss, t);
            return ss << ")";
        }

    }
}

