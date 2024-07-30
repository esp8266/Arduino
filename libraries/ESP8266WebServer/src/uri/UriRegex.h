#ifndef URI_REGEX_H
#define URI_REGEX_H

#include "Uri.h"

#include <cassert>
#include <regex.h>

#ifndef REGEX_MAX_GROUPS
#define REGEX_MAX_GROUPS 10
#endif

class UriRegex : public Uri {

    private:
        regex_t _regexCompiled{};
        int _regexErr{REG_EMPTY};

    public:
        UriRegex() = delete;

        explicit UriRegex(const char *uri) :
            Uri(uri),
            _regexErr(regcomp(&_regexCompiled, uri, REG_EXTENDED))
        {
            assert(_regexErr == 0);
        }

        explicit UriRegex(const String &uri) : UriRegex(uri.c_str()) {}

        ~UriRegex() {
            regfree(&_regexCompiled);
        }

        Uri* clone() const override final {
            return new UriRegex(_uri);
        }

        bool canHandle(const String &requestUri, std::vector<String> &pathArgs) override final {
            if (Uri::canHandle(requestUri, pathArgs))
                return true;

            if (_regexErr != 0)
                return false;

            regmatch_t groupArray[REGEX_MAX_GROUPS];
            if (regexec(&_regexCompiled, requestUri.c_str(), REGEX_MAX_GROUPS, groupArray, 0) == 0) {
                pathArgs.clear();

                unsigned int g = 1; 
                for (; g < REGEX_MAX_GROUPS; g++) {
                    if (groupArray[g].rm_so == (long int)-1)
                        break;  // No more groups

                    pathArgs.push_back(requestUri.substring(groupArray[g].rm_so, groupArray[g].rm_eo));
                }

                return true;
            }
            return false;
        }
};

#endif
