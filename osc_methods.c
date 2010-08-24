/**
 * Copyright (c) 2010 William Light <will@illest.net>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define _GNU_SOURCE /* for asprintf */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lo/lo.h>
#include <monome.h>

#include "serialosc.h"
#include "osc.h"


static int osc_clear_handler(const char *path, const char *types,
							 lo_arg **argv, int argc,
							 lo_message data, void *user_data) {
	monome_t *monome = user_data;
	int mode = (argc) ? argv[0]->i : 0;

	puts("beep");
	return monome_clear(monome, mode);
}

static int osc_intensity_handler(const char *path, const char *types,
								 lo_arg **argv, int argc,
								 lo_message data, void *user_data) {
	monome_t *monome = user_data;
	int intensity = (argc) ? argv[0]->i : 0xF;

	return monome_intensity(monome, intensity);
}

static int osc_led_handler(const char *path, const char *types,
						   lo_arg **argv, int argc,
						   lo_message data, void *user_data) {
	monome_t *monome = user_data;

	if( argv[2]->i )
		return monome_led_on(monome, argv[0]->i, argv[1]->i);
	else
		return monome_led_off(monome, argv[0]->i, argv[1]->i);
}


static int bail_if_false(int well_is_it) {
	if( well_is_it )
		return 0;

	fprintf(stderr, "aieee, could not allocate memory in "
			"osc_register_methods(), bailing out!\n");
	_exit(EXIT_FAILURE);

	return 1;
}

void osc_register_methods(sosc_state_t *state) {
	char *prefix, *cmd_buf;
	monome_t *monome;
	lo_server srv;

	prefix = state->osc_prefix;
	monome = state->monome;
	srv = state->server;

#define METHOD(path) for( cmd_buf = NULL,\
		 bail_if_false(asprintf(&cmd_buf, "%s/" path, prefix) > 0); \
		 cmd_buf; free(cmd_buf), cmd_buf = NULL )

#define REGISTER(typetags, cb) \
	lo_server_add_method(srv, cmd_buf, typetags, cb, monome)


	METHOD("clear") {
		REGISTER("", osc_clear_handler);
		REGISTER("i", osc_clear_handler);
	}

	METHOD("intensity") {
		REGISTER("", osc_intensity_handler);
		REGISTER("i", osc_intensity_handler);
	}

	METHOD("led")
		REGISTER("iii", osc_led_handler);

#undef REGISTER
#undef METHOD
}