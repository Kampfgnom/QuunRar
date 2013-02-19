UNRAR_PATH = $$PWD
UNRAR_INCLUDEPATH = $$PWD/include

DEFINES += RARDLL _UNIX SILENT

OTHER_FILES += \
    $$PWD/readme.txt \
    $$PWD/msc.dep

HEADERS += \
    $$PWD/volume.hpp \
    $$PWD/version.hpp \
    $$PWD/unpack.hpp \
    $$PWD/unicode.hpp \
    $$PWD/ulinks.hpp \
    $$PWD/timefn.hpp \
    $$PWD/system.hpp \
    $$PWD/suballoc.hpp \
    $$PWD/strlist.hpp \
    $$PWD/strfn.hpp \
    $$PWD/smallfn.hpp \
    $$PWD/sha1.hpp \
    $$PWD/secpassword.hpp \
    $$PWD/scantree.hpp \
    $$PWD/savepos.hpp \
    $$PWD/rs.hpp \
    $$PWD/rijndael.hpp \
    $$PWD/resource.hpp \
    $$PWD/recvol.hpp \
    $$PWD/rdwrfn.hpp \
    $$PWD/rawread.hpp \
    $$PWD/rarvm.hpp \
    $$PWD/rartypes.hpp \
    $$PWD/raros.hpp \
    $$PWD/rarlang.hpp \
    $$PWD/rardefs.hpp \
    $$PWD/rar.hpp \
    $$PWD/pathfn.hpp \
    $$PWD/os.hpp \
    $$PWD/options.hpp \
    $$PWD/model.hpp \
    $$PWD/match.hpp \
    $$PWD/log.hpp \
    $$PWD/loclang.hpp \
    $$PWD/list.hpp \
    $$PWD/isnt.hpp \
    $$PWD/headers.hpp \
    $$PWD/global.hpp \
    $$PWD/getbits.hpp \
    $$PWD/find.hpp \
    $$PWD/filestr.hpp \
    $$PWD/filefn.hpp \
    $$PWD/file.hpp \
    $$PWD/filcreat.hpp \
    $$PWD/extract.hpp \
    $$PWD/extinfo.hpp \
    $$PWD/errhnd.hpp \
    $$PWD/encname.hpp \
    $$PWD/dll.hpp \
    $$PWD/crypt.hpp \
    $$PWD/crc.hpp \
    $$PWD/consio.hpp \
    $$PWD/compress.hpp \
    $$PWD/coder.hpp \
    $$PWD/cmddata.hpp \
    $$PWD/array.hpp \
    $$PWD/archive.hpp

SOURCES += \
    $$PWD/win32stm.cpp \
    $$PWD/win32acl.cpp \
    $$PWD/volume.cpp \
    $$PWD/uowners.cpp \
    $$PWD/unpack20.cpp \
    $$PWD/unpack15.cpp \
    $$PWD/unpack.cpp \
    $$PWD/unios2.cpp \
    $$PWD/unicode.cpp \
    $$PWD/ulinks.cpp \
    $$PWD/timefn.cpp \
    $$PWD/system.cpp \
    $$PWD/suballoc.cpp \
    $$PWD/strlist.cpp \
    $$PWD/strfn.cpp \
    $$PWD/smallfn.cpp \
    $$PWD/sha1.cpp \
    $$PWD/secpassword.cpp \
    $$PWD/scantree.cpp \
    $$PWD/savepos.cpp \
    $$PWD/rs.cpp \
    $$PWD/rijndael.cpp \
    $$PWD/resource.cpp \
    $$PWD/recvol.cpp \
    $$PWD/rdwrfn.cpp \
    $$PWD/rawread.cpp \
    $$PWD/rarvmtbl.cpp \
    $$PWD/rarvm.cpp \
    $$PWD/rarpch.cpp \
    $$PWD/rar.cpp \
    $$PWD/pathfn.cpp \
    $$PWD/os2ea.cpp \
    $$PWD/options.cpp \
    $$PWD/model.cpp \
    $$PWD/match.cpp \
    $$PWD/log.cpp \
    $$PWD/list.cpp \
    $$PWD/isnt.cpp \
    $$PWD/global.cpp \
    $$PWD/getbits.cpp \
    $$PWD/find.cpp \
    $$PWD/filestr.cpp \
    $$PWD/filefn.cpp \
    $$PWD/file.cpp \
    $$PWD/filcreat.cpp \
    $$PWD/extract.cpp \
    $$PWD/extinfo.cpp \
    $$PWD/errhnd.cpp \
    $$PWD/encname.cpp \
    $$PWD/dll.cpp \
    $$PWD/crypt.cpp \
    $$PWD/crc.cpp \
    $$PWD/consio.cpp \
    $$PWD/coder.cpp \
    $$PWD/cmddata.cpp \
    $$PWD/beosea.cpp \
    $$PWD/arcread.cpp \
    $$PWD/archive.cpp \
    $$PWD/arccmt.cpp
