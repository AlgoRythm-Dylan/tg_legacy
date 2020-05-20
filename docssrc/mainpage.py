from generator import *

output = open("../docs/main.md", "w+")

doc = Document(output=output)

doc.add(DocumentPartial("main/head.part.md"))
doc.add(DocumentPartial("main/main-toc.part.md"))
doc.add(DocumentPartial("generic/what.part.md"))
doc.add(DocumentPartial("generic/build.part.md"))
doc.add(DocumentPartial("generic/getting-started.part.md"))
doc.add(DocumentPartial("generic/drawing.part.md"))
doc.add(DocumentPartial("main/reference-toc.part.md"))

generic_item_columns = ('Item', 'Description', 'Type')

color_items_table = Table()
color_items_table.title = "Color Items"
color_items_table.columns = generic_item_columns
color_items_table.add(('[Colors](#colors)', 'A list of colors', 'Constants'))

doc.add(color_items_table)

doc.render()

output.close()