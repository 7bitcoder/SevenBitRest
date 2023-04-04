#include <boost/core/demangle.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <string>

#include "Common/CliVariables.hpp"
#include "Common/EnvVariables.hpp"
#include "Common/Utils.hpp"

namespace sd::utils
{
    EnvVariables getEnvVariables(std::string prefix)
    {
        std::unordered_map<std::string, std::string> variables;
        for (auto env : boost::this_process::environment())
        {
            std::string key = env.get_name();
            if (!key.empty() && key.starts_with(prefix))
            {
                key.erase(0, prefix.size());
                variables[key] = env.to_string();
            }
        }
        return EnvVariables{variables};
    }

    boost::program_options::parsed_options getParsedOptions(int argc, char **argv)
    {
        using namespace boost::program_options;
        options_description desc;

        return command_line_parser(argc, argv)
            .options(desc)
            .style(command_line_style::default_style)
            .allow_unregistered()
            .run();
    }

    CliVariables getCliVariables(int argc, char **argv)
    {
        std::unordered_map<std::string, std::string> variables;
        if (argc <= 1 || !argv)
        {
            return {variables};
        }
        for (auto &opt : getParsedOptions(argc, argv).options)
        {
            auto &key = opt.string_key;
            auto &values = opt.value;

            if (key.empty() || values.empty() || values.back().empty())
            {
                throw std::runtime_error("Invalid argument: '" + key + "' it must follow key=value format");
            }
            variables[key] = values.back();
        }
        return {variables};
    }

    std::string demangle(const std::string &name) { return boost::core::demangle(name.c_str()); }
} // namespace sd::utils