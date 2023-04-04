#pragma once

#include <filesystem>
#include <memory>
#include <tao/json/type.hpp>

#include "Common/Utils.hpp"
#include "Configuration/IConfigurationSource.hpp"

namespace sd
{
    class FileConfigurationSource final : public IConfigurationSource
    {
      private:
        class Provider final : public IConfigurationProvider
        {
          private:
            FileConfigurationSource &_source;
            Json::object_t _configuration;

          public:
            Provider(FileConfigurationSource &source) : _source(source) {}

            void load() { _configuration = getJsonFromFile(_source.getFilePath(), _source.isOptional()); }

            Json::object_t get() { return _configuration; }

          private:
            Json::object_t getJsonFromFile(std::string filePath, bool optional)
            {
                bool exists = std::filesystem::exists(filePath);
                if (!exists)
                {
                    return optional ? Json::object_t{}
                                    : throw std::runtime_error("file '" + filePath + "' does not exist");
                }
                auto json = tao::json::basic_from_file<JsonTraits>(filePath);
                if (!json.is_object())
                {
                    throw std::runtime_error("file '" + filePath + "' does not contain json object'");
                }
                return json.get_object();
            }
        };

      private:
        std::filesystem::path _filePath;
        bool _isOptional;

      public:
        FileConfigurationSource(std::filesystem::path filePath, bool isOptional)
            : _filePath(filePath), _isOptional(isOptional)
        {
        }

        IConfigurationProvider::Ptr build(IConfigurationBuilder &builder) { return std::make_unique<Provider>(*this); }

        bool isOptional() const { return _isOptional; }

        std::filesystem::path getFilePath() const { return _filePath; }
    };
} // namespace sd