
# eg: $$qtLibraryNameVersion(qcanpool, 1)
defineReplace(qtLibraryNameVersion) {
   RET = $$qtLibraryTarget($$1)
   win32 {
      RET = $$RET$$2
   }
   return($$RET)
}

INCLUDEPATH += $$PWD/src
