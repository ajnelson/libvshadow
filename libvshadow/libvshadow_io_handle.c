/*
 * Input/Output (IO) handle functions
 *
 * Copyright (C) 2011-2013, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libvshadow_debug.h"
#include "libvshadow_definitions.h"
#include "libvshadow_io_handle.h"
#include "libvshadow_libbfio.h"
#include "libvshadow_libcdata.h"
#include "libvshadow_libcerror.h"
#include "libvshadow_libcnotify.h"
#include "libvshadow_libfdatetime.h"
#include "libvshadow_libfguid.h"
#include "libvshadow_store_descriptor.h"
#include "libvshadow_unused.h"

#include "vshadow_catalog.h"
#include "vshadow_volume.h"

const uint8_t vshadow_vss_identifier[ 16 ] = \
	{ 0x6b, 0x87, 0x08, 0x38, 0x76, 0xc1, 0x48, 0x4e, 0xb7, 0xae, 0x04, 0x04, 0x6e, 0x6c, 0xc7, 0x52 };

const char *vshadow_ntfs_volume_file_system_signature = "NTFS    ";

/* Creates an IO handle
 * Make sure the value io_handle is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libvshadow_io_handle_initialize(
     libvshadow_io_handle_t **io_handle,
     libcerror_error_t **error )
{
	static char *function = "libvshadow_io_handle_initialize";

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( *io_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid IO handle value already set.",
		 function );

		return( -1 );
	}
	*io_handle = memory_allocate_structure(
	              libvshadow_io_handle_t );

	if( *io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create IO handle.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *io_handle,
	     0,
	     sizeof( libvshadow_io_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear IO handle.",
		 function );

		goto on_error;
	}
	( *io_handle )->block_size = 0x4000;

	return( 1 );

on_error:
	if( *io_handle != NULL )
	{
		memory_free(
		 *io_handle );

		*io_handle = NULL;
	}
	return( -1 );
}

/* Frees an IO handle
 * Returns 1 if successful or -1 on error
 */
int libvshadow_io_handle_free(
     libvshadow_io_handle_t **io_handle,
     libcerror_error_t **error )
{
	static char *function = "libvshadow_io_handle_free";

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( *io_handle != NULL )
	{
		memory_free(
		 *io_handle );

		*io_handle = NULL;
	}
	return( 1 );
}

/* Reads the NTFS volume header
 * Returns 1 if successful, 0 if no usable header was found or -1 on error
 */
