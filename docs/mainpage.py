from generator import *

file = open("test.md", "w+")
document = Document()

# TITLE
main_title_section = TitleSection()
main_title_section.text = Text("TG")
document.add_section(main_title_section)
text_section = TextSection()
main_subtitle = Text("cross-platform advanced terminal control")
main_subtitle.italic = True
text_section.add_text(main_subtitle)
document.add_section(text_section)

# TABLE OF CONTENTS (MAIN)
main_toc_title = TitleSection()
main_toc_title.text = Text("TOC")
main_toc_title.level = 2
document.add_section(main_toc_title)

main_toc = UnorderedListSection()
main_toc_items = [
    Text("What does TG do?"),
    Text("Building"),
    Text("Getting Started"),
    Text("Drawing"),
    Text("Reference")
]

main_toc_links = [
    "#what-does-tg-do",
    "#building",
    "#getting-started",
    "#drawing",
    "#reference"
]

i = 0
for main_toc_item in main_toc_items:
    main_toc_item.set_link(main_toc_links[i])
    main_toc.add_item(main_toc_item)
    i+=1

document.add_section(main_toc)

## WHAT DOES TG DO?

document.add_section(TitleSection(text=Text("What does TG do?"), level=2))

document.renderMD(output = file)

file.close()