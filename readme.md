# SimpleLang Compiler for 8‑Bit CPU

This project implements a full compiler for **SimpleLang**, a minimal high‑level language designed for a custom 8‑bit CPU. The compiler translates SimpleLang source code into valid assembly compatible with the CPU architecture provided in the referenced GitHub repository.

---

## 🚀 Project Overview

The compiler performs the complete pipeline:

1. **Lexical Analysis** – Converts raw source code into tokens.
2. **Parsing** – Builds an Abstract Syntax Tree (AST) using recursive descent.
3. **Semantic Analysis** – Validates types, scope, declarations, and identifiers.
4. **Code Generation** – Emits correct assembly instructions for:

   * variable declarations
   * assignments
   * arithmetic expressions
   * conditional statements (`if`)

The compiler outputs assembly instructions compatible with the 8-bit CPU project:

🔗 **https://github.com/lightcode/8bit-computer**

A full copy of that project is included in the repository for convenience.

---

## 🧩 Supported SimpleLang Features

### ✔ Variable Declarations

```
int a;
int b;
```

### ✔ Assignments

```
a = 5;
b = a;
```

### ✔ Arithmetic Expressions

```
int c;
c = a + b;
```

### ✔ Conditional Statements

```
if(a > b) {
    c = a + b;
}
```

### ✔ Supported Comparison Operators

* `==`
* `!=`
* `<`
* `<=`
* `>`
* `>=`

All mapped to CPU jump instructions: `jz`, `jnz`, `jc`, `jnc`, `jmp`.

---

## 🏗 Architecture Overview

###  1. Lexer (`lexer.c`, `lexer.h`)

* Generates tokens
* Handles identifiers, numbers, keywords, operators, punctuation

### 2. Parser (`parser.c`, `parser.h`)

* Builds AST nodes for:

  * declarations
  * assignments
  * expressions
  * `if` statements
* Grammar is LL(1)

### 3. Semantic Analyzer (`semantic.c`, `semantic.h`)

* Implements hierarchical symbol tables
* Validates:

  * redeclarations
  * type correctness
  * scope correctness
  * undefined identifiers


### 4. Code Generator (`codegen.c`, `codegen.h`)

* Emits `.text` and `.data` sections
* Converts AST nodes into assembly
* Generates comparison logic using CPU flags (CF, ZF)
* Produces valid code for:

  * `push`, `pop`
  * `cmp`
  * `add`
  * `jmp` / `jz` / `jnz` / `jc` / `jnc`

---

## 📚 External Dependency (Included in Repository)

This project uses the **8-bit CPU architecture and assembler** from:

🔗 https://github.com/lightcode/8bit-computer

The entire project is included as a **separate folder** inside this repository for convenience.

My compiler generates assembly code that is **fully compatible** with this 8-bit CPU, allowing you to directly assemble and run the output using the included `asm.py` assembler and CPU simulator.

---

## 🧪 Sample Input

```
int a;
int b;
b = 2;
a = 5;
int c;
c = 3;
if(a > b) {
    c = a + b;
}
```

---

## 🟦 Generated Assembly Output (Example)

```
.text
start:
    ldi A 2
    sta %b
    ldi A 5
    sta %a
    ldi A 3
    sta %c

    lda %a
    push A
    lda %b
    pop B
    cmp
    jnz %L0
    jnc %L0
    jmp %L1

L0:
    lda %a
    push A
    lda %b
    pop B
    add
    sta %c

L1:
    hlt

.data
c = 0
b = 0
a = 0
```

---

## 🛠 Build & Run

### Compile compiler:

```
make
```

### Run compiler:

```
make run INPUT=input.sl OUTPUT=output.asm
```

### Assemble and run on 8-bit CPU:

```
python3 8bit-computer/asm/asm.py output.asm > 8bit-computer/memory.list
```
```
 cd ./8bit-computer/
```
```
 make clean && make run
```


---

## 📂 Project Structure

```
SIMPLELANG_COMPILER/

│
├── examples/
│   └── input.txt
│
├── include/
│   ├── codegen.h
│   ├── lexer.h
│   ├── parser.h
│   └── semantic.h
│
├── src/
│   ├── codegen.c
│   ├── lexer.c
│   ├── main.c
│   ├── parser.c
│   └── semantic.c
│
├── 8bit-computer/          ← Added external 8-bit CPU project
│   └── (full source from github.com/lightcode/8bit-computer)
│
├── makefile
├── output.asm
└── readme.md
```

---

## 👤 Author

Parth Chavda — Compiler + 8-bit CPU enthusiast.

