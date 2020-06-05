## The Reference Data

One of the major reasons for this semi-automated documentation
system is due to the labor of generating references, or
a table of contents. It's repetitive work, and making changes
can be annoying, as you need to alphabetize the table and
update the actual reference - it's just something a machine
would do better than a person.

Therefore, all reference data is stored in some python modules.
These modules start with `all_` and end with the name of the material
they are describing.

```
all_tg      The reference material for TG
all_docs    The reference material for the documentation system
```

These modules include items for generating both a table of contents
and actual reference materials dynamically with automatic sorting.
They use a few classes found in the `generator` module:

* [Table](#table) (internally)
* [DocCategory](#doccategory)
* [DocCategoryItem](#doccategoryitem)