int libvshadow_io_handle_read_ntfs_volume_header(
     libvshadow_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     size64_t *volume_size,
     libcerror_error_t **error )
{
	vshadow_ntfs_volume_header_t volume_header;

	static char *function                    = "libvshadow_io_handle_read_ntfs_volume_header";
	size64_t backup_volume_size              = 0;
	ssize_t read_count                       = 0;
	off64_t backup_ntfs_volume_header_offset = 0;
	uint64_t total_number_of_sectors         = 0;
	uint32_t cluster_block_size              = 0;
	uint16_t bytes_per_sector                = 0;
	uint8_t sectors_per_cluster_block        = 0;

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( volume_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume size.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading NTFS volume header at offset: 0 (0x00000000)\n",
		 function );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     0,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek NTFS volume header offset: 0.",
		 function );

		return( -1 );
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              (uint8_t *) &volume_header,
	              sizeof( vshadow_ntfs_volume_header_t ),
	              error );

	if( read_count != (ssize_t) sizeof( vshadow_ntfs_volume_header_t ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read NTFS volume header data.",
		 function );

		return( -1 );
	}
	if( memory_compare(
	     volume_header.signature,
	     vshadow_ntfs_volume_file_system_signature,
	     8 ) != 0 )
	{
		*volume_size = 0;

		return( 0 );
	}
	byte_stream_copy_to_uint16_little_endian(
	 volume_header.bytes_per_sector,
	 bytes_per_sector );

	sectors_per_cluster_block = volume_header.sectors_per_cluster_block;

	byte_stream_copy_to_uint16_little_endian(
	 volume_header.total_number_of_sectors_16bit,
	 total_number_of_sectors );

	if( total_number_of_sectors == 0 )
	{
		byte_stream_copy_to_uint32_little_endian(
		 volume_header.total_number_of_sectors_32bit,
		 total_number_of_sectors );
	}
	if( total_number_of_sectors == 0 )
	{
		byte_stream_copy_to_uint64_little_endian(
		 volume_header.total_number_of_sectors_64bit,
		 total_number_of_sectors );
	}
	if( total_number_of_sectors == 0 )
	{
		*volume_size = 0;

		return( 0 );
	}
	cluster_block_size = sectors_per_cluster_block * bytes_per_sector;

	*volume_size  = total_number_of_sectors * bytes_per_sector;
	*volume_size /= cluster_block_size;
	*volume_size += 1;
	*volume_size *= cluster_block_size;

	backup_ntfs_volume_header_offset = *volume_size - 512;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading NTFS backup volume header at offset: %" PRIi64 " (0x%08" PRIx64 ")\n",
		 function,
		 backup_ntfs_volume_header_offset,
		 backup_ntfs_volume_header_offset );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     backup_ntfs_volume_header_offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek NTFS backup volume header offset: %" PRIi64 ".",
		 function,
		 backup_ntfs_volume_header_offset );

		return( -1 );
	}
	read_count = libbfio_handle_read_buffer(
		      file_io_handle,
		      (uint8_t *) &volume_header,
		      sizeof( vshadow_ntfs_volume_header_t ),
		      error );

	if( read_count == 0 )
	{
		*volume_size = 0;

		return( 0 );
	}
	else if( read_count != (ssize_t) sizeof( vshadow_ntfs_volume_header_t ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read NTFS backup volume header data.",
		 function );

		return( -1 );
	}
	if( memory_compare(
	     volume_header.signature,
	     vshadow_ntfs_volume_file_system_signature,
	     8 ) != 0 )
	{
		*volume_size = 0;

		return( 0 );
	}
	byte_stream_copy_to_uint16_little_endian(
	 volume_header.bytes_per_sector,
	 bytes_per_sector );

	sectors_per_cluster_block = volume_header.sectors_per_cluster_block;

	byte_stream_copy_to_uint16_little_endian(
	 volume_header.total_number_of_sectors_16bit,
	 total_number_of_sectors );

	if( total_number_of_sectors == 0 )
	{
		byte_stream_copy_to_uint32_little_endian(
		 volume_header.total_number_of_sectors_32bit,
		 total_number_of_sectors );
	}
	if( total_number_of_sectors == 0 )
	{
		byte_stream_copy_to_uint64_little_endian(
		 volume_header.total_number_of_sectors_64bit,
		 total_number_of_sectors );
	}
	if( total_number_of_sectors == 0 )
	{
		*volume_size = 0;

		return( 0 );
	}
	backup_volume_size  = total_number_of_sectors * bytes_per_sector;
	backup_volume_size /= cluster_block_size;
	backup_volume_size += 1;
	backup_volume_size *= cluster_block_size;

	if( *volume_size != backup_volume_size )
	{
		*volume_size = 0;

		return( 0 );
	}
	return( 1 );
}

/* Reads the volume
 * Returns 1 if successful or -1 on error
 */
