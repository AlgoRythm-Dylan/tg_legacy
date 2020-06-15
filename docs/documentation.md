<!-- Generated 2020-06-14 19:05:03.068706 -->
# Documentation

This project has it's own documentation system. It's designed
to be a balance: less manual labor than just maintaining markdown
files by hand, but less technical debt than integrating an 
automated documentation system. It's just a few Python scripts
and some markdown files split into parts.

Jump to

[The Document Class](#the-document-class) • [Partial Documents](#partial-documents) •
[Reference Data](#the-reference-data)
## The Document Class

The Document class is the main class for documentation.
It's very simple, but essential.

It has two methods: `add` and `render`. You can `add`
two types of things:

* Any Python class with a `render` method that returns a string
* A string

The first option is great for extensibility: you can
generate documentation in any way that you would like.
Just create a class with a `render` method that takes
no arguments and returns a string.

The second option is for ease! You can literally just
`add("anything\n")` to your document, and it will render.
This is helpful for situations where simple is better.
(Don't forget your newlines!)

Of course, items are rendered in insertion order. That
is, they appear in your output in the same order you
added them.

Upon construction, the class accepts a named parameter,
`output`. This can be of type `string` or `file`. Upon
rendering, no matter what type was given, the output
member variable is converted into a `file` object. You
can manually close this if you would like, and this
is recommended, but Python cleans up open files for you.

You can set the member variable `output` any time before
rendering. Output defaults to `stdout` (It will print
to your console window)

```python
doc = Document(output="myfile.md")

doc.add("# This is my header\n")
doc.add(some_fn_that_returns_str())
doc.add(DocumentPartial("footer.part.md"))
doc.add("Document signed by " + input("Your signature: "))

doc.render()
doc.output.close()
```

*Note: any generated document will have a timestamp commented*
*at the top of of the source code*
## Partial Documents

One of the simplest components to the documentation system
is a Partial Document. It's literally just a markdown file
that is meant to be a module in a larger document. By
convention (and only convention), partial documents end
with a `.part.md` extension.

Because of how markdown works, all partial documents should end
with a newline, or else your document may not render properly.

There are a few advantages to this approach. The largest of
which being that you can use the exact same markdown
code in as many different documents as you wish. If you
need to correct or update some information, you only need
to do so once, in the appropriate partial document.

Partial documents also keep individual markdown files from
getting too large. You can split up one document into many
different sections so that they will be easier to maintain
over time.

In python, you just need to add a `PartialDocument` object to
your `Document` object.

Here is a simple example where you create a markdown file
called `somefile.md`. This is your documentation; your
final output. This document will be composed of three
partial documents: `intro.part.md`, `body.part.md`, and
`footer.part.md`

```python
from generator import *

doc = Document(output="somefile.md")

doc.add(DocumentPartial("intro.part.md"))
doc.add(DocumentPartial("body.part.md"))
doc.add(DocumentPartial("footer.part.md"))

doc.render()
doc.output.close()
```

The intro and body may be specific to the `somefile.md` document,
but if you wanted to have a footer shared by all of your
documentation, the `footer.part.md` file could accomplish that.
## The Reference Data

One of the major reasons for this semi-automated documentation
system is due to the labor of generating references, or
a table of contents. It's repetitive work, and making changes
can be annoying, as you need to alphabetize the table and
update the actual reference - it's just something a machine
would do better than a person.

Therefore, all reference data is stored in some python modules.
These modules start with `all_` and end with the name of the material
they are describing.

```
all_tg      The reference material for TG
all_docs    The reference material for the documentation system
```

These modules include items for generating both a table of contents
and actual reference materials dynamically with automatic sorting.
They use a few classes found in the `generator` module:

* [Table](#table) (internally)
* [DocCategory](#doccategory)
* [DocCategoryItem](#doccategoryitem)
