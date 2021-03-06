/*
 * Python object definition of the libvshadow block
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

#include <common.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( HAVE_WINAPI )
#include <stdlib.h>
#endif

#include "pyvshadow.h"
#include "pyvshadow_block.h"
#include "pyvshadow_libcerror.h"
#include "pyvshadow_libcstring.h"
#include "pyvshadow_libvshadow.h"
#include "pyvshadow_python.h"
#include "pyvshadow_unused.h"

PyMethodDef pyvshadow_block_object_methods[] = {

	/* Functions to access the block values */

	{ "get_original_offset",
	  (PyCFunction) pyvshadow_block_get_original_offset,
	  METH_NOARGS,
	  "get_original_offset() -> Long\n"
	  "\n"
	  "Retrieves the original offset." },

	{ "get_relative_offset",
	  (PyCFunction) pyvshadow_block_get_relative_offset,
	  METH_NOARGS,
	  "get_relative_offset() -> Long\n"
	  "\n"
	  "Retrieves the relative offset." },

	{ "get_offset",
	  (PyCFunction) pyvshadow_block_get_offset,
	  METH_NOARGS,
	  "get_offset() -> Long\n"
	  "\n"
	  "Retrieves the offset." },

	/* Sentinel */
	{ NULL, NULL, 0, NULL }
};

PyGetSetDef pyvshadow_block_object_get_set_definitions[] = {

	{ "original_offset",
	  (getter) pyvshadow_block_get_original_offset,
	  (setter) 0,
	  "The original offset.",
	  NULL },

	{ "relative_offset",
	  (getter) pyvshadow_block_get_relative_offset,
	  (setter) 0,
	  "The relative offset.",
	  NULL },

	{ "offset",
	  (getter) pyvshadow_block_get_offset,
	  (setter) 0,
	  "The offset.",
	  NULL },

	/* Sentinel */
	{ NULL, NULL, NULL, NULL, NULL }
};

PyTypeObject pyvshadow_block_type_object = {
	PyObject_HEAD_INIT( NULL )

	/* ob_size */
	0,
	/* tp_name */
	"pyvshadow.block",
	/* tp_basicsize */
	sizeof( pyvshadow_block_t ),
	/* tp_itemsize */
	0,
	/* tp_dealloc */
	(destructor) pyvshadow_block_free,
	/* tp_print */
	0,
	/* tp_getattr */
	0,
	/* tp_setattr */
	0,
	/* tp_compare */
	0,
	/* tp_repr */
	0,
	/* tp_as_number */
	0,
	/* tp_as_sequence */
	0,
	/* tp_as_mapping */
	0,
	/* tp_hash */
	0,
	/* tp_call */
	0,
	/* tp_str */
	0,
	/* tp_getattro */
	0,
	/* tp_setattro */
	0,
	/* tp_as_buffer */
	0,
        /* tp_flags */
	Py_TPFLAGS_DEFAULT,
	/* tp_doc */
	"pyvshadow block object (wraps libvshadow_block_t)",
	/* tp_traverse */
	0,
	/* tp_clear */
	0,
	/* tp_richcompare */
	0,
	/* tp_weaklistoffset */
	0,
	/* tp_iter */
	0,
	/* tp_iternext */
	0,
	/* tp_methods */
	pyvshadow_block_object_methods,
	/* tp_members */
	0,
	/* tp_getset */
	pyvshadow_block_object_get_set_definitions,
	/* tp_base */
	0,
	/* tp_dict */
	0,
	/* tp_descr_get */
	0,
	/* tp_descr_set */
	0,
	/* tp_dictoffset */
	0,
	/* tp_init */
	(initproc) pyvshadow_block_init,
	/* tp_alloc */
	0,
	/* tp_new */
	0,
	/* tp_free */
	0,
	/* tp_is_gc */
	0,
	/* tp_bases */
	NULL,
	/* tp_mro */
	NULL,
	/* tp_cache */
	NULL,
	/* tp_subclasses */
	NULL,
	/* tp_weaklist */
	NULL,
	/* tp_del */
	0
};

