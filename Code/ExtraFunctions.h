#pragma once

#include <string>

namespace extras {

    static std::string removeWhitespace(std::string& str, bool leading = true, bool following = true) {
        std::string ret = str; // So as not to modify str
        if(leading) {
            while(ret[0] == ' ' || ret[0] == '\t') {
                ret.erase(ret.begin());
            }
        }
        if(following) {
            for(int i = ret.size()-1; i >= 0; i--) { // Trailing
                if(ret[i] == '\t' || ret[i] == ' ') { // This is whitespace
                    ret.erase(i);
                } else {
                    break;
                }
            }
        }
        return ret;
    }

}
