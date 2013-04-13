/*
 * Copyright 2012 iamrekcah
 *
 * This file is part of wol.
 *
 * wol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * wol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wol.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * secure.h
 *
 * Settings for securing this linux program.
 */

#ifndef SECURE_H
#define SECURE_H

/*
 * Core dumps are a security risk because they leave the contents
 * of memory in text files.
 *
 * This disables the use of core dumps.
 */
void
no_core_dumps();

#endif /* SECURE_H */