/* Creates a new block object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyvshadow_block_new(
           libvshadow_block_t *block,
           pyvshadow_store_t *store_object )
{
	pyvshadow_block_t *pyvshadow_block = NULL;
	static char *function              = "pyvshadow_block_new";

	if( block == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block.",
		 function );

		return( NULL );
	}
	pyvshadow_block = PyObject_New(
	                   struct pyvshadow_block,
	                   &pyvshadow_block_type_object );

	if( pyvshadow_block == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize block.",
		 function );

		goto on_error;
	}
	if( pyvshadow_block_init(
	     pyvshadow_block ) != 0 )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize block.",
		 function );

		goto on_error;
	}
	pyvshadow_block->block        = block;
	pyvshadow_block->store_object = store_object;

	Py_IncRef(
	 (PyObject *) pyvshadow_block->store_object );

	return( (PyObject *) pyvshadow_block );

on_error:
	if( pyvshadow_block != NULL )
	{
		Py_DecRef(
		 (PyObject *) pyvshadow_block );
	}
	return( NULL );
}

/* Intializes an block object
 * Returns 0 if successful or -1 on error
 */
int pyvshadow_block_init(
     pyvshadow_block_t *pyvshadow_block )
{
	static char *function = "pyvshadow_block_init";

	if( pyvshadow_block == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block.",
		 function );

		return( -1 );
	}
	/* Make sure libvshadow block is set to NULL
	 */
	pyvshadow_block->block = NULL;

	return( 0 );
}

/* Frees an block object
 */
void pyvshadow_block_free(
      pyvshadow_block_t *pyvshadow_block )
{
	char error_string[ PYVSHADOW_ERROR_STRING_SIZE ];

	libcerror_error_t *error = NULL;
	static char *function    = "pyvshadow_block_free";

	if( pyvshadow_block == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block.",
		 function );

		return;
	}
	if( pyvshadow_block->ob_type == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block - missing ob_type.",
		 function );

		return;
	}
	if( pyvshadow_block->ob_type->tp_free == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block - invalid ob_type - missing tp_free.",
		 function );

		return;
	}
	if( pyvshadow_block->block == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block - missing libvshadow block.",
		 function );

		return;
	}
	if( libvshadow_block_free(
	     &( pyvshadow_block->block ),
	     &error ) != 1 )
	{
		if( libcerror_error_backtrace_sprint(
		     error,
		     error_string,
		     PYVSHADOW_ERROR_STRING_SIZE ) == -1 )
                {
			PyErr_Format(
			 PyExc_IOError,
			 "%s: unable to free libvshadow block.",
			 function );
		}
		else
                {
			PyErr_Format(
			 PyExc_IOError,
			 "%s: unable to free libvshadow block.\n%s",
			 function,
			 error_string );
		}
		libcerror_error_free(
		 &error );
	}
	if( pyvshadow_block->store_object != NULL )
	{
		Py_DecRef(
		 (PyObject *) pyvshadow_block->store_object );
	}
	pyvshadow_block->ob_type->tp_free(
	 (PyObject*) pyvshadow_block );
}

