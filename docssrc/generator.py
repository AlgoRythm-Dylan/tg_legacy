import sys
import datetime

class Document:

    def __init__(self, output=sys.stdout):
        self.items = []
        self.output = output

    def add(self, item):
        self.items.append(item)

    def render(self):
        print(f"<!-- Generated {datetime.datetime.now()} -->", file=self.output)
        for item in self.items:
            print(item.render(), file=self.output, end="")

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
        