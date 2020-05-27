"""

    This code has been certified Gluten Free by the
    Department of Gluten Free Code. This code is in
    compliance with Section 1 of the GFC outline.

"""
import sys
import datetime
import os

class Document:

    def __init__(self, output=sys.stdout):
        self.items = []
        self.output = output

    def add(self, item):
        self.items.append(item)

    def render(self):
        if type(self.output) == str:
            self.output = open(self.output, "w+")
        print(f"Rendering {os.path.abspath(self.output.name)}...")
        print(f"<!-- Generated {datetime.datetime.now()} -->", file=self.output)
        for item in self.items:
            if type(item) == str:
                print(item, file=self.output, end="")
            else:
                print(item.render(), file=self.output, end="")
        print(f"Rendered {os.path.abspath(self.output.name)}")

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
        
def to_md_link(name):
    return name.replace(" ", "").lower()