#include <TestCases.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace testCases
{

// Refactorings not requiring diffs are those that are not widely supported across tools and
// therefore specific to only one or two tools.
std::vector<Test> g_tests{
    {"Add Block Delimiter", "ABD"},
    {"Add Override", "AO"},
    {"Add Parameter", "AP"},
    {"Change Signature", "CS"},
    {"Create Method Stub", "CMS"},
    {"Create Multi-Variable Declaration", "CMVD"},
    {"Create Overload", "CO", false},
    {"Create Setter Method", "CSM", false},
    {"Extract Constant", "EC", false},
    {"Extract Function", "EXF"},
    {"Extract Method", "EM"},
    {"Extract Parameter", "EP", false},
    {"Extract Variable", "EXV"},
    {"Flatten Conditional", "FC"},
    {"Inline Macro", "IM"},
    {"Inline Type Alias", "ITA"},
    {"Inline Variable", "IV"},
    {"Make Method Static", "MMS"},
    {"Move Implementation to Source File", "MISF"},
    {"Move Method", "MM", false},
    {"Remove Block Delimiter", "RBD"},
    {"Remove Unused Includes", "RUI"},
    {"Remove Unused Parameter", "RUP"},
    {"Rename", "R"},
    {"Reorder Parameters", "RP"},
    {"Replace auto With Type", "RAWT"},
    {"Replace auto_ptr With unique_ptr", "RAWU"},
    {"Replace If With Ternary", "RIT"},
    {"Replace Iterative For With Range For", "RIRF"},
    {"Replace NULL/0 With nullptr", "RZNP"},
    {"Replace String Literal With Raw String Literal", "RAW"},
    {"Replace Ternary With If", "RTI"},
    {"Replace typedef With Type Alias", "RTDTA"},
    {"Replace Type With auto", "RTWA"},
    {"Reverse Conditional", "RC"},
    {"Simplify Boolean Expression", "SB"},
    {"Split Initialization From Declaration", "SID"},
    {"Split Multi-Variable Declaration", "SMVD"},
};

std::vector<std::string> g_allTestCases;
std::string g_currentFile;
int g_currentLine{};
std::vector<std::string> g_errors;

void Test::checkLabel(std::string_view label, std::string_view desc)
{
    if (std::find(g_allTestCases.begin(), g_allTestCases.end(), label) != g_allTestCases.end())
    {
        g_errors.emplace_back(g_currentFile + "(" + std::to_string(g_currentLine) + "): Duplicate test label "
                              + std::string{label});
        return;
    }
    const std::string_view prefix = label.substr(0, label.find_first_of("0123456789"));
    const auto pos = std::find_if(
        std::begin(g_tests), std::end(g_tests), [&](const Test &test) { return test.getPrefix() == prefix; });
    if (pos == std::end(g_tests))
    {
        g_errors.emplace_back(g_currentFile + "(" + std::to_string(g_currentLine) + "): Unknown test prefix "
                              + std::string{prefix});
        return;
    }
    pos->m_cases.emplace_back(label);
    g_allTestCases.emplace_back(label);
    if (desc == "(deprecated)")
    {
        pos->m_deprecatedCases.emplace_back(label);
    }
}

void Test::scanTestCaseLine(std::string_view line)
{
    if (size_t pos = line.find("#TEST#"); pos != std::string::npos)
    {
        const size_t begin = line.find_first_not_of(" \t", line.find_first_of(' ', pos));
        const size_t end = line.find_first_of(' ', begin);
        const std::string_view label = line.substr(begin, end - begin);
        const std::string_view desc =
            end != std::string_view::npos ? line.substr(line.find_first_not_of(' ', end)) : "";
        checkLabel(label, desc);
    }
}

void Test::scanTestCaseFile(std::filesystem::path path)
{
    std::ifstream file(path.string());
    g_currentFile = path.string();
    g_currentLine = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        ++g_currentLine;
        scanTestCaseLine(line);
    }
}

void Test::scanTestCaseDirectory(std::filesystem::path dir)
{
    for (auto &entry : std::filesystem::directory_iterator(dir))
    {
        if (is_directory(entry))
        {
            scanTestCaseDirectory(entry);
        }
        else
        {
            scanTestCaseFile(entry);
        }
    }
}

void Test::sortTestCases()
{
    const auto extractCaseNum = [](const std::string &label)
    { return std::stoi(label.substr(label.find_first_of("0123456789"))); };
    for (Test &test : g_tests)
    {
        std::sort(test.m_cases.begin(),
                  test.m_cases.end(),
                  [&](const std::string &lhs, const std::string &rhs)
                  { return extractCaseNum(lhs) < extractCaseNum(rhs); });
    }
}

bool Test::isDeprecatedLabel(const std::string &label) const
{
    return std::find(m_deprecatedCases.begin(), m_deprecatedCases.end(), label) != m_deprecatedCases.end();
}

const std::vector<Test> &getTests()
{
    return g_tests;
}

const Test &getTestForPrefix(std::string_view prefix)
{
    auto pos =
        std::find_if(g_tests.begin(), g_tests.end(), [&](const Test &test) { return test.getPrefix() == prefix; });
    if (pos == g_tests.end())
    {
        throw std::runtime_error("Unknown test prefix " + std::string(prefix));
    }
    return *pos;
}

std::vector<std::string> Test::scanTestDirectory(std::filesystem::path dir)
{
    scanTestCaseDirectory(dir);
    sortTestCases();
    return g_errors;
}

const std::string &Test::getPrefixForTestName(const std::string &name)
{
    auto pos = std::find_if(
        std::begin(g_tests), std::end(g_tests), [&name](const Test &test) { return test.getName() == name; });
    if (pos == std::end(g_tests))
    {
        throw std::runtime_error("Unknown test name " + name);
    }
    return pos->getPrefix();
}

}    // namespace testCases
