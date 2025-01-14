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

char const *const g_diffPlaceholder = "AWAITING TEST RESULTS";

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
std::filesystem::path g_currentFile;
int g_currentLine{};
std::filesystem::path g_scannedTestCaseDir;
std::vector<std::string> g_errors;

void Test::checkLabel(std::string_view label, std::string_view desc)
{
    if (std::find(g_allTestCases.begin(), g_allTestCases.end(), label) != g_allTestCases.end())
    {
        g_errors.emplace_back(g_currentFile.string() + "(" + std::to_string(g_currentLine) + "): Duplicate test label "
                              + std::string{label});
        return;
    }
    const std::string_view prefix = label.substr(0, label.find_first_of("0123456789"));
    const auto test = std::find_if(
        std::begin(g_tests), std::end(g_tests), [&](const Test &test) { return test.getPrefix() == prefix; });
    if (test == std::end(g_tests))
    {
        g_errors.emplace_back(g_currentFile.string() + "(" + std::to_string(g_currentLine) + "): Unknown test prefix "
                              + std::string{prefix});
        return;
    }
    test->m_cases.emplace_back(label);
    if (!desc.empty() && desc[0] == '=')
    {
        const auto aliasEnd = desc.find_first_of(' ');
        const std::string_view aliasLabel = desc.substr(1, aliasEnd - 1);
        auto it = std::find_if(test->m_aliases.begin(),
                               test->m_aliases.end(),
                               [&](const TestCaseAlias &alias) { return alias.master == label; });
        if (it == test->m_aliases.end())
        {
            test->m_aliases.emplace_back(TestCaseAlias{std::string{label}, {std::string{aliasLabel}}});
        }
        else
        {
            it->aliases.emplace_back(aliasLabel);
        }
        desc = desc.substr(desc.find_first_not_of(' ', aliasEnd));
    }
    g_allTestCases.emplace_back(label);
    if (desc == "(deprecated)")
    {
        test->m_deprecatedCases.emplace_back(label);
    }
    if (std::find(test->m_paths.begin(), test->m_paths.end(), g_currentFile) == test->m_paths.end())
    {
        test->m_paths.push_back(g_currentFile);
    }
    test->m_locations.push_back(TestCaseLocation{std::string{label}, g_currentFile, g_currentLine});
}

std::string_view getTestCaseLabel(std::string_view line)
{
    if (size_t pos = line.find("#TEST#"); pos != std::string::npos)
    {
        const size_t begin = line.find_first_not_of(" \t", line.find_first_of(' ', pos));
        const size_t end = line.find_first_of(' ', begin);
        const std::string_view label = line.substr(begin, end - begin);
        return label;
    }
    return {};
}

std::string_view getTestCaseAlias(std::string_view line)
{
    if (const std::string_view label = getTestCaseLabel(line); !label.empty())
    {
        if (const size_t pos = line.find_first_not_of(' ', line.find(label) + label.length());
            pos != std::string_view::npos)
        {
            if (line[pos] == '=')
            {
                const size_t end = line.find_first_of(' ', pos);
                return line.substr(pos + 1, end - pos - 1);
            }
        }
    }
    return {};
}

void Test::scanTestCaseLine(std::string_view line)
{
    const std::string_view label = getTestCaseLabel(line);
    if (label.empty())
    {
        return;
    }

    const size_t begin = line.find_first_not_of(" \t", line.find(label) + label.length() - 1);
    const size_t end = line.find_first_of(' ', begin);
    const std::string_view desc = end != std::string_view::npos ? line.substr(line.find_first_not_of(' ', end)) : "";
    checkLabel(label, desc);
}

void Test::scanTestCaseFile(const std::filesystem::path &path)
{
    std::ifstream file(path.string());
    g_currentFile = path;
    g_currentLine = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        ++g_currentLine;
        scanTestCaseLine(line);
    }
}

// a < b
//  if a's filename < b's filename
//  or
//  they have the same filename and a is a header file and b is a source file
bool compareSourcePath(const std::filesystem::path &lhs, const std::filesystem::path &rhs)
{
    return lhs.stem() < rhs.stem()
        || (lhs.stem() == rhs.stem() && lhs.extension().string() == ".h"
            && (rhs.extension().string() == ".cpp" || rhs.extension().string() == ".c"));
};

class TestCaseFileLister
{
public:
    TestCaseFileLister(const std::filesystem::path &dir)
    {
        scanDirectory(dir);
        std::sort(m_paths.begin(), m_paths.end(), compareSourcePath);
    }
    const std::vector<std::filesystem::path> &getPaths() const
    {
        return m_paths;
    }

private:
    void scanDirectory(const std::filesystem::path &dir)
    {
        for (auto &entry : std::filesystem::directory_iterator(dir))
        {
            if (is_directory(entry))
            {
                scanDirectory(entry);
            }
            else
            {
                m_paths.push_back(entry);
            }
        }
    }

    std::vector<std::filesystem::path> m_paths;
};

void Test::scanTestCaseDirectory(const std::filesystem::path &dir)
{
    const TestCaseFileLister lister(dir);
    for (const std::filesystem::path &path : lister.getPaths())
    {
        scanTestCaseFile(path);
    }
}

void Test::sortTestCases()
{
    const auto extractCaseNum = [](const std::string &label)
    { return std::stoi(label.substr(label.find_first_of("0123456789"))); };
    for (Test &test : g_tests)
    {
        int caseNum{};
        for (const std::string &label : test.m_cases)
        {
            ++caseNum;
            if (extractCaseNum(label) != caseNum)
            {
                test.m_casesConsecutive = false;
                break;
            }
        }
        std::sort(test.m_cases.begin(),
                  test.m_cases.end(),
                  [&](const std::string &lhs, const std::string &rhs)
                  { return extractCaseNum(lhs) < extractCaseNum(rhs); });
        std::sort(test.m_locations.begin(),
                  test.m_locations.end(),
                  [&](const TestCaseLocation &lhs, const TestCaseLocation &rhs)
                  { return extractCaseNum(lhs.label) < extractCaseNum(rhs.label); });
    }
}

void Test::sortTestPaths()
{
    for (Test &test : g_tests)
    {
        if (test.m_paths.size() > 1)
        {
            std::sort(test.m_paths.begin(), test.m_paths.end(), compareSourcePath);
        }
    }
}

bool Test::isDeprecatedCase(const std::string &label) const
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

std::vector<std::string> Test::scanTestDirectory(const std::filesystem::path &dir)
{
    if (g_scannedTestCaseDir.empty())
    {
        scanTestCaseDirectory(dir);
        sortTestPaths();
        sortTestCases();
        g_scannedTestCaseDir = dir;
    }
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