int libvshadow_io_handle_read_volume_header(
     libvshadow_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     off64_t file_offset,
     off64_t *catalog_offset,
     libcerror_error_t **error )
{
	vshadow_volume_header_t volume_header;

	static char *function       = "libvshadow_io_handle_read_volume_header";
	ssize_t read_count          = 0;
	uint32_t record_type        = 0;
	uint32_t version            = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	libcstring_system_character_t guid_string[ 48 ];

	libfguid_identifier_t *guid = NULL;
	uint64_t value_64bit        = 0;
	uint32_t value_32bit        = 0;
	int result                  = 0;
#endif

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( catalog_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid catalog offset.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading volume header at offset: %" PRIi64 " (0x%08" PRIx64 ")\n",
		 function,
		 file_offset,
		 file_offset );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     file_offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek volume header offset: %" PRIi64 ".",
		 function,
		 file_offset );

		goto on_error;
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              (uint8_t *) &volume_header,
	              sizeof( vshadow_volume_header_t ),
	              error );

	if( read_count != (ssize_t) sizeof( vshadow_volume_header_t ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read volume header data.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: volume header data:\n",
		 function );
		libcnotify_print_data(
		 (uint8_t *) &volume_header,
		 sizeof( vshadow_volume_header_t ),
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	if( memory_compare(
	     volume_header.identifier,
	     vshadow_vss_identifier,
	     8 ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid volume identifier.",
		 function );

		goto on_error;
	}
	byte_stream_copy_to_uint32_little_endian(
	 volume_header.version,
	 version );

	byte_stream_copy_to_uint32_little_endian(
	 volume_header.record_type,
	 record_type );

	byte_stream_copy_to_uint64_little_endian(
	 volume_header.catalog_offset,
	 *catalog_offset );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( libfguid_identifier_initialize(
		     &guid,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create GUID.",
			 function );

			goto on_error;
		}
		if( libfguid_identifier_copy_from_byte_stream(
		     guid,
		     volume_header.identifier,
		     16,
		     LIBFGUID_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy byte stream to GUID.",
			 function );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libfguid_identifier_copy_to_utf16_string(
			  guid,
			  (uint16_t *) guid_string,
			  48,
			  LIBFGUID_STRING_FORMAT_FLAG_USE_LOWER_CASE,
			  error );
#else
		result = libfguid_identifier_copy_to_utf8_string(
			  guid,
			  (uint8_t *) guid_string,
			  48,
			  LIBFGUID_STRING_FORMAT_FLAG_USE_LOWER_CASE,
			  error );
#endif
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy GUID to string.",
			 function );

			goto on_error;
		}
		libcnotify_printf(
		 "%s: identifier\t\t\t: %" PRIs_LIBCSTRING_SYSTEM "\n",
		 function,
		 guid_string );

		libcnotify_printf(
		 "%s: version\t\t\t: %" PRIu32 "\n",
		 function,
		 version );

		libcnotify_printf(
		 "%s: record type\t\t\t: %" PRIu32 "\n",
		 function,
		 record_type );

		byte_stream_copy_to_uint64_little_endian(
		 volume_header.offset,
		 value_64bit );
		libcnotify_printf(
		 "%s: offset\t\t\t\t: 0x%08" PRIx64 "\n",
		 function,
		 value_64bit );

		byte_stream_copy_to_uint64_little_endian(
		 volume_header.unknown1,
		 value_64bit );
		libcnotify_printf(
		 "%s: unknown1\t\t\t: 0x%08" PRIx64 "\n",
		 function,
		 value_64bit );

		byte_stream_copy_to_uint64_little_endian(
		 volume_header.unknown2,
		 value_64bit );
		libcnotify_printf(
		 "%s: unknown2\t\t\t: 0x%08" PRIx64 "\n",
		 function,
		 value_64bit );

		libcnotify_printf(
		 "%s: catalog offset\t\t\t: 0x%08" PRIx64 "\n",
		 function,
		 *catalog_offset );

		byte_stream_copy_to_uint64_little_endian(
		 volume_header.maximum_size,
		 value_64bit );
		libcnotify_printf(
		 "%s: maximum size\t\t\t: %" PRIu64 "\n",
		 function,
		 value_64bit );

		if( libfguid_identifier_copy_from_byte_stream(
		     guid,
		     volume_header.volume_identifier,
		     16,
		     LIBFGUID_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy byte stream to GUID.",
			 function );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libfguid_identifier_copy_to_utf16_string(
			  guid,
			  (uint16_t *) guid_string,
			  48,
			  LIBFGUID_STRING_FORMAT_FLAG_USE_LOWER_CASE,
			  error );
#else
		result = libfguid_identifier_copy_to_utf8_string(
			  guid,
			  (uint8_t *) guid_string,
			  48,
			  LIBFGUID_STRING_FORMAT_FLAG_USE_LOWER_CASE,
			  error );
#endif
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy GUID to string.",
			 function );

			goto on_error;
		}
		libcnotify_printf(
		 "%s: volume identifier\t\t: %" PRIs_LIBCSTRING_SYSTEM "\n",
		 function,
		 guid_string );

		if( libfguid_identifier_copy_from_byte_stream(
		     guid,
		     volume_header.store_volume_identifier,
		     16,
		     LIBFGUID_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy byte stream to GUID.",
			 function );

			goto on_error;
		}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
		result = libfguid_identifier_copy_to_utf16_string(
			  guid,
			  (uint16_t *) guid_string,
			  48,
			  LIBFGUID_STRING_FORMAT_FLAG_USE_LOWER_CASE,
			  error );
