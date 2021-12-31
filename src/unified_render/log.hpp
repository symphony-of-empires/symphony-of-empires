#pragma once

#include <string>

namespace UnifiedRender {
    namespace Log {
        void debug(const std::string& category, const std::string& msg);
        void warning(const std::string& category, const std::string& msg);
        void error(const std::string& category, const std::string& msg);
    };
};
