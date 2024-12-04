# **Ease Language Documentation**

## **Welcome to Ease!**

Hey, so you’ve stumbled across **Ease**. Congratulations? It’s a tiny little language in development, and honestly, it doesn’t do much yet. But hey, you can create variables, show some output, and… that’s about it. 

Oh, and there are bugs. Plenty of bugs. But I'm working on it. Probably.

---

## **Getting Started**

To get started, all you need is to clone this repository, open `tests/test1.ease` file where you’ll write your very fancy Ease code. Then you can run the file by running `make run` command in the terminal (I will change this later).

---

## **1. Declaring Variables**

Creating variables in Ease is... well, easy. Just write the variable name, an equals sign, and its initial value. Done.

### **Rules:**
1. You **must** assign a value when declaring a variable. No lazy uninitialized stuff here.
2. Mathematical equations aren’t allowed in variable declarations. This isn’t a math competition.
3. Variables only hold one value. No assigning one variable to another.

### **Examples**

```ease
name = "Frank"
age = 30
height = 5.9
is_human = 1
```

### **Invalid Examples**

```ease
points = 5 + 2  # Do I look like a calculator to you?
score = points  # Nope, no assigning variables to other variables.
```

---

## **2. Printing Output**

Ease lets you print things using the `say` command. Whether it’s a string, a number, or a combination of the two, `say` is your go-to.

### **Rules:**

1. Use `say` to display output.


2. Strings go inside double quotes.


3. Numbers can be printed directly.


### **Examples**

```ease
say "Hello, world!"
say 42
say "This is Ease, a very average language."
```

### **Output**:

```output
Hello, world!
42
This is Ease, a very average language.
```

---

### **3. Printing Variables**

Now for something *slightly* more advanced. You can include variables in strings by putting them in curly braces `{}`.

### **Rules:**

1. Only variables are allowed in `{}`.

Numbers, math, or anything remotely *clever* won’t work.



2. Stick to **single** variables.



### **Examples**

```ease
pet = "dog"
say "I have a {pet}."
```

### **Output:**

```output
I have a dog.
```

```ease
age = 21
say "I’m {age} years old, and it feels... okay, I guess."
```

### **Output:**

```output
I’m 17 years old, and it feels... okay, I guess.
```

### **Invalid Examples:**

```ease
say "Five plus five is {5 + 5}."  # Nope, no math allowed.
say "Here's a number: {7}."      # Numbers in {}? Not happening.
```


---

### **4. What’s Next?**

Right now, Ease is very much in development. Like, the training wheels are still on. I'm working on:

Fixing the many (many) bugs.

Adding *actual* features.

Making it not crash when you look at it funny.

Also if you saw comments in the code and thought **Ease** atleast had comments then sorry to disappoint but no (it does **not**).


If you have suggestions, feedback, or just want to laugh at my work-in-progress, feel free to join the journey.

Happy Easing (or something like that).
