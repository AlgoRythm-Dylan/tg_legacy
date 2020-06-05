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
