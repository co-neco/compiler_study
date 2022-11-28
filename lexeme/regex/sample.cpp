#include "sample.hpp"

namespace lexeme {
namespace regex {
namespace sample {
    
    std::string RegexSample::sample1 = R"xx((a|b)*abb)xx";
    std::string RegexSample::sample2 = R"xx((($|a)b*)*)xx";
}
}
}
