"""

    This code has been certified Gluten Free by the
    Department of Gluten Free Code. This code is in
    compliance with Section 1 of the GFC outline.

"""
import sys
import datetime
import os

# Logging method, quick'n'dirty
def lg(tag, msg):
    print(f"{f'[{tag}]'.ljust(8)} {msg}")

class Document:

    def __init__(self, output=sys.stdout):
        self.items = []
        self.output = output

    def add(self, item):
        self.items.append(item)

    def render(self):
        try:
            if type(self.output) == str:
                self.output = open(self.output, "w+")
            lg("INFO", f" Rendering {os.path.abspath(self.output.name)}...")
            print(f"<!-- Generated {datetime.datetime.now()} -->", file=self.output)
            for item in self.items:
                if type(item) == str:
                    print(item, file=self.output, end="")
                else:
                    print(item.render(), file=self.output, end="")
            lg("DONE", f" Rendered {os.path.abspath(self.output.name)}")
        except Exception as e:
            lg("ERROR", f" {str(e)}")

class DocumentPartial:

    def __init__(self, path):
        self.path = path

    def render(self):
        try:
            file = open(self.path, "r")
            content = file.read()
            file.close()
            return content
        except Exception as e:
            print(str(e), file=sys.stderr)

class Table:

    def __init__(self):
        self.title = None
        self.title_level = 3
        self.columns = None
        self.rows = []

    def add(self, row):
        self.rows.append(row)

    def render(self):
        content = ""
        if self.title != None:
            content += f"{'#'*self.title_level} {self.title}\n"
        if self.columns != None:
            content += f"|{'|'.join(self.columns)}|\n"
            content += f"|{'|'.join(['---']*len(self.columns))}|\n"
        for row in self.rows:
            content += f"|{'|'.join(row)}|\n"
        return content


def newlines(amout=1):
    return "\n"*amount


def to_md_link(name):
    return name.replace(" ", "-").lower()


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

class DocItem:

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