/* Retrieves the original offset
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyvshadow_block_get_original_offset(
           pyvshadow_block_t *pyvshadow_block,
           PyObject *arguments PYVSHADOW_ATTRIBUTE_UNUSED )
{
	char error_string[ PYVSHADOW_ERROR_STRING_SIZE ];

	libcerror_error_t *error = NULL;
	static char *function    = "pyvshadow_block_get_original_offset";
	off64_t original_offset  = 0;
	int result               = 0;

	PYVSHADOW_UNREFERENCED_PARAMETER( arguments )

	if( pyvshadow_block == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libvshadow_block_get_original_offset(
	          pyvshadow_block->block,
	          &original_offset,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		if( libcerror_error_backtrace_sprint(
		     error,
		     error_string,
		     PYVSHADOW_ERROR_STRING_SIZE ) == -1 )
		{
			PyErr_Format(
			 PyExc_IOError,
			 "%s: unable to retrieve original offset.",
			 function );
		}
		else
		{
			PyErr_Format(
			 PyExc_IOError,
			 "%s: unable to retrieve original offset.\n%s",
			 function,
			 error_string );
		}
		libcerror_error_free(
		 &error );

		return( NULL );
	}
#if defined( HAVE_LONG_LONG )
	if( original_offset > (off64_t) LLONG_MAX )
	{
		PyErr_Format(
		 PyExc_OverflowError,
		 "%s: original offset value exceeds maximum.",
		 function );

		return( NULL );
	}
	return( PyLong_FromLongLong(
	         (long long) original_offset ) );
#else
	if( original_offset > (off64_t) LONG_MAX )
	{
		PyErr_Format(
		 PyExc_OverflowError,
		 "%s: original offset value exceeds maximum.",
		 function );

		return( NULL );
	}
	return( PyLong_FromLong(
	         (long) original_offset ) );
#endif
}

/* Retrieves the relative offset
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyvshadow_block_get_relative_offset(
           pyvshadow_block_t *pyvshadow_block,
           PyObject *arguments PYVSHADOW_ATTRIBUTE_UNUSED )
{
	char error_string[ PYVSHADOW_ERROR_STRING_SIZE ];

	libcerror_error_t *error = NULL;
	static char *function    = "pyvshadow_block_get_relative_offset";
	off64_t relative_offset  = 0;
	int result               = 0;

	PYVSHADOW_UNREFERENCED_PARAMETER( arguments )

	if( pyvshadow_block == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libvshadow_block_get_relative_offset(
	          pyvshadow_block->block,
	          &relative_offset,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		if( libcerror_error_backtrace_sprint(
		     error,
		     error_string,
		     PYVSHADOW_ERROR_STRING_SIZE ) == -1 )
		{
			PyErr_Format(
			 PyExc_IOError,
			 "%s: unable to retrieve relative offset.",
			 function );
		}
		else
		{
			PyErr_Format(
			 PyExc_IOError,
			 "%s: unable to retrieve relative offset.\n%s",
			 function,
			 error_string );
		}
		libcerror_error_free(
		 &error );

		return( NULL );
	}
#if defined( HAVE_LONG_LONG )
	if( relative_offset > (off64_t) LLONG_MAX )
	{
		PyErr_Format(
		 PyExc_OverflowError,
		 "%s: relative offset value exceeds maximum.",
		 function );

		return( NULL );
	}
	return( PyLong_FromLongLong(
	         (long long) relative_offset ) );
#else
	if( relative_offset > (off64_t) LONG_MAX )
	{
		PyErr_Format(
		 PyExc_OverflowError,
		 "%s: relative offset value exceeds maximum.",
		 function );

		return( NULL );
	}
	return( PyLong_FromLong(
	         (long) relative_offset ) );
#endif
}

/* Retrieves the offset
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyvshadow_block_get_offset(
           pyvshadow_block_t *pyvshadow_block,
           PyObject *arguments PYVSHADOW_ATTRIBUTE_UNUSED )
{
	char error_string[ PYVSHADOW_ERROR_STRING_SIZE ];

	libcerror_error_t *error = NULL;
	static char *function    = "pyvshadow_block_get_offset";
	off64_t offset           = 0;
	int result               = 0;

	PYVSHADOW_UNREFERENCED_PARAMETER( arguments )

	if( pyvshadow_block == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid block.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libvshadow_block_get_offset(
	          pyvshadow_block->block,
	          &offset,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		if( libcerror_error_backtrace_sprint(
		     error,
		     error_string,
		     PYVSHADOW_ERROR_STRING_SIZE ) == -1 )
		{
			PyErr_Format(
			 PyExc_IOError,
			 "%s: unable to retrieve offset.",
			 function );
		}
		else
		{
			PyErr_Format(
			 PyExc_IOError,
			 "%s: unable to retrieve offset.\n%s",
			 function,
			 error_string );
		}
		libcerror_error_free(
		 &error );

		return( NULL );
	}
#if defined( HAVE_LONG_LONG )
	if( offset > (off64_t) LLONG_MAX )
	{
		PyErr_Format(
		 PyExc_OverflowError,
		 "%s: offset value exceeds maximum.",
		 function );

		return( NULL );
	}
	return( PyLong_FromLongLong(
	         (long long) offset ) );
#else
	if( offset > (off64_t) LONG_MAX )
	{
		PyErr_Format(
		 PyExc_OverflowError,
		 "%s: offset value exceeds maximum.",
		 function );

		return( NULL );
	}
	return( PyLong_FromLong(
	         (long) offset ) );
#endif
}