#else
		result = libfguid_identifier_copy_to_utf8_string(
			  guid,
			  (uint8_t *) guid_string,
			  48,
			  LIBFGUID_STRING_FORMAT_FLAG_USE_LOWER_CASE,
			  error );
#endif
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy GUID to string.",
			 function );

			goto on_error;
		}
		libcnotify_printf(
		 "%s: store volume identifier\t: %" PRIs_LIBCSTRING_SYSTEM "\n",
		 function,
		 guid_string );

		if( libfguid_identifier_free(
		     &guid,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free GUID.",
			 function );

			goto on_error;
		}
		byte_stream_copy_to_uint32_little_endian(
		 volume_header.unknown3,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown3\t\t\t: 0x%08" PRIx64 "\n",
		 function,
		 value_32bit );

		libcnotify_printf(
		 "%s: unknown4:\n",
		 function );
		libcnotify_print_data(
		 volume_header.unknown4,
		 412,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	if( ( version != 1 )
	 && ( version != 2 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported version: %" PRIu32 ".",
		 function,
		 version );

		return( -1 );
	}
	if( record_type != LIBVSHADOW_RECORD_TYPE_VOLUME_HEADER )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported record type: %" PRIu32 ".",
		 function,
		 version );

		return( -1 );
	}
	return( 1 );

on_error:
#if defined( HAVE_DEBUG_OUTPUT )
	if( guid != NULL )
	{
		libfguid_identifier_free(
		 &guid,
		 NULL );
	}
#endif
	return( -1 );
}

/* Reads the catalog
 * Returns 1 if successful or -1 on error
 */
int libvshadow_io_handle_read_catalog(
     libvshadow_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     off64_t file_offset,
     size64_t *volume_size,
     libcdata_array_t *store_descriptors_array,
     libcerror_error_t **error )
{
	libvshadow_store_descriptor_t *last_store_descriptor = NULL;
	libvshadow_store_descriptor_t *store_descriptor      = NULL;
	uint8_t *catalog_block_data                          = NULL;
	static char *function                                = "libvshadow_io_handle_read_catalog";
	off64_t next_offset                                  = 0;
	size_t catalog_block_offset                          = 0;
	size_t catalog_block_size                            = 0;
	ssize_t read_count                                   = 0;
	uint64_t catalog_entry_type                          = 0;
	uint32_t record_type                                 = 0;
	uint32_t version                                     = 0;
	int result                                           = 0;
	int store_descriptor_index                           = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	libcstring_system_character_t guid_string[ 48 ];

	libfguid_identifier_t *guid                          = NULL;
	uint64_t value_64bit                                 = 0;
#endif

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( volume_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume size.",
		 function );

		return( -1 );
	}
	catalog_block_data = (uint8_t *) memory_allocate(
	                                  sizeof( uint8_t ) * io_handle->block_size );

	if( catalog_block_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to catalog block data.",
		 function );

		goto on_error;
	}
	do
	{
		catalog_block_size = io_handle->block_size;

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: reading catalog block at offset: %" PRIi64 " (0x%08" PRIx64 ")\n",
			 function,
			 file_offset,
			 file_offset );
		}
