from generator import *
from all_doc import *

doc = Document(output="../docs/documentation.md")

doc.add(DocumentPartial("docs/intro.part.md"))
doc.add(DocumentPartial("docs/docclass.part.md"))
doc.add(DocumentPartial("docs/partialdocs.part.md"))
doc.add(DocumentPartial("docs/refdata.part.md"))

if __name__ == "__main__":
    doc.render()
    doc.output.close()