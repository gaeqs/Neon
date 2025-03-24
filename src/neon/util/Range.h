//
// Created by gaelr on 28/11/2022.
//

#ifndef NEON_RANGE_H
#define NEON_RANGE_H

#include <algorithm>
#include <ostream>

namespace neon
{
    template<class Number>
    class Range
    {
        Number _from;
        Number _to;

      public:
        Range(Number from, Number to) :
            _from(from),
            _to(to)
        {
            if (to < from) {
                _to = from;
            }
        }

        Number getFrom() const
        {
            return _from;
        }

        Number getTo() const
        {
            return _to;
        }

        Number size() const
        {
            return _to - _from;
        }

        Range<Number> operator+(const Range<Number>& other) const
        {
            if (size() == 0) {
                return other;
            }
            if (other.size() == 0) {
                return this;
            }
            return {std::min(_from, other._from), std::max(_to, other._to)};
        }

        void operator+=(const Range<Number>& other)
        {
            if (other.size() == 0) {
                return;
            }
            if (size() == 0) {
                _from = other._from;
                _to = other._to;
            } else {
                _from = std::min(_from, other._from);
                _to = std::max(_to, other._to);
            }
        }

        Range<Number> operator*(const Range<Number>& other) const
        {
            if (other.size() == 0 || size() == 0) {
                return Range<Number>(0, 0);
            }
            return {std::max(_from, other._from), std::min(_to, other._to)};
        }

        void operator*=(const Range<Number>& other)
        {
            if (size() == 0) {
                return;
            }
            if (other.size() == 0 || size() == 0) {
                _from = 0;
                _to = 0;
            } else {
                _from = std::max(_from, other._from);
                _to = std::min(_to, other._to);
                if (_to < _from) {
                    _to = _from;
                }
            }
        }

        Range<Number> operator*(const Number& other) const
        {
            return {_from * other, _to * other};
        }

        void operator*=(const Number& other)
        {
            _from *= other;
            _to *= other;
        }

        friend std::ostream& operator<<(std::ostream& os, const Range& range)
        {
            os << "[" << range._from << ", " << range._to << ")";
            return os;
        }
    };
} // namespace neon

#endif //NEON_RANGE_H
