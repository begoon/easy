# EASY grammar

## COMPILATIONS

```bnf
<compilation> ::= <program segment>
                | <compilation> <program segment>
<program segment> ::= <main program>
                    | <external procedure>
```

## PROGRAMS

```bnf
<main program> ::= <program head> <program body> <program end>
<program head> ::= PROGRAM <identifier> :
<program body> ::= <segment body>
<program end> ::= END PROGRAM <identifier> ;
```

## EXTERNAL PROCEDURES

```bnf
 <external procedure> ::= <external subprogram>
                        | <external function>

 <external subprogram> ::= <external subprogram head> :
                           <external subprogram body>
                           <external subprogram end>

 <external function> ::= <external function head> :
                         <external function body>
                         <external function end>

 <external subprogram head> ::= EXTERNAL PROCEDURE
                                <external procedure name>

 <external function head> ::= EXTERNAL FUNCTION
                              <external procedure name>
                              <external type>

 <external procedure name> ::= <identifier>
                             | <identifier> <external parameter list>

 <external parameter list> ::= <external parameter head> >
 <external parameter head> ::= <external parameter>
                             | <external parameter head> , <external parameter>

 <external parameter> ::= <identifier> <external type>
                        | <identifier> <external type> NAME

 <external type> ::= <basic type>
 <external subprogram body> ::= <segment body>
 <external function body> ::= <segment body>

 <external subprogram end> ::= END EXTERNAL PROCEDURE <identifier> ;
 <external function end> ::= END EXTERNAL FUNCTION <identifier> ;
```

## SEGMENTS

```bnf
<segment body> ::= <type definition part> <variable declaration part>
                   <procedure definition part>
                   <executable statement part>

<type definition part> ::= 
                        | <type definition part> <type definition>

<variable declaration part> ::= 
                              | <variable declaration part> <variable declaration>

<procedure definition part> ::= 
                             | <procedure definition part> <procedure definition>

<executable statement part> ::= <executable statement>
                            | <executable statement part> <executable statement>
```

## TYPES

```bnf
<type definition> ::= TYPE <identifier> IS <type> ;
<type> ::= <basic type>
         | <arrayed type>
         | <structured type>
         | <type identifier>

<basic type> ::= INTEGER
               | REAL
               | BOOLEAN
               | STRING

<arrayed type> ::= ARRAY <bounds> OF <type>

<bounds> ::= [ <bounds expression> ]
           | [ <bounds expression> : <bounds expression> ]

<bounds expression> ::= <expression>

<structured type> ::= STRUCTURE <field list> END STRUCTURE

<field list> ::= <field>
               | <field list> , <field>

<field> ::= FIELD <identifier> IS <type>

<type identifier> ::= <identifier>
```

## DECLARATIONS

```bnf
<variable declaration> ::= DECLARE <declared names> <type> ;

<declared names> ::= <identifier>
                   | <declared names list>

<declared names list> ::= <identifier>
                        | <declared names list> , <identifier>
```

## INTERNAL PROCEDURES

```bnf
<procedure definition> ::= <subprogram definition>
                         | <function definition>
                         | <external subprogram definition>
                         | <external function definition>

<subprogram definition> ::= <subprogram head> : <subprogram body> <subprogram end>

<function definition> ::= <function head> : <function body> <function end>

<external subprogram definition> ::= <external subprogram head> ;

<external function definition> ::= <external function head> ;

<subprogram head> ::= PROCEDURE <procedure name>

<function head> ::= FUNCTION <procedure name> <type>

<subprogram body> ::= <segment body>

<function body> ::= <segment body>

<subprogram end> ::= END PROCEDURE <identifier> ;

<function end> ::= END FUNCTION <identifier> ;

<procedure name> ::= <identifier>
                   | <identifier> <internal parameter list>

<internal parameter list> ::= <internal parameter head> )
                            | <internal parameter head> , <internal parameter>

<internal parameter head> ::= <internal parameter>
                            | <internal parameter head> , <internal parameter>

<internal parameter> ::= <identifier> <type>
                       | <identifier> <type> NAME
```

## EXECUTABLE STATEMENTS

```bnf
<executable statement> ::= <assignment statement>
                         | <call statement>
                         | <return statement>
                         | <exit statement>
                         | <conditional statement>
                         | <compound statement>
                         | <iteration statement>
                         | <selection statement>
                         | <repeat statement>
                         | <repent statement>
                         | <input statement>
                         | <output statement>
                         | <null statement>
```

## ASSIGNMENTS

```bnf
<assignment statement> ::= SET <target list> <expression> ;

<target list> ::= <target>
                | <target list> <target>

<target> ::= <variable> <replace>

<replace> ::= :=
```

## PROCEDURE CALLS

```bnf
<call statement> ::= CALL <procedure reference> ;

<procedure reference> ::= <procedure identifier>
                        | <procedure identifier> <actual argument list>

<procedure identifier> ::= <identifier>

<actual argument list> ::= ( <actual argument head> )

<actual argument head> ::= <expression>
                         | <actual argument head> , <expression>
```

## RETURNS

```bnf
<return statement> ::= RETURN ;
                     | RETURN <expression> ;
```

## EXITS

```bnf
<exit statement> ::= EXIT ;
```

## CONDITIONALS

```bnf
<conditional statement> ::= <simple conditional statement>
                          | <label> <simple conditional statement>

<simple conditional statement> ::= <conditional clause> <true branch> FI ;
                                 | <conditional clause> <true branch> <false branch> FI ;

<conditional clause> ::= IF <expression>

<true branch> ::= THEN <conditional body>

<false branch> ::= <else> <conditional body>

<else> ::= ELSE

<conditional body> ::= <segment body>
```

## COMPOUNDS

```bnf
<compound statement> ::= <simple compound>
                       | <label> <simple compound>

<simple compound> ::= <compound head> <compound body> <compound end>

<compound head> ::= BEGIN

<compound body> ::= <segment body>

<compound end> ::= END ;
                 | END <identifier> ;
```

## ITERATIONS

```bnf
<iteration statement> ::= <simple iteration statement>
                        | <label> <simple iteration statement>

<simple iteration statement> ::= <iteration head> <iteration body> <iteration end>

<iteration head> ::= <for> <iteration target> <control> DO

<iteration body> ::= <segment body>

<iteration end> ::= END FOR ;
                  | END FOR <identifier> ;

<for> ::= FOR

<iteration target> ::= <variable> <replace>

<control> ::= <step control>
            | <while control>

<step control> ::= <initial value> <step>
                 | <initial value> <limit>
                 | <initial value> <step> <limit>

<initial value> ::= <expression>

<step> ::= BY <expression>

<limit> ::= TO <expression>

<while control> ::= WHILE <expression>
```

### ITERATION EXECUTION

```algol
    SET <iteration target> := <initial value>;

top:
    IF <while control> exists
        THEN SET stoploop := NOT <while control>;
        ELSE SET stoploop := FALSE; FI;
    IF stoploop THEN GOTO end; FI;

    IF <limit> exists & (<iteration target> > <limit>)
        THEN GOTO end; FI;

    <iteration body>

    IF <step> exists
        THEN SET stepvalue := <step>;
        ELSE SET stepvalue := 1; FI;

    SET <iteration target> := <iteration target> + stepvalue;

    GOTO top;
end: ...
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
<relation> ::= < 
              | > 
              | = 
              | <= 
              | >= 
              | <>
<adding operator> ::= + 
                    | -
<multiplying operator> ::= * 
                         | / 
                         | MOD
```
