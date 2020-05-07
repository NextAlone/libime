/*
 * Copyright (C) 2017~2017 by CSSlayer
 * wengxt@gmail.com
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; see the file COPYING. If not,
 * see <http://www.gnu.org/licenses/>.
 */

#include "libime/table/tablebaseddictionary.h"
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <tuple>

void usage(const char *argv0) {
    std::cout << "Usage: " << argv0 << " [-du] <source> <dest>" << std::endl
              << "-d: Dump binary to text" << std::endl
              << "-u: User dict" << std::endl
              << "-h: Show this help" << std::endl;
}

int main(int argc, char *argv[]) {

    bool dump = false;
    bool user = false;
    int c;
    while ((c = getopt(argc, argv, "dhu")) != -1) {
        switch (c) {
        case 'd':
            dump = true;
            break;
        case 'u':
            user = true;
            break;
        case 'h':
            usage(argv[0]);
            return 0;
        default:
            usage(argv[0]);
            return 1;
        }
    }

    if (optind + 2 != argc) {
        usage(argv[0]);
        return 1;
    }
    using namespace libime;
    TableBasedDictionary dict;

    std::ifstream fin;
    std::istream *in;
    if (strcmp(argv[optind], "-") == 0) {
        in = &std::cin;
    } else {
        fin.open(argv[optind], std::ios::in | std::ios::binary);
        in = &fin;
    }

    if (user) {
        dict.loadUser(*in, dump ? TableFormat::Binary : TableFormat::Text);
    } else {
        dict.load(*in, dump ? TableFormat::Binary : TableFormat::Text);
    }

    std::ofstream fout;
    std::ostream *out;
    if (strcmp(argv[optind + 1], "-") == 0) {
        out = &std::cout;
    } else {
        fout.open(argv[optind + 1], std::ios::out | std::ios::binary);
        out = &fout;
    }

    if (user) {
        dict.saveUser(*out, dump ? TableFormat::Text : TableFormat::Binary);
    } else {
        dict.save(*out, dump ? TableFormat::Text : TableFormat::Binary);
    }
    return 0;
}
