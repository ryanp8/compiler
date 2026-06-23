# Toy C-like Language Compiler

Work in progress project inspired by the code submitted for the assignments in [Compiler Construction](https://users.cs.northwestern.edu/~simonec/CC.html) course at Northwestern.
The assignments involved creating a series of compilers that perform semantics-preserving transformations (implementing loops, code linearization, instruction selection, register allocation, etc) but used many intermediate languages (LB -> LA -> IR -> L3 -> L2 -> L1 -> x86). The goal for this project is
to improve on the code written for class and go from LB -> IR -> x86 without emitting as many intermediate languages, following a structure more similar to what is used in production.

## Stages
1. Parsing: Use PEGTL to convert the program into AST and determines the type of each variable. The AST is converted into block-based IR
2. IR: Convert into SSA form for optimizations. Perform instruction selection and graph-coloring register allocation to generate x86
3. x86_64 assembly

## Language Grammar
Inspired from CS322's LB language
```
p ::= f+
f ::= T name ( pars ) scope
scope ::= { i* }
i ::= type names | name <- t | name <- t op t |
label | if (cond) label label | goto label | return t? |
while (cond) label label | continue | break |
name <- name([t])+ | name([t])+<- t | name <- length name t? |
name( args? ) | name <- name( args? ) | name <- new Array(args) | scope
T ::= int64([])* | void
args ::= t | t (,t)*
pars ::= type var | type var (, type var)* |
t ::= name | N
N ::= (+|-)?[0-9]+
op ::= + | - | * | & | << | >> | cmp
cmp ::= < | <= | =| >= | >
name ::= [a-zA-Z_][a-zA-Z_0-9]*
label ::= :name
cond ::= t cmp t
names::= name | name (, name)*
```

## Usage
```
mkdir build && cd build
cmake ..
./compiler path_to_source
```
