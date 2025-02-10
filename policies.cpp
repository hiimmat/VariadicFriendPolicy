#include <iostream>

inline namespace internal {

template <typename...>
struct first_policy;

/*
 * Specialization of the first_policy struct to extract the first policy from a list of policies.
 * Template Parameters:
 * @tparam Policy: The first policy identified in the list of policies.
 * @tparam PolicyArgs: Template arguments associated with the first policy.
 * @tparam Policies: The remaining policies in the list.
 */
template <template <typename...> typename Policy, typename... PolicyArgs, typename... Policies>
struct first_policy<Policy<PolicyArgs...>, Policies...> {
  using type = Policy<PolicyArgs...>;
};

/*
 * Determines the first policy inherited by an object.
 * Template Parameters:
 * @tparam Policies: A list of policies from which the first policy is selected.
 */
template <typename... Policies>
using first_policy_t = typename first_policy<Policies...>::type;

template <typename, typename>
struct append;

/*
 * Specialization of the append struct that adds a new policy to an object.
 * Template Parameters:
 * @tparam Policy: The policy that needs to be inherited by the object.
 * @tparam PolicyArgs: Policy's template arguments.
 * @tparam T: The type of the object that inherits the policies.
 * @tparam Policies: The list of policies currently inherited by the object.
 */
template <template <typename...> typename Policy, typename... PolicyArgs,
          template <template <typename...> typename...> typename T,
          template <typename...> typename... Policies>
struct append<Policy<PolicyArgs...>, T<Policies...>> {
  using type = T<Policy, Policies...>;
};

/*
 * Specialization of the append struct that adds a new policy to an object.
 * Template Parameters:
 * @tparam T: The type of the object inheriting the policies.
 * @tparam Policies: The list of policies currently inherited by the object.
 * @tparam Policy: The policy to be added to the object.
 * @tparam PolicyArgs: Policy's template arguments.
 */
template <template <template <typename...> typename...> typename T,
          template <typename...> typename... Policies, template <typename...> typename Policy, typename... PolicyArgs>
struct append<T<Policies...>, Policy<PolicyArgs...>> {
  using type = T<Policies..., Policy>;
};

/*
 * Specialization of the append struct that concatenates the policies of two objects into a new object type.
 * Template Parameters:
 * @tparam T: The base type representing both objects.
 * @tparam FirstPoliciesList: The policies inherited by the first object.
 * @tparam SecondPoliciesList: The policies inherited by the second object.
 */
template <template <template <typename...> typename...> typename T,
          template <typename...> typename... FirstPoliciesList, template <typename...> typename... SecondPoliciesList>
struct append<T<FirstPoliciesList...>, T<SecondPoliciesList...>> {
  using type = T<FirstPoliciesList..., SecondPoliciesList...>;
};

/*
 * Adds a policy to the specified object type.
 * Template Parameters:
 * @tparam Policy: The policy to be added.
 * @tparam T: The type of the object to which the policy is added.
 */
template <typename Policy, typename T>
using append_t = typename append<Policy, T>::type;

template <typename, typename>
struct erase;

/*
 * Specialization of the erase struct to remove the first policy from an object.
 * Template Parameters:
 * @tparam Policy: The first policy associated with the object.
 * @tparam PolicyArgs: Template arguments for the first policy.
 * @tparam T: The type representing the object.
 * @tparam Policies: The remaining policies associated with the object.
 */
template <template <typename...> typename Policy, typename... PolicyArgs,
          template <template <typename...> typename...> typename T,
          template <typename...> typename... Policies>
struct erase<Policy<PolicyArgs...>, T<Policy, Policies...>> {
  using type = T<Policies...>;
};


/*
 * Specialization of the erase struct that removes the first policy and returns the object type.
 * Template Parameters:
 * @tparam Policy: The first policy inherited by the object.
 * @tparam PolicyArgs: Template arguments associated with the first policy.
 * @tparam T: The type representing the object.
 * @tparam Policies: The remaining policies inherited by the object.
 */
template <template <typename...> typename Policy, typename... PolicyArgs,
          template <template <typename...> typename...> typename T,
          template <typename...> typename... Policies>
struct erase<Policy<PolicyArgs...>, T<Policies...>> {
  using type = T<Policies...>;
};

/*
 * Specialization of the erase struct for the case where the policy to be removed is not found.
 * In this scenario, the recursion continues until all policies are checked or the target policy is found.
 * Template Parameters:
 * @tparam Policy: The current policy being checked.
 * @tparam PolicyArgs: Current policy template arguments.
 * @tparam T: The type representing the object.
 * @tparam FirstPolicy: The first policy inherited by the object.
 * @tparam Policies: The remaining policies inherited by the object.
 */
template <template <typename...> typename Policy, typename... PolicyArgs,
          template <template <typename...> typename...> typename T,
          template <typename...> typename FirstPolicy, template <typename...> typename... Policies>
struct erase<Policy<PolicyArgs...>, T<FirstPolicy, Policies...>> {
  using type = typename append<T<FirstPolicy>,
                               typename erase<Policy<PolicyArgs...>, T<Policies...>>::type>::type;
};

/*
 * Removes a policy from the specified object type.
 * Template Parameters:
 * @tparam Policy: The policy to be removed.
 * @tparam T: The type of the object from which the policy is removed.
 */
template <typename Policy, typename T>
using erase_t = typename erase<Policy, T>::type;

template <typename>
struct reorder_as_first_policy;

/*
 * Reorders the policies inherited by the object, placing a specified policy at the beginning of the sequence.
 * Template Parameters:
 * @tparam Policy: The policy to be moved to the first position in the policy list.
 * @tparam T: The type of the object.
 * @tparam Policies: The sequence of policies inherited by the object.
 */
template <template <typename...> typename Policy,
          template <template <typename...> typename...> typename T,
          template <typename...> typename... Policies>
struct reorder_as_first_policy<Policy<T<Policies...>>> {
  using type = Policy<append_t<Policy<T<Policies...>>,
                               erase_t<Policy<T<Policies...>>, T<Policies...>>>>;
};

/*
 * Reorders a sequence of policies by placing the specified policy at the beginning of the sequence.
 * Template Parameters:
 * @tparam Policy: The policy to be moved to the first position in the sequence.
 */
template <typename Policy>
using reorder_as_first_policy_t = typename reorder_as_first_policy<Policy>::type;

}  // namespace internal


