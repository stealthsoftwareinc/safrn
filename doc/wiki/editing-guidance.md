# Wiki Editing Guidance

* The first section of a wiki page should be an "introduction" section, describing:
  * The purpose of the page itself
  * The architectural purpose of any software components contained within
* For wiki pages describing a datastructure, please ensure they contain a **section called "Structure"** that describes a top-down hierarchy of the datastructure.
* If there are any previously answered or outstanding questions, please **include a "Questions" section** with answers indented below each question
* When indenting pages, please use **2 spaces** between levels in a structure.  This will help with readability and consistency.
* When describing datatypes or code please **use double back-tick (``<<datatype>>``)** escape sequences.
* When describing datatypes **use the UML convention of double angle brackets (``<<datatype>>``)**
* When specifying optional fields **use (optional) notation**
* Ensure that all documentation goes into ``doc/wiki``, so it will get populated into the wiki page under the project.  When these changes are created, ensure you create a ``wiki/*`` branch from the master branch and back into master.