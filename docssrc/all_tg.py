from generator import *

"""

    Necessary classes

"""

generic_item_columns = ('Item', 'Description', 'Type')

class DocCategory:

    def __init__(self, title=None, description=None):
        self.items = []
        self.title = title
        self.description = description

    def sort(self):
        self.items.sort(key=lambda item: item.name)

    def add(self, item):
        self.items.append(item)

    def toc(self, links=False):
        content = f"|{'|'.join(generic_item_columns)}|\n"
        content += f"|{'|'.join(['---']*3)}|\n"
        for item in self.items:
            description = "No Description"
            if item.short_description != None:
                description = item.short_description
            name = item.name
            if links:
                name = f"[{name}](#{to_md_link(name)})"
            row = (name, description, item.item_type)
            content += f"|{'|'.join(row)}|\n"
        return content

    def render(self, include_toc=True, toc_links=False):
        content = ""
        if include_toc:
            content = self.toc(links=toc_links)
        for item in self.items:
            content += item.render()
        return content

DocCategoryColumns = {
    "Struct": ('Member', 'Data Type', 'Description'),
    "Function": ('Usage','Item', 'Data Type', 'Description'),
    "Extern": ('Data Type', 'Description'),
    "Constant": ('Data Type', 'Description'),
    "Constants": ('Name','Data Type', 'Description')
}

class DocCategoryItem:

    def __init__(self, name, description, item_type):
        self.name = name
        self.description = description
        self.short_description = None
        self.item_type = item_type
        self.title = None
        self.pre_text = None
        self.post_text = None
        self.table = Table()
        self.table.columns = DocCategoryColumns[item_type]

    def render(self):
        content = ""
        if self.title != None:
            content += f"## {self.title}\n"
        else:
            content += f"## {self.name}\n"
        if self.description != None:
            content += f"*{self.description}*\n"
        if self.pre_text != None:
            if type(self.pre_text) == str:
                content += f"{self.pre_text}\n"
            else:
                content += self.pre_text.render()
        if len(self.table.rows) > 0:
            content += self.table.render()
        if self.post_text != None:
            if type(self.post_text == str):
                content += f"{self.post_text}\n"
            else:
                content += self.post_text.render()
        return content

"""

    Actual content

"""

category_all = DocCategory()
category_colors = DocCategory(title="Color Items")

doc_item_colors = DocCategoryItem("Colors", None, "Constants")
doc_item_colors.short_description = "A list of colors"
doc_item_colors.pre_text = DocumentPartial("items/colors_pretext.part.md")

doc_item_tgcolor = DocCategoryItem("TGColor", "Describes a color", "Struct")
doc_item_tgcolor.short_description = "The color structure"
doc_item_tgcolor.table.add(('id','unsigned int','Color ID'))
doc_item_tgcolor.table.add(('Foreground','unsigned short','Foreground color ID'))
doc_item_tgcolor.table.add(('Background','unsigned short','Backgorund color ID'))

category_all.add(doc_item_colors)
category_all.add(doc_item_tgcolor)

category_colors.add(doc_item_colors)
category_colors.add(doc_item_tgcolor)

category_all.sort()
category_colors.sort()