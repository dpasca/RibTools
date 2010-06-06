project( libtiff )

set( SRCS
../../tiff-3.9.2/libtiff/tif_aux.c
../../tiff-3.9.2/libtiff/tif_close.c
../../tiff-3.9.2/libtiff/tif_codec.c
../../tiff-3.9.2/libtiff/tif_color.c
../../tiff-3.9.2/libtiff/tif_compress.c
../../tiff-3.9.2/libtiff/tif_dir.c
../../tiff-3.9.2/libtiff/tif_dir.h
../../tiff-3.9.2/libtiff/tif_dirinfo.c
../../tiff-3.9.2/libtiff/tif_dirread.c
../../tiff-3.9.2/libtiff/tif_dirwrite.c
../../tiff-3.9.2/libtiff/tif_dumpmode.c
../../tiff-3.9.2/libtiff/tif_error.c
../../tiff-3.9.2/libtiff/tif_extension.c
../../tiff-3.9.2/libtiff/tif_fax3.c
../../tiff-3.9.2/libtiff/tif_fax3.h
../../tiff-3.9.2/libtiff/tif_fax3sm.c
../../tiff-3.9.2/libtiff/tif_flush.c
../../tiff-3.9.2/libtiff/tif_getimage.c
../../tiff-3.9.2/libtiff/tif_jbig.c
../../tiff-3.9.2/libtiff/tif_jpeg.c
../../tiff-3.9.2/libtiff/tif_luv.c
../../tiff-3.9.2/libtiff/tif_lzw.c
../../tiff-3.9.2/libtiff/tif_next.c
../../tiff-3.9.2/libtiff/tif_ojpeg.c
../../tiff-3.9.2/libtiff/tif_open.c
../../tiff-3.9.2/libtiff/tif_packbits.c
../../tiff-3.9.2/libtiff/tif_pixarlog.c
../../tiff-3.9.2/libtiff/tif_predict.c
../../tiff-3.9.2/libtiff/tif_predict.h
../../tiff-3.9.2/libtiff/tif_print.c
../../tiff-3.9.2/libtiff/tif_read.c
../../tiff-3.9.2/libtiff/tif_stream.cxx
../../tiff-3.9.2/libtiff/tif_strip.c
../../tiff-3.9.2/libtiff/tif_swab.c
../../tiff-3.9.2/libtiff/tif_thunder.c
../../tiff-3.9.2/libtiff/tif_tile.c
../../tiff-3.9.2/libtiff/tif_version.c
../../tiff-3.9.2/libtiff/tif_warning.c
../../tiff-3.9.2/libtiff/tif_win32.c
../../tiff-3.9.2/libtiff/tif_write.c
../../tiff-3.9.2/libtiff/tif_zip.c
)

set( INCS
../../tiff-3.9.2/libtiff/tif_dir.h
../../tiff-3.9.2/libtiff/tif_fax3.h
../../tiff-3.9.2/libtiff/tif_predict.h
../../tiff-3.9.2/libtiff/tiff.h
)

set( CONF_INCS
./tif_config.h
./tiffconf.h
)

include_directories( . )
include_directories( ../../tiff-3.9.2/libtiff )

source_group( Conf FILES ${CONF_INCS} )
source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${CONF_INCS} ${INCS} )

