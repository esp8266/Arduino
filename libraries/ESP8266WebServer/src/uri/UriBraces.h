#ifndef URI_BRACES_H
#define URI_BRACES_H

#include "Uri.h"

class UriBraces : public Uri {

    public:
        explicit UriBraces(const char *uri) : Uri(uri) {};
        explicit UriBraces(const String &uri) : Uri(uri) {};

        Uri* clone() const override final {
            return new UriBraces(_uri);
        };

        bool canHandle(const String &requestUri, std::vector<String> &currentPathArgs) override final {
            if (Uri::canHandle(requestUri, currentPathArgs))
                return true;

            currentPathArgs.clear();

            size_t uriLength = _uri.length();
            unsigned int requestUriIndex = 0;
            for (unsigned int i = 0; i < uriLength; i++, requestUriIndex++) {
                char uriChar = _uri[i];
                char requestUriChar = requestUri[requestUriIndex];

                if (uriChar == requestUriChar)
                    continue;
                if (uriChar != '{')
                    return false;

                i += 2; // index of char after '}'
                if (i >= uriLength) {
                    // there is no char after '}'
                    currentPathArgs.push_back(requestUri.substring(requestUriIndex));
                    return currentPathArgs.back().indexOf("/") == -1; // path argument may not contain a '/'
                }
                else
                {
                    char charEnd = _uri[i];
                    int uriIndex = requestUri.indexOf(charEnd, requestUriIndex);
                    if (uriIndex < 0)
                        return false;
                    currentPathArgs.push_back(requestUri.substring(requestUriIndex, uriIndex));
                    requestUriIndex = (unsigned int) uriIndex;
                }
            }

            return requestUriIndex >= requestUri.length();
        }
};

#endif
