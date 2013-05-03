/*
 * The internal libcpath header
 *
 * Copyright (c) 2011-2013, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBVSHADOWTOOLS_LIBCPATH_H )
#define _LIBVSHADOWTOOLS_LIBCPATH_H

#include <common.h>

/* Define HAVE_LOCAL_LIBCPATH for local use of libcpath
 */
#if defined( HAVE_LOCAL_LIBCPATH )

#include <libcpath_definitions.h>
#include <libcpath_narrow_split_string.h>
#include <libcpath_narrow_string.h>
#include <libcpath_path.h>
#include <libcpath_types.h>
#include <libcpath_wide_split_string.h>
#include <libcpath_wide_string.h>

#elif defined( HAVE_LIBCPATH_H )

/* If libtool DLL support is enabled set LIBCPATH_DLL_IMPORT
 * before including libcpath.h
 */
#if defined( _WIN32 ) && defined( DLL_IMPORT )
#define LIBCPATH_DLL_IMPORT
#endif

#include <libcpath.h>

#else
#error Missing libcpath.h
#endif

#endif

