from generator import *
from all_tg import *

doc = Document(output="../docs/main.md")

doc.add(DocumentPartial("main/head.part.md"))
doc.add(DocumentPartial("main/main-toc.part.md"))
doc.add(DocumentPartial("generic/what.part.md"))
doc.add(DocumentPartial("generic/build.part.md"))
doc.add(DocumentPartial("generic/getting-started.part.md"))
doc.add(DocumentPartial("generic/drawing.part.md"))
doc.add(DocumentPartial("main/reference-toc.part.md"))

doc.add(category_colors.toc(links=True))
doc.add(category_all.render(include_toc=False))

if __name__ == "__main__":
    doc.render()
    doc.output.close()