#endif
		if( libbfio_handle_seek_offset(
		     file_io_handle,
		     file_offset,
		     SEEK_SET,
		     error ) == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to seek catalog block offset: %" PRIi64 ".",
			 function,
			 file_offset );

			goto on_error;
		}
		read_count = libbfio_handle_read_buffer(
			      file_io_handle,
			      catalog_block_data,
			      catalog_block_size,
			      error );

		if( read_count != (ssize_t) catalog_block_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read catalog block data.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: catalog header data:\n",
			 function );
			libcnotify_print_data(
			 catalog_block_data,
			 sizeof( vshadow_catalog_header_t ),
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
#endif
		if( memory_compare(
		     ( (vshadow_catalog_header_t *) catalog_block_data )->identifier,
		     vshadow_vss_identifier,
		     8 ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid catalog header identifier.",
			 function );

			goto on_error;
		}
		byte_stream_copy_to_uint32_little_endian(
		 ( (vshadow_catalog_header_t *) catalog_block_data )->version,
		 version );

		byte_stream_copy_to_uint32_little_endian(
		 ( (vshadow_catalog_header_t *) catalog_block_data )->record_type,
		 record_type );

		byte_stream_copy_to_uint64_little_endian(
		 ( (vshadow_catalog_header_t *) catalog_block_data )->next_offset,
		 next_offset );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( libfguid_identifier_initialize(
			     &guid,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to create GUID.",
				 function );

				goto on_error;
			}
			if( libfguid_identifier_copy_from_byte_stream(
			     guid,
			     ( (vshadow_catalog_header_t *) catalog_block_data )->identifier,
			     16,
			     LIBFGUID_ENDIAN_LITTLE,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy byte stream to GUID.",
				 function );

				goto on_error;
			}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
			result = libfguid_identifier_copy_to_utf16_string(
				  guid,
				  (uint16_t *) guid_string,
				  48,
				  LIBFGUID_STRING_FORMAT_FLAG_USE_LOWER_CASE,
				  error );
#else
			result = libfguid_identifier_copy_to_utf8_string(
				  guid,
				  (uint8_t *) guid_string,
				  48,
				  LIBFGUID_STRING_FORMAT_FLAG_USE_LOWER_CASE,
				  error );
#endif
			if( result != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy GUID to string.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "%s: identifier\t\t\t\t: %" PRIs_LIBCSTRING_SYSTEM "\n",
			 function,
			 guid_string );

			libcnotify_printf(
			 "%s: version\t\t\t\t: %" PRIu32 "\n",
			 function,
			 version );

			libcnotify_printf(
			 "%s: record type\t\t\t\t: %" PRIu32 "\n",
			 function,
			 record_type );

			byte_stream_copy_to_uint64_little_endian(
			 ( (vshadow_catalog_header_t *) catalog_block_data )->relative_offset,
			 value_64bit );
			libcnotify_printf(
			 "%s: relative offset\t\t\t: 0x%08" PRIx64 "\n",
			 function,
			 value_64bit );

			byte_stream_copy_to_uint64_little_endian(
			 ( (vshadow_catalog_header_t *) catalog_block_data )->offset,
			 value_64bit );
			libcnotify_printf(
			 "%s: offset\t\t\t\t: 0x%08" PRIx64 "\n",
			 function,
			 value_64bit );

			libcnotify_printf(
			 "%s: next offset\t\t\t\t: 0x%08" PRIx64 "\n",
			 function,
			 next_offset );

			if( libfguid_identifier_free(
			     &guid,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free GUID.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "%s: unknown1:\n",
			 function );
			libcnotify_print_data(
			 ( (vshadow_catalog_header_t *) catalog_block_data )->unknown1,
			 80,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
#endif
		if( version != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported version: %" PRIu32 ".",
			 function,
			 version );

			return( -1 );
		}
		if( record_type != LIBVSHADOW_RECORD_TYPE_CATALOG )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported record type: %" PRIu32 ".",
			 function,
			 version );

			return( -1 );
		}
		catalog_block_offset = sizeof( vshadow_catalog_header_t );
		catalog_block_size  -= sizeof( vshadow_catalog_header_t );

		while( catalog_block_offset < catalog_block_size )
		{
			if( store_descriptor == NULL )
			{
				if( libvshadow_store_descriptor_initialize(
				     &store_descriptor,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
					 "%s: unable to create store descriptor.",
					 function );

					goto on_error;
				}
			}
			result = libvshadow_store_descriptor_read_catalog_entry(
			          store_descriptor,
			          &( catalog_block_data[ catalog_block_offset ] ),
			          catalog_block_size,
			          &catalog_entry_type,
			          error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read catalog entry.",
				 function );

				goto on_error;
			}
			if( catalog_entry_type == 2 )
			{
				if( libcdata_array_insert_entry(
				     store_descriptors_array,
				     &store_descriptor_index,
				     (intptr_t *) store_descriptor,
				     (int(*)(intptr_t *, intptr_t *, libcerror_error_t **)) &libvshadow_store_descriptor_compare_by_creation_time,
				     LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
					 "%s: unable to append store descriptor to array.",
					 function );

					goto on_error;
				}
				if( ( *volume_size == 0 )
				 && ( store_descriptor_index == 0 ) )
				{
					*volume_size = store_descriptor->volume_size;
				}
#if defined( HAVE_DEBUG_OUTPUT )
				else if( libcnotify_verbose != 0 )
				{
					if( *volume_size != store_descriptor->volume_size )
					{
						libcnotify_printf(
						 "%s: store descriptor: %d - mismatch in volume size: %" PRIu64 " (expected: %" PRIu64 ").",
						 function,
						 store_descriptor_index,
						 store_descriptor->volume_size,
						 *volume_size );
					}
				}
#endif
				last_store_descriptor = store_descriptor;
				store_descriptor      = NULL;
			}
			else if( catalog_entry_type == 3 )
			{
				if( ( last_store_descriptor != NULL )
				 && ( memory_compare(
				       store_descriptor->identifier,
				       last_store_descriptor->identifier,
				       16 ) != 0 ) )
				{
					result = libcdata_array_get_entry_by_value(
					          store_descriptors_array,
					          (intptr_t *) store_descriptor,
					          (int (*)(intptr_t *, intptr_t *, libcerror_error_t **)) &libvshadow_store_descriptor_compare_by_identifier,
					          (intptr_t **) &last_store_descriptor,
					          error );

					if( result == -1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve number of store descriptors.",
						 function );

						goto on_error;
					}
				}
