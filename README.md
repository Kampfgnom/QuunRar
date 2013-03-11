QuunRar 0.1
========

QuunRar is a Qt wrapper the UnRAR dll from http://www.rarlab.com/rar_add.htm.

Project setup
=============

You may include the library in your qmake based project by adding the following lines to you *.pro file:

````
QUUNRAR_PATH = relative/or/absolute/path/to/QuunRar
include($$QUUNRAR_PATH/QuunRar.pri)
INCLUDEPATH     += $$QUUNRAR_INCLUDEPATH
LIBS            += $$QUUNRAR_LIBS
````

You may of course include the library in any other way, too.

Requirements and building
=========================

QuunRar has been developed and tested only on Qt 5.0 on a Mac. The library should be compatible to older versions of Qt and other operating systems as well.


Contributing
============

Since this library is currently only used internally, it is not tested well.

If you find errors, simply file an issue in GitHub, or even better:

If you want to contribute, you can do the usual Fork-Patch-FilePullRequest dance. I might even tranfser the project in someone else's hands!


License
=======

QuunRar is licensed under the LGPLv3.0. See LICENSE for details.

The UnRAR dll comes with its own licence, which you can find in unrarlicence.txt.

In truth I have no idea if these two licences are compatible with each other :wink:.