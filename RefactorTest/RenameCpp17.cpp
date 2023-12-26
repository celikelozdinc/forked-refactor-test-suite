#include "RenameCpp17.h"
#include "Require.h"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

// C++17 language features:
// - lambda capture of *this
// - compile time if constexpr
// - constexpr lambda
// - inline variables
// - structured bindings
// - initializers for if and switch
// - template fold expressions
// - class template argument deduction
// - non-type template parameters declared with auto
// - simplified nested namespaces
// - using-declaration declaring multiple names
// - attribute namespace don't have to repeat

// #TEST#: R810 Rename A
// #TEST#: R811 Rename B
// #TEST#: R812 Rename C
namespace RenameCpp17::A::B::C
{

// #TEST#: R813 Rename x
int x{10};

int y{20};
int z{30};

}    // namespace RenameCpp17::A::B::C

namespace
{

class Foo
{
public:
    int f()
    {
        // #TEST#: R640 Rename local variable fn
        // #TEST#: R641 Rename parameter val
        // #TEST#: R642 Rename use of val
        // #TEST#: R643 Rename use of m_i
        auto fn = [*this](int &val) { val = 3 + m_i; };
        // #TEST#: R644 Rename use of m_i
        m_i = 1;
        // #TEST#: R645 Rename use of fn
        // #TEST#: R646 Rename use of m_i
        fn(m_i);
        // #TEST#: R647 Rename use of m_i
        return m_i;
    }

private:
    int m_i{10};
};

// #TEST#: R648 Rename template parameter T
// #TEST#: R649 Rename template parameter N
template <typename T, size_t N>
// #TEST#: R650 Rename function f
int f()
{
    // #TEST#: R651 Rename use of T
    // #TEST#: R652 Rename use of N
    if constexpr (sizeof(T) == N)
    {
        return 10;
    }
    else
    {
        return -10;
    }
}

void f1()
{
    Foo f;
    REQUIRE_EQUAL(13, f.f());
}

void f2()
{
    // #TEST#: R653 Rename use of f
    REQUIRE_EQUAL(10, (f<std::uint32_t, 4>()));
}

void f3()
{
    // #TEST#: R654 Rename local variable initial
    constexpr int initial{10};
    // #TEST#: R655 Rename local variable fn
    // #TEST#: R656 Rename initial in capture list
    // #TEST#: R657 Rename parameter val
    // #TEST#: R658 Rename use of initial
    // #TEST#: R659 Rename use of val
    constexpr auto fn = [initial](int val) constexpr { return initial + val; };
    int value;
    // #TEST#: R660 Rename use of fn
    if constexpr (15 == fn(5))
    {
        value = 10;
    }
    else
    {
        value = 15;
    }
    // #TEST#: R661 Rename use of fn
    REQUIRE_EQUAL(15, fn(5));
    REQUIRE_EQUAL(10, value);
}

void f4()
{
    // #TEST#: R662 Rename use of g_counter
    ++RenameCpp17::g_counter;
    // #TEST#: R663 Rename use of g_counter
    REQUIRE_EQUAL(1, RenameCpp17::g_counter);
}

void f5()
{
    // binding to arrays
    {
        // #TEST#: R665 Rename variable vals
        int vals[]{1, 2};
        // #TEST#: R666 Rename binding cx
        // #TEST#: R667 Rename binding cy
        // #TEST#: R668 Rename use of vals
        const auto [cx, cy] = vals;
        // #TEST#: R669 Rename use of cx
        REQUIRE_EQUAL(1, cx);
        // #TEST#: R670 Rename use of cy
        REQUIRE_EQUAL(2, cy);
        // #TEST#: R671 Rename binding x
        // #TEST#: R672 Rename binding y
        // #TEST#: R673 Rename use of vals
        auto [x, y] = vals;
        // #TEST#: R674 Rename use of x
        REQUIRE_EQUAL(1, x);
        // #TEST#: R675 Rename use of y
        REQUIRE_EQUAL(2, y);
        // #TEST#: R676 Rename binding vx
        // #TEST#: R677 Rename binding vy
        // #TEST#: R678 Rename use of vals
        auto &[vx, vy] = vals;
        // #TEST#: R679 Rename use of vx
        vx = 4;
        // #TEST#: R680 Rename use of vy
        vy = 5;
        REQUIRE_EQUAL(4, vals[0]);
        REQUIRE_EQUAL(5, vals[1]);
        // #TEST#: R681 Rename binding crx
        // #TEST#: R682 Rename binding cry
        // #TEST#: R683 Rename use of vals
        const auto &[crx, cry] = vals;
        // #TEST#: R684 Rename use of crx
        REQUIRE_EQUAL(4, crx);
        // #TEST#: R685 Rename use of cry
        REQUIRE_EQUAL(5, cry);
    }

    // binding to tuple-like type
    {
        using pair = std::tuple<int, std::string>;
        pair p{1, "one"};
        // #TEST#: R686 Rename binding val
        // #TEST#: R687 Rename binding name
        // #TEST#: R688 Rename use of p
        const auto [val, name] = p;
        REQUIRE_EQUAL(1, val);
        REQUIRE_EQUAL(std::string{"one"}, name);
        // #TEST#: R689 Rename binding val2
        // #TEST#: R690 Rename binding name2
        // #TEST#: R691 Rename use of p
        auto [val2, name2] = p;
        // #TEST#: R692 Rename use of val2
        REQUIRE_EQUAL(1, val2);
        // #TEST#: R693 Rename use of name2
        REQUIRE_EQUAL(std::string{"one"}, name2);
        // #TEST#: R694 Rename binding val3
        // #TEST#: R695 Rename binding name3
        // #TEST#: R696 Rename use of p
        auto &[val3, name3] = p;
        // #TEST#: R697 Rename use of val3
        val3 = 11;
        // #TEST#: R698 Rename use of name3
        name3 = "eleven";
        REQUIRE_EQUAL(11, std::get<0>(p));
        REQUIRE_EQUAL(std::string{"eleven"}, std::get<1>(p));
        // #TEST#: R699 Rename binding val4
        // #TEST#: R700 Rename binding name4
        // #TEST#: R701 Rename use of p
        const auto &[val4, name4] = p;
        // #TEST#: R702 Rename use of val4
        REQUIRE_EQUAL(11, val4);
        // #TEST#: R703 Rename use of name4
        REQUIRE_EQUAL(std::string{"eleven"}, name4);
    }

    // binding to data members
    {
        struct Foo
        {
            int val;
            std::string name;
        };
        Foo f{1, "one"};
        // #TEST#: R704 Rename binding val1
        // #TEST#: R705 Rename binding name1
        // #TEST#: R706 Rename use of f
        const auto [val1, name1] = f;
        // #TEST#: R707 Rename use of val1
        REQUIRE_EQUAL(1, val1);
        // #TEST#: R708 Rename use of name1
        REQUIRE_EQUAL(std::string{"one"}, name1);
        // #TEST#: R709 Rename binding val2
        // #TEST#: R710 Rename binding name2
        // #TEST#: R711 Rename use of f
        const auto &[val2, name2] = f;
        // #TEST#: R712 Rename use of val2
        REQUIRE_EQUAL(1, val2);
        // #TEST#: R713 Rename use of name2
        REQUIRE_EQUAL(std::string{"one"}, name2);
        // #TEST#: R714 Rename binding val3
        // #TEST#: R715 Rename binding name3
        // #TEST#: R716 Rename use of f
        auto &[val3, name3] = f;
        // #TEST#: R717 Rename use of val3
        val3 = 11;
        // #TEST#: R718 Rename use of name3
        name3 = "eleven";
        REQUIRE_EQUAL(11, f.val);
        REQUIRE_EQUAL(std::string{"eleven"}, f.name);
        // #TEST#: R719 Rename binding val4
        // #TEST#: R720 Rename binding name4
        // #TEST#: R721 Rename use of f
        auto [val4, name4] = f;
        // #TEST#: R722 Rename use of val4
        REQUIRE_EQUAL(11, val4);
        // #TEST#: R723 Rename use of name4
        REQUIRE_EQUAL(std::string{"eleven"}, name4);
    }
}

void f6()
{
    int val{6};
    // #TEST#: R724 Rename local variable i
    // #TEST#: R725 Rename use of i
    // #TEST#: R726 Rename use of val
    if (int i = val % 2; i == 0)
    {
        // #TEST#: R727 Rename use of i
        ++i;
        // #TEST#: R728 Rename use of i
        val = i;
    }
    REQUIRE_EQUAL(1, val);

    // #TEST#: R729 Rename local variable i
    // #TEST#: R730 Rename use of i
    switch (int i = val % 2; i)
    {
    case 0:
        // #TEST#: R731 Rename use of i
        ++i;
        // #TEST#: R732 Rename use of i
        val = 100 + i;
        break;
    case 1:
        // #TEST#: R733 Rename use of i
        ++i;
        // #TEST#: R734 Rename use of i
        val = 200 + i;
        break;
    default:
        val = 0;
        break;
    }
    REQUIRE_EQUAL(202, val);
}

// #TEST#: R735 Rename template parameter Args
template <typename... Args>
// #TEST#: R736 Rename function allUnaryRight
// #TEST#: R737 Rename use of Args
// #TEST#: R738 Rename parameter args
bool allUnaryRight(Args... args)
{
    // #TEST#: R739 Rename use of args
    return (args && ...);
}

// #TEST#: R740 Rename template parameter Args
template <typename... Args>
// #TEST#: R741 Rename function allUnaryLeft
// #TEST#: R742 Rename use of Args
// #TEST#: R743 Rename parameter args
bool allUnaryLeft(Args... args)
{
    // #TEST#: R744 Rename use of args
    return (... && args);
}

// #TEST#: R745 Rename template parameter Args
template <typename... Args>
// #TEST#: R746 Rename function allUnaryLeft
// #TEST#: R747 Rename use of Args
// #TEST#: R748 Rename parameter init
// #TEST#: R749 Rename parameter args
bool allBinaryRight(bool init, Args... args)
{
    // #TEST#: R750 Rename use of init
    // #TEST#: R751 Rename use of args
    return (args && ... && init);
}

// #TEST#: R752 Rename template parameter Args
template <typename... Args>
// #TEST#: R753 Rename function allUnaryLeft
// #TEST#: R754 Rename use of Args
// #TEST#: R755 Rename parameter init
// #TEST#: R756 Rename parameter args
bool allBinaryLeft(bool init, Args... args)
{
    // #TEST#: R757 Rename use of init
    // #TEST#: R758 Rename use of args
    return (init && ... && args);
}

void f7()
{
    const bool all1 = allUnaryLeft(true, true, true, true);
    REQUIRE_EQUAL(true, all1);
    const bool all2 = allUnaryRight(true, true, true, true);
    REQUIRE_EQUAL(true, all2);
    const bool all3 = allBinaryLeft(true, true, true, true);
    REQUIRE_EQUAL(true, all3);
    const bool all4 = allBinaryRight(true, true, true, true);
    REQUIRE_EQUAL(true, all4);
}

template <typename Value, typename String>
struct ValueName
{
    ValueName(Value value, String name) : value(value), name(name) {}
    Value value;
    String name;
};

void f8()
{
    float f1{1.0f};
    int i1{10};
    // declarations
    {
        // #TEST#: R759 Rename local variable p
        // #TEST#: R760 Rename use of f1
        // #TEST#: R761 Rename use of i1
        std::pair p{f1, i1};
        // #TEST#: R762 Rename use of p
        REQUIRE_EQUAL(1.0f, p.first);
        // #TEST#: R763 Rename use of p
        REQUIRE_EQUAL(10, p.second);
        // #TEST#: R764 Rename local variable t
        // #TEST#: R765 Rename use of f1
        // #TEST#: R766 Rename use of i1
        std::tuple t{f1, i1};
        // #TEST#: R767 Rename use of t
        REQUIRE_EQUAL(1.0f, std::get<0>(t));
        // #TEST#: R768 Rename use of t
        REQUIRE_EQUAL(10, std::get<1>(t));
    }
    // new expressions
    {
        // #TEST#: R769 Rename local variable p1
        // #TEST#: R770 Rename use of f1
        // #TEST#: R771 Rename use of i1
        const auto *p1 = new std::pair{f1, i1};
        // #TEST#: R772 Rename use of p1
        REQUIRE_EQUAL(1.0f, p1->first);
        // #TEST#: R773 Rename use of p1
        REQUIRE_EQUAL(10, p1->second);
        // #TEST#: R774 Rename use of p1
        delete p1;
        // #TEST#: R775 Rename local variable name
        const char *name{"one"};
        // #TEST#: R776 Rename local variable p2
        // #TEST#: R777 Rename use of f1
        // #TEST#: R778 Rename use of name
        const auto *p2 = new ValueName{f1, std::string_view{name}};
        // #TEST#: R779 Rename use of p2
        REQUIRE_EQUAL(1.0f, p2->value);
        // #TEST#: R780 Rename use of name
        // #TEST#: R781 Rename use of p2
        REQUIRE_EQUAL(std::string_view{name}, p2->name);
        // #TEST#: R782 Rename use of p2
        delete p2;
    }
    // function-style cast expressions
    {
        int src[]{1, 2, 3};
        // #TEST#: R783 Rename local variable dest
        std::vector<int> dest;
        // #TEST#: R784 Rename use of dest
        std::copy(std::begin(src), std::end(src), std::back_insert_iterator(dest));
        REQUIRE_EQUAL(true, std::equal(std::begin(src), std::end(src), dest.begin()));
    }
}

// #TEST#: R785 Rename template parameter T
template <typename T>
// #TEST#: R786 Rename struct Container
struct Container
{
    // #TEST#: R787 Rename use of Container
    // #TEST#: R788 Rename use of T
    Container(T t) : m_how(1) {}

