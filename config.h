/*
 *    robot assembler - robot assembly language
 *    Copyright (C) 1996-2002  Elias Martenson (elias-m@algonet.se)
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CONFIG_H
#define CONFIG_H

/*
 *  Define this if your X libraries have support for
 *  the MULTI_BUFFER extension
 */
#define USE_MULTI_BUFFER_EXTENSION

/*
 *  Define this if your X libraries have support for
 *  the DOUBLE-BUFFER extension
 */
#define USE_DOUBLE_BUFFER_EXTENSION

/*
 *  Define this if dividing a double by zero causes a core dump
 *  instead of returning +/-Inf
 */
/* #define STUPID_MATH */

#endif
