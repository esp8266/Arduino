#ifndef URI_REGEX_H
#define URI_REGEX_H

#include "Uri.h"
#include <regex.h>
#include <assert.h>

#ifndef REGEX_MAX_GROUPS
#define REGEX_MAX_GROUPS 10
#endif

class UriRegex : public Uri {

    private:
        regex_t _regexCompiled;

    public:
        explicit UriRegex(const char *uri) : Uri(uri) {
            assert(regcomp(&_regexCompiled, uri, REG_EXTENDED) == 0);
        };
        explicit UriRegex(const String &uri) : UriRegex(uri.c_str()) {};

        ~UriRegex() {
            regfree(&_regexCompiled);
        }

        Uri* clone() const override final {
            return new UriRegex(_uri);
        };

        bool canHandle(const String &requestUri, std::vector<String> &currentPathArgs) override final {
            if (Uri::canHandle(requestUri, currentPathArgs))
                return true;

            regmatch_t groupArray[REGEX_MAX_GROUPS];
            if (regexec(&_regexCompiled, requestUri.c_str(), REGEX_MAX_GROUPS, groupArray, 0) == 0) {
                // matches
                currentPathArgs.clear();

                unsigned int g = 1; 
                for (; g < REGEX_MAX_GROUPS; g++) {
                    if (groupArray[g].rm_so == (long int)-1)
                        break;  // No more groups

                    currentPathArgs.push_back(requestUri.substring(groupArray[g].rm_so, groupArray[g].rm_eo));
                }

                return true;
            }
            return false;
        }
};

#endif
