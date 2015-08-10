/*
 * zOS
 * Copyright (C) 2014 - 2015 Baptiste Covolato
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zOS.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file    userland/bin/init/src/exec.h
 * \brief   Function prototypes init configuration execution
 *
 * \author  Baptiste Covolato
 */
#ifndef INIT_EXEC_H
# define INIT_EXEC_H

# include "config.h"

int init_conf_execute(struct init_conf *config);

#endif /* !INIT_EXEC_H */