/* TODO look for the last store decriptor ? */
				if( last_store_descriptor != NULL )
				{
					last_store_descriptor->store_block_list_offset       = store_descriptor->store_block_list_offset;
					last_store_descriptor->store_header_offset           = store_descriptor->store_header_offset;
					last_store_descriptor->store_block_range_list_offset = store_descriptor->store_block_range_list_offset;
					last_store_descriptor->store_bitmap_offset           = store_descriptor->store_bitmap_offset;
					last_store_descriptor->store_previous_bitmap_offset  = store_descriptor->store_previous_bitmap_offset;
				}
#if defined( HAVE_DEBUG_OUTPUT )
				else
				{
					libcnotify_printf(
					 "%s: missing last store descriptor.\n",
					 function );
				}
#endif
			}
			catalog_block_offset += (size_t) 128;
			catalog_block_size   -= (size_t) 128;
		}
		file_offset = next_offset;
	}
	while( file_offset != 0 );

	if( store_descriptor != NULL )
	{
		if( libvshadow_store_descriptor_free(
		     &store_descriptor,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free store descriptor.",
			 function );

			goto on_error;
		}
	}
	memory_free(
	 catalog_block_data );

	return( 1 );

on_error:
#if defined( HAVE_DEBUG_OUTPUT )
	if( guid != NULL )
	{
		libfguid_identifier_free(
		 &guid,
		 NULL );
	}
#endif
	if( store_descriptor != NULL )
	{
		libvshadow_store_descriptor_free(
		 &store_descriptor,
		 NULL );
	}
	if( catalog_block_data != NULL )
	{
		memory_free(
		 catalog_block_data );
	}
	return( -1 );
}

