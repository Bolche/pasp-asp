#ifndef ASSERT_LIST_EQUAL_H
#define	ASSERT_LIST_EQUAL_H

#include <sstream>
#include <string>
#include <list>
#include <map>
#include <cppunit/SourceLine.h>

#define CPPUNIT_ASSERT_LIST_EQUALS(expected, actual)  ( CPPUNIT_NS::assertEquals( (printList(expected)), \
                                                                                  (printList(actual)), \
                                                                                  CPPUNIT_SOURCELINE(), "") )

template <class T>
inline std::string printList(const T& list) {
    std::stringstream s;
    s << "{ ";

    for (auto i: list)
        s << printList(i) << ", ";

    s << "}";
    return s.str();
}

template <>
inline std::string printList(const int& list) {
    std::stringstream s;
    s << list;
    return s.str();
}

template <>
inline std::string printList(const double& list) {
    std::stringstream s;
    s << list;
    return s.str();
}

template <>
inline std::string printList(const std::string& list) {
    std::stringstream s;
    s << list;
    return s.str();
}

inline std::list<std::string> explode(std::string str, const char sep) {
    int found = str.find_first_of(sep);
    std::list<std::string> result;

    while ( found != (int)std::string::npos) {
        if (found > 0)
            result.push_back(str.substr(0, found));

        str = str.substr(found+1);
        found = str.find_first_of(sep);
    }

    if (str.length() > 0)
        result.push_back(str.substr(0, found));

    return result;
}

#endif /* ASSERT_LIST_EQUAL_H */
