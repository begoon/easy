# EASY grammar

## COMPILATIONS

```bnf
<compilation> ::= <program segment>
                | <compilation> <program segment>
<program segment> ::= <main program>
                    | <external procedure>
```

## SELECTION

```bnf
<selection statement> ::= <simple selection>
                        | <label> <simple selection>

<simple selection> ::= <selection head> <selection body> <selection end>
<selection head> ::= SELECT <expression> OF
<selection body> ::= <cast list> 
                   | <case list> <escape case>
<selection end> ::= END SELECT ;  END SELECT <identifier> ;
<case list> ::= <case> 
              | <case list> <case>
<case> ::= <case head> <case body>
<case head> ::= CASE <selector>
<selector> ::= <selector head> )
<selector head> ::= ( <expression> 
                  | <selector head> , <expression>
<escape case> ::= <escape head> <case body>
<escape head> ::= OTHERWISE :
<case body> ::= <segment body>
```

## REPEAT and REPENT

```bnf
<repeat statement> ::= REPEAT <identifier> ;
<repent statement> ::= REPENT <identifier> ;
```

## INPUT and OUTPUT

```bnf
<input statement> ::= INPUT <input list> ;
<input list> ::= <variable> 
               | <input list> , <variable>

<output statement> ::= OUTPUT <output list> ;
<output list> ::=  <expression> 
                | <output list> , <expression>
```

## NULLS and LABELS

```bnf
<null statement> ::= ;
<label> ::= <identifier> :
```

## EXPRESSIONS

```bnf
<expression> ::= <expression one> 
               | <expression> "|" <expression one>
               | <expression> XOR <expression one>

<expression one> ::= <expression two>
                   | <expression one> & <expression two>

<expression two> ::= <expression three>
                   | NOT <expression three>

<expression three> ::= <expression four>
                     | <expression three> <relation> <expression four>

<expression four> ::= <expression five>
                    | <expression four> || <expression five>

<expression five> ::= <expression six>
                    | <expression five> <adding operator> <expression six>
                    | <adding operator> <expression six>

<expression six> ::= <expression seven>
                   | <expression six> <multiplying operator> <expression seven>

<expression seven> ::= FLOOR ( <expression> )
                     | LENGTH ( <expression> )
                     | SUBSTR ( <expression>, <expression>, <expression> )
                     | CHARACTER ( <expression> )
                     | NUMBER ( <expression> )
                     | FLOAT ( <expression> )
                     | FIX ( <expression> )
                     | <expression eight>

<expression eight> ::= <variable>
                     | <constant>
                     | <function reference>
                     | ( <expression> )
```

## VARIABLES

```bnf
<variable> ::= <identifier>
             | <variable> . <identifier>
             | <variable> [ <expression> ]
```

## CONSTANTS

```bnf
<contant> ::= <integer contant> 
            | <real constant> 
            | <boolean constant> 
            | <string constant>
<boolean constant> ::= TRUE 
                     | FALSE
```

## FUNCTION CALLS

```bnf
<function reference> ::= <function identifier> ()
                       | <function identifier> <actual argument list>
<function identifier> ::= <identifier>
```

## LEXICAL ITEMS

```bnf
<relation> ::= < | > | = | <= | >= | <>
<adding operator> ::= + | -
<multiplying operator> ::= * | / | MOD
```
