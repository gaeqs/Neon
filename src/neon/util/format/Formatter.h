//
// Created by gaeqs on 6/11/25.
//

#ifndef NEON_FORMATTER_H
#define NEON_FORMATTER_H

namespace neon
{

    template<typename Input, typename Output>
    class Formatter
    {
      public:
        virtual ~Formatter() = default;

        virtual Output format(const Input& input) = 0;
    };

} // namespace neon

#endif // NEON_FORMATTER_H
