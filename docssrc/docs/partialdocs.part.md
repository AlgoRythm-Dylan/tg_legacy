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
