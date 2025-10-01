w      PROGRAM:Life|w                                          IntegerType  <'('|SYMBOL|tests/life/test.easy:4:11>                
h      PROGRAM:Life|h                                          IntegerType  <'('|SYMBOL|tests/life/test.easy:4:11>                
field  PROGRAM:Life|field                                      AliasType    <field|IDENT|tests/life/test.easy:5:11>               
x      PROGRAM:Life|x                                          IntegerType  <'('|SYMBOL|tests/life/test.easy:7:11>                
y      PROGRAM:Life|y                                          IntegerType  <'('|SYMBOL|tests/life/test.easy:7:11>                
i      PROGRAM:Life|i                                          IntegerType  <i|IDENT|tests/life/test.easy:8:11>                   
x      PROGRAM:Life.FUNCTION:valid|x                           IntegerType  <x|IDENT|tests/life/test.easy:10:18>                  
y      PROGRAM:Life.FUNCTION:valid|y                           IntegerType  <y|IDENT|tests/life/test.easy:10:29>                  
x      PROGRAM:Life.FUNCTION:neighbours|x                      IntegerType  <x|IDENT|tests/life/test.easy:14:23>                  
y      PROGRAM:Life.FUNCTION:neighbours|y                      IntegerType  <y|IDENT|tests/life/test.easy:14:34>                  
n      PROGRAM:Life.FUNCTION:neighbours|n                      IntegerType  <n|IDENT|tests/life/test.easy:15:13>                  
xx     PROGRAM:Life.FUNCTION:neighbours|xx                     IntegerType  <'('|SYMBOL|tests/life/test.easy:16:13>               
yy     PROGRAM:Life.FUNCTION:neighbours|yy                     IntegerType  <'('|SYMBOL|tests/life/test.easy:16:13>               
x      PROGRAM:Life.PROCEDURE:print|x                          IntegerType  <'('|SYMBOL|tests/life/test.easy:33:13>               
y      PROGRAM:Life.PROCEDURE:print|y                          IntegerType  <'('|SYMBOL|tests/life/test.easy:33:13>               
$0     |$0                                                     StringType   <'** [ EASY LIFE ]'|STRING|tests/life/test.easy:35:12>
$1     |$1                                                     StringType   <' '|STRING|tests/life/test.easy:35:32>               
$2     |$2                                                     StringType   <'*'|STRING|tests/life/test.easy:36:38>               
$F     |$F                                                     StringType   <x|IDENT|tests/life/test.easy:40:21>                  
$4     |$4                                                     StringType   <'x'|STRING|tests/life/test.easy:41:18>               
x      PROGRAM:Life.PROCEDURE:glider|x                         IntegerType  <x|IDENT|tests/life/test.easy:51:20>                  
y      PROGRAM:Life.PROCEDURE:glider|y                         IntegerType  <y|IDENT|tests/life/test.easy:51:31>                  
x      PROGRAM:Life.PROCEDURE:evolution|x                      IntegerType  <'('|SYMBOL|tests/life/test.easy:60:13>               
y      PROGRAM:Life.PROCEDURE:evolution|y                      IntegerType  <'('|SYMBOL|tests/life/test.easy:60:13>               
next   PROGRAM:Life.PROCEDURE:evolution|next                   AliasType    <next|IDENT|tests/life/test.easy:61:13>               
alive  PROGRAM:Life.PROCEDURE:evolution.FOR:$12.FOR:$13|alive  BooleanType  <alive|IDENT|tests/life/test.easy:65:17>              
n      PROGRAM:Life.PROCEDURE:evolution.FOR:$12.FOR:$13|n      IntegerType  <n|IDENT|tests/life/test.easy:66:17>                  
$5     |$5                                                     StringType   <'GENERATION: '|STRING|tests/life/test.easy:99:12>    

INTEGER  int                                                          
REAL     double                                                       
BOOLEAN  int                                                          
STRING   STR                                                          
Field    struct { struct { int data[80 - 0 + 1]; } data[25 - 0 + 1]; }

LENGTH      ->  int     built-in
CHARACTER   ->  STR     built-in
SUBSTR      ->  STR     built-in
FIX         ->  int     built-in
FLOAT       ->  double  built-in
FLOOR       ->  int     built-in
valid       ->  int     FUNCTION  (x int, y int)  <FUNCTION|KEYWORD|tests/life/test.easy:10:3>
neighbours  ->  int     FUNCTION  (x int, y int)  <FUNCTION|KEYWORD|tests/life/test.easy:14:3>

print      PROCEDURE  ()              <PROCEDURE|KEYWORD|tests/life/test.easy:32:3>
glider     PROCEDURE  (x int, y int)  <PROCEDURE|KEYWORD|tests/life/test.easy:51:3>
evolution  PROCEDURE  ()              <PROCEDURE|KEYWORD|tests/life/test.easy:59:3>

