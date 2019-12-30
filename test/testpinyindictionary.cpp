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

#include "libime/pinyin/pinyindictionary.h"
#include "libime/pinyin/pinyinencoder.h"
#include "testdir.h"
#include "testutils.h"
#include <fcitx-utils/log.h>
#include <sstream>

int main() {
    using namespace libime;
    PinyinDictionary dict;
    dict.load(PinyinDictionary::SystemDict,
              LIBIME_BINARY_DIR "/data/dict.converted", PinyinDictFormat::Text);

    // add a manual dict
    std::stringstream ss;
    ss << "倪辉 ni'hui 0.0";
    dict.load(PinyinDictionary::UserDict, ss, PinyinDictFormat::Text);
    // dict.dump(std::cout);
    char c[] = {static_cast<char>(PinyinInitial::N), 0,
                static_cast<char>(PinyinInitial::H), 0};
    dict.matchWords(c, sizeof(c),
                    [c](std::string_view encodedPinyin, std::string_view hanzi,
                        float cost) {
                        std::cout
                            << PinyinEncoder::decodeFullPinyin(encodedPinyin)
                            << " " << hanzi << " " << cost << std::endl;
                        return true;
                    });

    dict.save(0, LIBIME_BINARY_DIR "/test/testpinyindictionary.dict",
              PinyinDictFormat::Binary);
    return 0;
}
