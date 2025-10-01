w      PROGRAM:Life|w                          IntegerType  <'('|SYMBOL|life.easy:4:11>                
h      PROGRAM:Life|h                          IntegerType  <'('|SYMBOL|life.easy:4:11>                
field  PROGRAM:Life|field                      AliasType    <field|IDENT|life.easy:5:11>               
x      PROGRAM:Life|x                          IntegerType  <'('|SYMBOL|life.easy:7:11>                
y      PROGRAM:Life|y                          IntegerType  <'('|SYMBOL|life.easy:7:11>                
i      PROGRAM:Life|i                          IntegerType  <i|IDENT|life.easy:8:11>                   
x      PROGRAM:Life.FUNCTION:valid|x           IntegerType  <x|IDENT|life.easy:10:18>                  
y      PROGRAM:Life.FUNCTION:valid|y           IntegerType  <y|IDENT|life.easy:10:29>                  
x      PROGRAM:Life.FUNCTION:neighbours|x      IntegerType  <x|IDENT|life.easy:14:23>                  
y      PROGRAM:Life.FUNCTION:neighbours|y      IntegerType  <y|IDENT|life.easy:14:34>                  
n      PROGRAM:Life.FUNCTION:neighbours|n      IntegerType  <n|IDENT|life.easy:15:13>                  
xx     PROGRAM:Life.FUNCTION:neighbours|xx     IntegerType  <'('|SYMBOL|life.easy:16:13>               
yy     PROGRAM:Life.FUNCTION:neighbours|yy     IntegerType  <'('|SYMBOL|life.easy:16:13>               
$0     |$0                                     StringType   <'['|STRING|life.easy:33:27>               
$1     |$1                                     StringType   <'H'|STRING|life.easy:33:32>               
$2     |$2                                     StringType   <'J'|STRING|life.easy:33:57>               
x      PROGRAM:Life.PROCEDURE:print|x          IntegerType  <'('|SYMBOL|life.easy:37:13>               
y      PROGRAM:Life.PROCEDURE:print|y          IntegerType  <'('|SYMBOL|life.easy:37:13>               
$3     |$3                                     StringType   <'** [ EASY LIFE ]'|STRING|life.easy:41:12>
$4     |$4                                     StringType   <' '|STRING|life.easy:41:32>               
$5     |$5                                     StringType   <'*'|STRING|life.easy:42:38>               
$6     |$6                                     StringType   <'x'|STRING|life.easy:47:18>               
x      PROGRAM:Life.PROCEDURE:glider|x         IntegerType  <x|IDENT|life.easy:57:20>                  
y      PROGRAM:Life.PROCEDURE:glider|y         IntegerType  <y|IDENT|life.easy:57:31>                  
x      PROGRAM:Life.PROCEDURE:evolution|x      IntegerType  <'('|SYMBOL|life.easy:66:13>               
y      PROGRAM:Life.PROCEDURE:evolution|y      IntegerType  <'('|SYMBOL|life.easy:66:13>               
next   PROGRAM:Life.PROCEDURE:evolution|next   AliasType    <next|IDENT|life.easy:67:13>               
alive  PROGRAM:Life.PROCEDURE:evolution|alive  BooleanType  <alive|IDENT|life.easy:71:17>              
n      PROGRAM:Life.PROCEDURE:evolution|n      IntegerType  <n|IDENT|life.easy:72:17>                  
$7     |$7                                     StringType   <'GENERATION: '|STRING|life.easy:105:12>   

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
valid       ->  int     FUNCTION  (x int, y int)  <FUNCTION|KEYWORD|life.easy:10:3>
neighbours  ->  int     FUNCTION  (x int, y int)  <FUNCTION|KEYWORD|life.easy:14:3>

clearscreen  PROCEDURE  ()              <PROCEDURE|KEYWORD|life.easy:32:3>
print        PROCEDURE  ()              <PROCEDURE|KEYWORD|life.easy:36:3>
glider       PROCEDURE  (x int, y int)  <PROCEDURE|KEYWORD|life.easy:57:3>
evolution    PROCEDURE  ()              <PROCEDURE|KEYWORD|life.easy:65:3>

