# Variadic Friend Policy - Befriending Policies in a Variadic Policy-Based Design

## Introduction

This project demonstrates how a class inheriting multiple policies using variadic templates can <b><i>appear</i></b> to befriend all of them, while it actually leverages casts to distinct base-class types - each granting friendship to a distinct policy. It overcomes a limitation mentioned in the "Almost policy-based approach" section of the book "Hands-On Design Patterns with C++ (1st edition)", where the author claims that variadic policies cannot be befriended.

The 2nd edition of the book does not include the aforementioned section at all, likely due to the limitations of its design, such as not being able to befriend policies, define default policies or name policies.

The main advantage of this design is that it allows adding or removing features in the class that inherits the policies. I found this especially useful when designing libraries, as it gives users the flexibility to add new features without modifying the original code. Instead of waiting for an official implementation, maintaining a fork, or resorting to [template tricks to access private members](https://github.com/hiimmat/private_access), users can simply add another policy.

## How it works
This approach leverages the Curiously Recurring Template Pattern (CRTP) with a twist: although `cls` inherits all policies through variadic templates, each policy inherits `cls` with reordered template arguments,  in which it occupies the first position.

- `cls` inherits all policies through variadic templates
- Each policy inherits a distinct type of `cls`, where that policy is the first parameter in the `cls` parameter pack
- `cls` declares friendship with its first template argument
- Since each policy becomes the first template argument in one of the reordered instantiations, it gains friendship in that context, making it look like all policies are befriended by the original class

For example, when writing:

    cls<PrintPolicy, ArithmeticPolicy, ComparisonPolicy> c{5};
    c += 5;

The second line requires a call to the `+=` operator implemented by the `ArithmeticPolicy`. The `ArithmeticPolicy` casts itself (`this` pointer) to `cls<ArithmeticPolicy, PrintPolicy, ComparisonPolicy>*`, which has a different template argument ordering from the original base class. And since the `cls` class befriends the first policy, this cast allows `ArithmeticPolicy` to gain access to the base class's private members.

## Constraints
 To ensure that the object layout remains consistent when casting between these base-class types, the policies cannot introduce any data member variables, or virtual functions. This could be enforced with a compile-time check.

## Real-World implementation
I originally implemented this technique in another project called `ntensor`, which you can find here [ntensor - tensor.hpp (line 192)](https://github.com/hiimmat/ntensor/blob/master/include/tensor.hpp#L192)
