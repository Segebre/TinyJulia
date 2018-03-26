# TinyJulia

## Variables
- ### Types
    - Integer: a positive or negative whole number capable of being represented with 32 bits.
    - Booleans: the keywords `true` and `false` with their respective meaning.
- ### Usage
    - \<variable name 1\>`::Int`: Declares an int value.
    - \<variable name 2\>`::Bool`: Declares an int value.

    - \<variable name 1\>`::Int = <Integer>`: Declares and initializes an integer value.
    - \<variable name 2\>`::Bool = <Boolean>`: Declares and initializes an boolean value.

    - \<variable name 1\> : Gets the integer value.
    - \<variable name 2\> : Gets the boolean value.
    
    - \<variable name 1\> `= <Integer>`: Sets the integer value.
    - \<variable name 2\> `= <Integer>`: Sets the boolean value.

## Comments
- ### Variants
    - `#`: single line comment. Ends when reaching new line or end of file.
    - `#=` \<comment\> `=#`: multiline comment. Comment between start symbol(`#=`) and end symbol (`=#`)

## Printing
- ### Instructions
    - `print`: prints parameters
    - `println`: prints parameters and new line at the end
- ### Parameters (separaed by comas)
    - Literal: a string literal encapsulated by double quoutes, may contain escape characters. For example:  `"Hello World!"`.
        - #### Escape characters:
            - `\\`
            - `\"`
            - `\t`
            - `\n`
    - Integers: any expression resulting in an integer.
    - Booleans: any expression resulting in a boolean.

## Expressions
- ### Operators
    - `+` : Addition
    - `-` : Subtraction
    - `*` : Multiplication
    - `/` : Division
    - `%` : Modularization
    - `^` : Exponentiation
 - ### Usage
    - \<Integer or Boolean Value\> \<Operator\> \<Integer or Boolean Value\> : Returns result of operation
 - ### Type
    - Any operation will result in an Integer

## Conditions
- ### Binary Operators
    - `>` : Greater than
    - `<` : Less than
    - `==` : Equal to
    - `>=` : Greater than or Equal to
    - `<=` : Less than or Equal to
    - `!=` : Not Equal to
 - ### Binary Operators Usage
    - \<Integer or Boolean Value\> \<Operator\> \<Integer or Boolean Value\> : Returns result of operation
- ### Unary Operators
    - `!` : Negation of
 - ### Unary Operators Usage
    - \<Operator\> \<Integer or Boolean Value\> : Returns result of operation
 - ### Type
    - Any operation will result in a Boolean

## Bitwise Operations
- ### Operators
    - `|` : OR
    - `&` : AND
    - `~` : NOT
    - `<<` : Arithmetic Shift Left 
    - `>>` : Arithmetic Shift Right
 - ### Usage
    - \<Integer or Boolean Value\> \<Operator\> \<Integer or Boolean Value\> : Returns result of operation
 - ### Type
    - Any operation will result in a Boolean