    // #TEST#: R789 Rename template parameter Iter
    template <typename Iter>
    // #TEST#: R790 Rename use of Container
    // #TEST#: R791 Rename use of Iter
    Container(Iter begin, Iter end) : m_how(2)
    {
    }

    int m_how{};
};

// #TEST#: R792 Rename template parameter Iter
template <typename Iter>
// #TEST#: R793 Rename first use of Container
// #TEST#: R794 Rename first use of Iter
// #TEST#: R795 Rename second use of Iter
// #TEST#: R796 Rename second use of Container
// #TEST#: R797 Rename third use of Iter
Container(Iter begin, Iter end)->Container<typename std::iterator_traits<Iter>::value_type>;

void f9()
{
    // #TEST#: R798 Rename use of Container
    // #TEST#: R799 Rename local variable c1
    const Container c1{7};
    // #TEST#: R800 Rename use of c1
    REQUIRE_EQUAL(1, c1.m_how);
    std::vector vals{1.0f, 2.0f, 3.0f};
    // #TEST#: R801 Rename use of Container
    // #TEST#: R802 Rename local variable c2
    // #TEST#: R803 Rename first use of vals
    // #TEST#: R804 Rename second use of vals
    const Container c2{vals.begin(), vals.end()};
    // #TEST#: R805 Rename use of c2
    REQUIRE_EQUAL(2, c2.m_how);
}

void f10()
{
    // #TEST#: R814 Rename A
    // #TEST#: R815 Rename B
    // #TEST#: R816 Rename C
    // #TEST#: R817 Rename x
    REQUIRE_EQUAL(10, RenameCpp17::A::B::C::x);

    {
        // #TEST#: R818 Rename A
        // #TEST#: R819 Rename B
        // #TEST#: R820 Rename C
        using namespace RenameCpp17::A::B::C;
        // #TEST#: R821 Rename x
        REQUIRE_EQUAL(10, x);
    }
}

void f11()
{
    {
        // #TEST#: R822 Rename A
        // #TEST#: R823 Rename B
        // #TEST#: R824 Rename C
        // #TEST#: R825 Rename y
        // #TEST#: R826 Rename z
        using RenameCpp17::A::B::C::y, RenameCpp17::A::B::C::z;
        // #TEST#: R827 Rename use of y
        // #TEST#: R828 Rename use of z
        REQUIRE_EQUAL(50, y + z);
    }
    {
        using namespace RenameCpp17;
        // #TEST#: R829 Rename A
        // #TEST#: R830 Rename B
        // #TEST#: R831 Rename C
        // #TEST#: R832 Rename y
        // #TEST#: R833 Rename z
        using A::B::C::y, A::B::C::z;
        // #TEST#: R834 Rename use of y
        // #TEST#: R835 Rename use of z
        REQUIRE_EQUAL(50, y + z);
    }
    {
        // #TEST#: R836 Rename A
        using namespace RenameCpp17::A;
        // #TEST#: R837 Rename B
        // #TEST#: R838 Rename C
        // #TEST#: R839 Rename y
        // #TEST#: R840 Rename z
        using B::C::y, B::C::z;
        // #TEST#: R841 Rename use of y
        // #TEST#: R842 Rename use of z
        REQUIRE_EQUAL(50, y + z);
    }
}

}    // namespace

void TestRenameCpp17()
{
    f1();
    f2();
    f3();
    f4();
    f5();
    f6();
    f7();
    f8();
    f9();
    f10();
    f11();
}