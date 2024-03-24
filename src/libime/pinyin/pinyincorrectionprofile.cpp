/*
 * SPDX-FileCopyrightText: 2024-2024 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include "pinyincorrectionprofile.h"
#include "pinyindata.h"
#include "pinyinencoder.h"
#include <string>

namespace libime {

namespace {

/*
 * Helper function to create mapping based on keyboard rows.
 * Function assume that the key can only be corrected to the key adjcent to it.
 */
std::unordered_map<char, std::vector<char>>
mappingFromRows(const std::vector<std::string> &rows) {
    std::unordered_map<char, std::vector<char>> result;
    for (const auto &row : rows) {
        for (size_t i = 0; i < row.size(); i++) {
            std::vector<char> items;
            if (i > 0) {
                items.push_back(row[i - 1]);
            }
            if (i + 1 < row.size()) {
                items.push_back(row[i + 1]);
            }
            result[row[i]] = std::move(items);
        }
    }
    return result;
}

std::string row1("qwertyuiop");
std::string row2("asdfghjkl");
std::string row3("zxcvbnm");

/*
 * Helper function to create mapping based on keyboard columns.
 * Function assume that the key can only be corrected to the key adjcent to it.
 * For example, if the input is {"qwa"}, the output will be {'q': ['a'], 'w': ['a'], 'a': ['q', 'w']}.
 * Char in row 1 can only be corrected to char in row 2, char in row 2 can only be corrected to char in
 * row 1 and row 3, char in row 3 can only be corrected to char in row 2.
 */
std::unordered_map<char, std::vector<char>>
mappingFromColumns(const std::vector<std::string> &columns) {
    std::unordered_map<char, std::vector<char>> result;
    for (const auto &column : columns) {
        for (size_t i = 0; i < column.size(); i++) {
            bool isRow1 = row1.find(column[i]) != std::string::npos;
            bool isRow2 = row2.find(column[i]) != std::string::npos;
            bool isRow3 = row3.find(column[i]) != std::string::npos;
            std::vector<char> items;
            if (isRow1) {
                for (size_t j = 0; j < column.size(); j++) {
                    if (row2.find(column[j]) != std::string::npos) {
                        items.push_back(column[j]);
                    }
                }
            } else if (isRow2) {
                for (size_t j = 0; j < column.size(); j++) {
                    if (row1.find(column[j]) != std::string::npos ||
                        row3.find(column[j]) != std::string::npos) {
                        items.push_back(column[j]);
                    }
                }
            } else if (isRow3) {
                for (size_t j = 0; j < column.size(); j++) {
                    if (row2.find(column[j]) != std::string::npos) {
                        items.push_back(column[j]);
                    }
                }
            }
            if (result.find(column[i]) != result.end()) {
                result[column[i]].insert(result[column[i]].end(), items.begin(), items.end());
            } else {
                result[column[i]] = std::move(items);
            }
        }
    }
    return result;
}

std::unordered_map<char, std::vector<char>>
getProfileMapping(BuiltinPinyinCorrectionProfile profile) {
    switch (profile) {
    case BuiltinPinyinCorrectionProfile::Qwerty:
      auto rowCorrection = mappingFromRows({"qwertyuiop", "asdfghjkl", "zxcvbnm"});
      auto columnCorrection = mappingFromColumns({"qwa", "wesz", "erdx", "rtfc", "tygv", "yuhb", "uijn", "iokm", "opl"});
      for (const auto &item : columnCorrection) {
          auto row = rowCorrection.find(item.first);
          if (row != rowCorrection.end()) {
              row->second.insert(row->second.end(), item.second.begin(), item.second.end());
          } else {
              rowCorrection[item.first] = item.second;
          }
      }
      return rowCorrection;
    }

    return {};
}
} // namespace

class PinyinCorrectionProfilePrivate {
public:
    PinyinMap pinyinMap_;
};

PinyinCorrectionProfile::PinyinCorrectionProfile(
    BuiltinPinyinCorrectionProfile profile)
    : PinyinCorrectionProfile(getProfileMapping(profile)) {}

PinyinCorrectionProfile::PinyinCorrectionProfile(
    const std::unordered_map<char, std::vector<char>> &mapping)
    : d_ptr(std::make_unique<PinyinCorrectionProfilePrivate>()) {
    FCITX_D();
    // Fill with the original pinyin map.
    d->pinyinMap_ = getPinyinMapV2();
    if (mapping.empty()) {
        return;
    }
    // Re-map all entry with the correction mapping.
    std::vector<PinyinEntry> newEntries;
    for (const auto &item : d->pinyinMap_) {
        for (size_t i = 0; i < item.pinyin().size(); i++) {
            auto chr = item.pinyin()[i];
            auto swap = mapping.find(chr);
            if (swap == mapping.end() || swap->second.empty()) {
                continue;
            }
            auto newEntry = item.pinyin();
            for (auto sub : swap->second) {
                newEntry[i] = sub;
                newEntries.push_back(
                    PinyinEntry(newEntry.data(), item.initial(), item.final(),
                                item.flags() | PinyinFuzzyFlag::Correction));
                newEntry[i] = chr;
            }
        }
    }
    for (const auto &newEntry : newEntries) {
        d->pinyinMap_.insert(newEntry);
    }
}

PinyinCorrectionProfile::~PinyinCorrectionProfile() = default;

const PinyinMap &PinyinCorrectionProfile::pinyinMap() const {
    FCITX_D();
    return d->pinyinMap_;
}

} // namespace libime
