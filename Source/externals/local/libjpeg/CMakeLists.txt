project( libjpeg )

set( SRCS
../../libjpeg/jcapimin.c
../../libjpeg/jcapistd.c
../../libjpeg/jccoefct.c
../../libjpeg/jccolor.c
../../libjpeg/jcdctmgr.c
../../libjpeg/jchuff.c
../../libjpeg/jcinit.c
../../libjpeg/jcmainct.c
../../libjpeg/jcmarker.c
../../libjpeg/jcmaster.c
../../libjpeg/jcomapi.c
../../libjpeg/jcparam.c
../../libjpeg/jcphuff.c
../../libjpeg/jcprepct.c
../../libjpeg/jcsample.c
../../libjpeg/jctrans.c
../../libjpeg/jdapimin.c
../../libjpeg/jdapistd.c
../../libjpeg/jdatadst.c
../../libjpeg/jdatasrc.c
../../libjpeg/jdcoefct.c
../../libjpeg/jdcolor.c
../../libjpeg/jddctmgr.c
../../libjpeg/jdhuff.c
../../libjpeg/jdinput.c
../../libjpeg/jdmainct.c
../../libjpeg/jdmarker.c
../../libjpeg/jdmaster.c
../../libjpeg/jdmerge.c
../../libjpeg/jdphuff.c
../../libjpeg/jdpostct.c
../../libjpeg/jdsample.c
../../libjpeg/jdtrans.c
../../libjpeg/jerror.c
../../libjpeg/jfdctflt.c
../../libjpeg/jfdctfst.c
../../libjpeg/jfdctint.c
../../libjpeg/jidctflt.c
../../libjpeg/jidctfst.c
../../libjpeg/jidctint.c
../../libjpeg/jidctred.c
../../libjpeg/jmemmgr.c
../../libjpeg/jmemnobs.c
../../libjpeg/jquant1.c
../../libjpeg/jquant2.c
../../libjpeg/jutils.c
)

set( INCS
../../libjpeg/jchuff.h
../../libjpeg/jdct.h
../../libjpeg/jdhuff.h
../../libjpeg/jerror.h
../../libjpeg/jinclude.h
../../libjpeg/jmemsys.h
../../libjpeg/jmorecfg.h
../../libjpeg/jpegint.h
../../libjpeg/jpeglib.h
../../libjpeg/jversion.h
)

set( CONF_INCS
./jconfig.h
)

include_directories( . )
include_directories( ../../libjpeg )

source_group( Conf FILES ${CONF_INCS} )
source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${CONF_INCS} ${INCS} )

