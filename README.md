# visitor-double-dispatch

This is a very simple demo of double dispatch combined with the visitor pattern
in C++. This code is inspired by [a very clear answer][1] given by [Oleksiy][2] at
codereview.stackexchange.com.

## Motivation

The motivation here is to deal with each item from some collection of objects,
all derived from some base. The method of dealing with these objects depends on
the object, and the objects themselves don't know how to do it.

As an example, let's consider a simple viewer for simple data objects, perhaps
some sort of debugger or analyser. Some library produces a container full of
`DataObjects`, which is an abstract base class. We have `StringObjects`,
`IntegerObjects` and `FloatObjects`, each of which have their own special
properties.

There is no practical way (say) that simple polymorphism by virtual
functions can be used to indicate to a renderer how to render a `DataObject` in
generic terms - the renderer has to handle each derived class separately and
access the special features of each one as needed.

We wish to take this container of abstract objects and produce a
tailored rendering for each one:

    String:     "Hello"  (utf-8)
    Integer:    16  (32 bits)
    Float:      3.14  (ieee754)

We want to do this without having to imbue our `DataObjects` with any knowledge
of how to do this rendering, because another program might choose to render them
differently, and rendering is not the job of the `DataObject`.

This is summed up well by a comment on Oleksiy's answer by John Neuhaus:

> Virtual functions are the way to do it if you want the derived classes to do
> different things, but the Visitor pattern is what you want if you want to do
> different things to them. ... The animal doesn't pet itself, but you don't rub
> a Cow's tummy like a Dog.

(This example differs in that the vistor can't modify the data object, but that
can be changed, see the Variations section.)

### 1D double-dispatch

Many examples of double dispatch involve a "2D" array of combinations of double
dispatch. For example, how _n_ character classes interact with _m_ monster classes
produces _n*m_ variations (or half if the interactions are symmetric). Every
time you add a character, you need to update _m_ monsters, and adding a monster
needs _m_ character updates.

This example is "1D" double-dispatch, where we have _n_ classes, but there is a
single vistation of each one, avoiding the combinatorial explosion that often
signals that double dispatch is the wrong tool.

Moreover, it's explicit that the vistor implementations (the renderer here)
should do something special for each one. If the implementation of the object
classes is such that you can do it all with the polymorphic interfaces, you
might never even need to downcast to the specific derived class, and this
technique is pointless.

## Technique

The method use here is to:

- Define a class `DataObjectVisitor`, which has a virtual method defined for
  each derived class of `DataObject`. This virtual method is called `visit` and
  takes a const reference to the `DataObject` in question. This is because the
  renderer does not modify the data. This is a non-const function, because the
  act of rendering might affect the internal state of the renderer.
- Add a pure virtual function `accept` to the `DataObject` base class, which
  takes a reference to a `DataObjectVistor`. The reference is non-const and the
  function is const because, as before, the renderer might have state to change,
  but rendering data never alters the data.
- For each derived class, override `accept` and call `visit` on the reference to
  the vistor, using `*this` as the argument.
- Implement a `DataObjectVisitor` which does what you want in each `visit`
  function.
- For each pointer/reference-to-base object you have, call `accept` on it,
  passing in your visitor instance.

The total cost of the dispatch is two virtual function calls - one to the
`DataObject::accept` and one to `DataObjectVisit::visit`. Compared to
`dynamic_cast` in an if/elsif chain, that's pretty good!

## How to use

    make
    ./visitor-double-dispatch

## Variations

### Constness

As written, the visitor cannot modify the `DataObject`. This doesn't have to be
true - for example if your use case was a game where the vistor was a Hero and
the derived classes were monsters, and the Hero gives and takes damage from
various kinds of monsters.

To change this, remove `const` from the references taken by the `visit`
functions, and remove `const` from the cv-specifier of the `accept` functions.
Now your vistor can modify the visitees.

You could even make the `DataObjectVisitor` reference in the `accept` functions
const, and then the renderer cannot be changed by the act of rendering a
`DataObject`.

### Implementation of `accept` functions

You can inline the definitions of the overriding accept functions (Which may
save you a .cpp file), but you have to move `DataObjectVistor` above the
`DataObject` declarations, and therefore you will need to forward-declare each
derived class before that.

### Wrapping of third party types

If you can't directly modify the `DataObject` types, you can still use this
technique, but you'll have to wrap each one on your own class which provides the
`accept` function.

### Default behaviour

If your `DataObjectVistor` implementations (the renderer in this example)
doesn't implement every `visit` overload, you can define a default action to
happen for each one in the base class. This can even be set by the end user by
providing an inteface on the visitor base class for setting the default
behaviour.

Alternatively, make them all pure-virtual and your force the implementation to
handle them all. Or a subset can be made pure virtual.

## Limitations

The primary limitation is that you must know all the derived types that you can
vist at the time of definition of the vistor base class. If you don't have this
(e.g. objects are defined in plugins or something where the core doesn't see the
definition), you'll have to make do with polymorphism via the base class pointer
and not a lot else. This can still be flexible, as the `DataObject` class can
provide a plugin-like interface where the renderer can query for hints on how to
render the class. It's more work than this pattern, though.

[1]: https://codereview.stackexchange.com/questions/56363/casting-base-to-derived-class-according-to-a-type-flag
[2]: https://codereview.stackexchange.com/users/47784/oleksiy