template <template <typename> typename... Policies>
class cls : public reorder_as_first_policy_t<Policies<cls<Policies...>>>... {
 private:
 int x{0};
  friend first_policy_t<Policies<cls<Policies...>>...>;

 public:
  explicit cls(int x) : x{x} {}
};

template<typename T>
struct PrintPolicy {
    void print() const {
        std::cout << static_cast<const T*>(this)->x << std::endl;
    }
};

template<typename T>
struct ArithmeticPolicy {
    [[nodiscard]] int operator+() const { return +static_cast<const T*>(this)->x; }

    [[nodiscard]] int operator-() const { return -static_cast<const T*>(this)->x; }

    [[nodiscard]] int operator+(int val) const { return static_cast<const T*>(this)->x+val; }

    [[nodiscard]] int operator-(int val) const { return static_cast<const T*>(this)->x-val; }

    [[nodiscard]] int operator*(int val) const { return static_cast<const T*>(this)->x*val; }

    [[nodiscard]] int operator/(int val) const { return static_cast<const T*>(this)->x/val; }
    
    [[nodiscard]] int operator%(int val) const { return static_cast<const T*>(this)->x%val; }

    void operator+=(int val) { static_cast<T*>(this)->x+=val; }

    void operator-=(int val) { static_cast<T*>(this)->x-=val; }

    void operator*=(int val) { static_cast<T*>(this)->x*=val; }

    void operator/=(int val) { static_cast<T*>(this)->x/=val; }

    void operator%=(int val) { static_cast<T*>(this)->x%=val; }
};

template<typename T>
struct ComparisonPolicy {
    [[nodiscard]] bool operator==(int val) const noexcept { return static_cast<const T*>(this)->x == val; }

    [[nodiscard]] auto operator<=>(int val) const noexcept { return static_cast<const T*>(this)->x <=> val; }

};


int main() {
    cls<PrintPolicy, ArithmeticPolicy, ComparisonPolicy> c{5};
    c.print();
    c += 3;
    c.print();
    std::cout << (c == 8) << std::endl;

    return 0;
}
