# TinyJulia

## Variables
- ### Types
    - Integer: a positive or negative whole number capable of being represented with 32 bits.
    - Booleans: the keywords `true` and `false` with their respective meaning.
- ### Declaration (can only be done once)
    - <variable name 1\>`::Int`: Declares an Integer value.
    - <variable name 2\>`::Bool`: Declares a Boolean value.
    - <variable name 3\>`::Array{Int}(`<size\>`)`: Declares a Integer array of size <size\>.
    - <variable name 4\>`::Array{Bool}(`<size\>`)`: Declares a Boolean array of size <size\>.

    - <variable name 1\>`::Int =` <Integer or Boolean\>: Declares and initializes an Integer value.
    - <variable name 2\>`::Bool =` <Boolean\>: Declares and initializes an Boolean value.
    - <variable name 3\>`::Array{Int} = [<value-1\>, <value-2\>, <value-n\>]`: Declares a Integer array of size n.
    - <variable name 4\>`::Array{Bool} = [<value-1\>, <value-2\>, <value-n\>]`: Declares a Boolean array of size n.

- ### Usage
    - <variable name 1\> : Gets the Integer value.
    - <variable name 2\> : Gets the Boolean value.
    - <variable name 3\>`[`<offset\>`]` : Gets the Integer value at offset <offset\>; Index starts from 1.
    - <variable name 4\>`[`<offset\>`]` : Gets the Boolean value at offset <offset\>; Index starts from 1.
    
    - <variable name 1\> `=` <Integer or Boolean\>: Sets the Integer value.
    - <variable name 2\> `=` <Boolean\>: Sets the Boolean value.
    - <variable name 3\>`[`<offset\>`]` : Sets the Integer value at offset <offset\>; Index starts from 1.
    - <variable name 4\>`[`<offset\>`]` : Sets the Boolean value at offset <offset\>; Index starts from 1.

## Comments
- ### Variants
    - `#`: single line comment. Ends when reaching new line or end of file.
    - `#=` <comment\> `=#`: multiline comment. Comment between start symbol(`#=`) and end symbol (`=#`)

## Printing
- ### Instructions
    - `print`: prints parameters
    - `println`: prints parameters and new line at the end
- ### Parameters (separaed by comas)
    - Literal: a string literal encapsulated by double quoutes, may contain escape characters. For example:  `"Hello World!"`.
        - #### Escape characters:
            - `\\` : Adds a `\` to the string
            - `\"` : Adds a `"` to the string
            - `\t` : Adds a tabulation to the string
            - `\n` : Adds a new line to the string
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
    - <Integer or Boolean Value\> <Operator\> <Integer or Boolean Value\> : Returns result of operation
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
    - <Integer or Boolean Value\> <Operator\> <Integer or Boolean Value\> : Returns result of operation
- ### Unary Operators
    - `!` : Negation of
 - ### Unary Operators Usage
    - <Operator\> <Boolean Value\> : Returns result of operation
 - ### Type
    - Any operation will result in a Boolean

## Bitwise Operations
- ### Binary Operators
    - `|` : OR
    - `&` : AND
    - `<<` : Arithmetic Shift Left 
    - `>>` : Arithmetic Shift Right
 - ### Binary Operators Usage
    - <Integer or Boolean Value\> <Operator\> <Integer or Boolean Value\> : Returns result of operation
 - ### Uniary Operators
    - `~` : NOT
 - ### Uniary Operators Usage
    - <Operator\> <Integer or Boolean Value\> : Returns the `false` if true and `true` if false.
 - ### Type
    - `|` : Boolean if both values are boolean, else Integer
    - `&` : Boolean if both values are boolean, else Integer
    - `<<` : Integer
    - `>>` : Integer
    - `~` : Boolean if value is boolean, else Integer

