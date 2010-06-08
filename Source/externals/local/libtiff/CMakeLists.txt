project( libtiff )

set( SRCS
../../tiff/libtiff/tif_aux.c
../../tiff/libtiff/tif_close.c
../../tiff/libtiff/tif_codec.c
../../tiff/libtiff/tif_color.c
../../tiff/libtiff/tif_compress.c
../../tiff/libtiff/tif_dir.c
../../tiff/libtiff/tif_dir.h
../../tiff/libtiff/tif_dirinfo.c
../../tiff/libtiff/tif_dirread.c
../../tiff/libtiff/tif_dirwrite.c
../../tiff/libtiff/tif_dumpmode.c
../../tiff/libtiff/tif_error.c
../../tiff/libtiff/tif_extension.c
../../tiff/libtiff/tif_fax3.c
../../tiff/libtiff/tif_fax3.h
../../tiff/libtiff/tif_fax3sm.c
../../tiff/libtiff/tif_flush.c
../../tiff/libtiff/tif_getimage.c
../../tiff/libtiff/tif_jbig.c
../../tiff/libtiff/tif_jpeg.c
../../tiff/libtiff/tif_luv.c
../../tiff/libtiff/tif_lzw.c
../../tiff/libtiff/tif_next.c
../../tiff/libtiff/tif_ojpeg.c
../../tiff/libtiff/tif_open.c
../../tiff/libtiff/tif_packbits.c
../../tiff/libtiff/tif_pixarlog.c
../../tiff/libtiff/tif_predict.c
../../tiff/libtiff/tif_predict.h
../../tiff/libtiff/tif_print.c
../../tiff/libtiff/tif_read.c
../../tiff/libtiff/tif_stream.cxx
../../tiff/libtiff/tif_strip.c
../../tiff/libtiff/tif_swab.c
../../tiff/libtiff/tif_thunder.c
../../tiff/libtiff/tif_tile.c
../../tiff/libtiff/tif_version.c
../../tiff/libtiff/tif_warning.c
../../tiff/libtiff/tif_win32.c
../../tiff/libtiff/tif_write.c
../../tiff/libtiff/tif_zip.c
)

set( INCS
../../tiff/libtiff/tif_dir.h
../../tiff/libtiff/tif_fax3.h
../../tiff/libtiff/tif_predict.h
../../tiff/libtiff/tiff.h
)

set( CONF_INCS
./tif_config.h
./tiffconf.h
)

include_directories( . )
include_directories( ../../tiff/libtiff )

source_group( Conf FILES ${CONF_INCS} )
source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${CONF_INCS} ${INCS} )

