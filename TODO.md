# TODO

## interval.hpp

### RingInterval

- inplace complement.
- intersection check (methods for Interval and RingInterval).
- hash and equality.
- iteration.
- `[[nodiscard]] std::optional<Interval> to_interval() const noexcept;`
- make empty/full/less/less_or_equal/equal/greater_or_equal/greater + not_equal.
- make_range / make_complement_range.
- safe_cast + exact_cast? with asserts on Traits range and ordering?

### Interval

- intersection operator (`&`) + inplace intersection operator (`&=`).
- intersection check (methods for Interval and RingInterval).
- iteration.
- hash and equality.
- implicit cast to RingInterval.
- make empty/full/less/less_or_equal/equal/greater_or_equal/greater.
- make_range.
