# Generate markdown or HTML docs
# TODO: each object needs a renderHTML
import datetime

class Document:

    def __init__(self):
        self.items = []

    def add_section(self, section):
        self.items.append(section)

    def renderMD(self, output=None):
        if output == None:
            # Return a string
            markdown = f"<!-- This was auto generated {datetime.datetime.now()} -->\n"
            for item in self.items:
                markdown += item.renderMD()
                return markdown
        else:
            # Write to file
            print(f"<!-- This was auto generated {datetime.datetime.now()} -->\n", file=output)
            for item in self.items:
                print(item.renderMD(), file=output)




# BASE SECTION CLASS
class SectionBase:

    def __init__(self):
        pass

    def renderMD(self):
        pass
    
# A rather important class
class Text():

    def __init__(self, text):
        self.bold = False
        self.italic = False
        self.underline = False
        self.text = text
        self.link = False
        self.inline_code = False
        self.code_block = False
        self.location = ""

    def set_link(self, location):
        if location == None:
            self.link = False
        else:
            self.link = True
            self.location = location

    def renderMD(self):
        text = ""
        if self.link:
            text += "["
        if self.code_block:
            text += "```"
        if self.inline_code:
            text += "`"
        if self.bold:
            text += "**"
        if self.italic:
            text += "*"
        if self.underline:
            text += "_"
        text += self.text
        if self.bold:
            text += "**"
        if self.italic:
            text += "*"
        if self.underline:
            text += "_"
        if self.inline_code:
            text += "`"
        if self.code_block:
            text += "```"
        if self.link:
            text += f"]({self.location})"
        return text

class TitleSection(SectionBase):
    def __init__(self, text=None, level=1):
        self.level = level
        self.text = text

    def renderMD(self):
        if self.text != None:
            return f"{'#'*self.level} {self.text.renderMD()}"

class TextSection(SectionBase):

    def __init__(self):
        self.items = []

    def add_text(self, text):
        self.items.append(text)

    def renderMD(self):
        text = ""
        for item in self.items:
            text += item.renderMD()
        if not text.endswith("    "):
            text += "    "
        return text

class Newline:

    def __init__(self, amount=1):
        self.amount = 1

    def renderMD(self):
        return "\n"*self.amount

class UnorderedListSection(SectionBase):

    def __init__(self):
        self.items = []

    def add_item(self, item):
        self.items.append(item)

    def renderMD(self):
        text = ""
        for item in self.items:
            if type(item) == str:
                text += f"* {item}\n"
            else:
                text += f"* {item.renderMD()}\n"
        return text

class ArgumentTable(SectionBase):

    def __init__(self):
        pass

    def renderMD(self):
        return "Hey!"

ITEM_TYPE_FUNCTION = 0
ITEM_TYPE_STRUCT   = 1
ITEM_TYPE_EXTERN   = 2
ITEM_TYPE_CONSTANT = 3

class Item:

    def __init__(self, name, item_type):
        this.title = None # Section
        this.notes = None # Section
        this.name = name
        this.item_type = item_type
