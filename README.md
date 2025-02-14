# Variadic Friend Policy - Befriending Policies in a Variadic Policy-Based Design

This project demonstrates how a class inheriting multiple policies using variadic templates can befriend all of them. It overcomes a limitation mentioned in the "Almost policy-based approach" section of the book "Hands-On Design Patterns with C++ (1st edition)", where the author claims that variadic policies cannot be befriended. However, this project shows that it is indeed possible.

The 2nd edition of the book does not mention this approach at all. I suspect this is due to the perceived limitations of this design, such as not being able to befriend policies, define default policies or name policies. However, I found a solution to the friendship issue, and I believe the other limitations can be addressed as well.

The main advantage of this design is that it allows adding or removing features in the class that inherits the policies. I found this especially useful when designing libraries, as it gives users the flexibility to add new features without modifying the original code. Instead of waiting for an official implementation, maintaining a fork, or resorting to [template tricks to access private members](https://github.com/hiimmat/private_access), users can simply add another policy.

This works due to the `Curiously Recurring Template Pattern (CRTP)`. The `cls` class inherits each policy, and each policy, in turn, inherits `cls`, but with a different policy ordering (which results in a distinct base class type). This reordering ensures that each policy becomes the first policy in one instantiation. And since `cls` befriends the first policy in the parameter pack, this effectively grants friendship to all policies.

For example, when writing:
    cls<PrintPolicy, ArithmeticPolicy, ComparisonPolicy> c{5};

If `ArithmeticPolicy` needs to call the `+=` operator, it casts itself to `cls<ArithmeticPolicy, PrintPolicy, ComparisonPolicy>`, which has a different ordering than the original instantiation. However, since `cls` befriends the first policy, `ArithmeticPolicy` gains access to its private members.

I originally implemented this technique in another project called `ntensor`, which you can find here [ntensor - tensor.hpp (line 192)](https://github.com/hiimmat/ntensor/blob/master/include/tensor.hpp#L192)
