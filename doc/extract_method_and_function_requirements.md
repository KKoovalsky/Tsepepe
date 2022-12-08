# Extract method code action 

1. Shall handle selection within:

* a function, 
* an initializer list,
* class definition, when a property is initialized with some statement,
* ...

2. Shall properly figure out the return type of the statement.
3. Resharper seeing a single variable which is mutated, and defined above the selected block, will make that variable
be returned within the new function/method, as a reference.
4. Shall capture variables that are used inside the selected block, and declared outside of it, to properly define
the parameters of the function.
5. Shall not put the class members to the parameter list.
6. Shall decide whether this is can be a 'const' function or not (when the properties are mutated).
7. Shall create the declaration, if the current selection is within an out-of-line definition, or even prompt the user
in what section shall it be put.
8. For a declaration being a definition as well, no out-of-line definition shall be generated.
9. For statements inside a class definition, e.g.: (`/*[[*/` and `/*]]*/` denote the visual selection)

```
class SomeClass
{
// ...
    unsigned property{/*[[*/ 2*3*1000 /*]]*/};
};
```
when a corresponding source file is provided, the user shall be asked whether the definition shall be out-of-line.
10. Support "hoisting": when a variable is declared within the visual block, and then it is used outside of it, then
the declaration should be left as is, and captured as a parameter. (Resharper within CLion doesn't handle that well
at the stage of writing this: the variables' initial values are removed).

# Extract function code action 

Similar to above except that there are no properties in a free-function, so everything must be captured by a parameter.
