# JSON Parser

> Parser for simplified JSON.

Object keys are ASCII upper- or lower-case characters.
Values are integers, arrays or objects (no strings or floats).

The EBNF:

```ebnf
(* toplevel elements *)
value   = blank, ( integer | array | object ), blank ;
integer = [ '-' ], digits  | '0' ;
array   = '[', valist, ']' | '[]' ;
object  = '{', kvlist, '}' | '{}' ;

(* compound data *)
valist  = value,  { ',', value } ;
kvlist  = kvpair, { ',', kvpair } ;
kvpair  = blank, key, blank, ':', value ;

(* lexemes *)
digits  = nonzero, { digit } ;
nonzero = '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' ;
digit   = '0' | nonzero ;
key     = keychar, { keychar } ;
keychar = ? ASCII upper- or lower-case alphabetical character ? ;
blank   = { ? ASCII space, tab or newline character ? } ;
```

Calling `item_at` with integer on `json_type::object` returns element in key-alphabetical order.

## Example

```cpp
#include "parser.hpp"

#include <cassert>

auto json = json_parse("{ array: [1, 2, 3], object: { number: 42 } }");
assert(json->type() == json_type::object);

assert(json->item_at("array").type() == json_type::array);
assert(json->item_at("array").item_at(0).int_value() == 1);
assert(json->item_at("array").item_at(1).int_value() == 2);
assert(json->item_at("array").item_at(2).int_value() == 3);

assert(json->item_at("object").type() == json_type::object);
assert(json->item_at("object").item_at("number").type() == json_type::integer);
assert(json->item_at("object").item_at("number").int_value() == 42);
```
