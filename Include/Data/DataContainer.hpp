#pragma once
#include <string>
#include <unordered_map>

#include "Data/IData.hpp"
#include "Data/IDataContainer.hpp"

namespace sd
{

    class DataContainer : public IDataContainer
    {
      private:
        std::unordered_map<std::string, IData::Ptr> _data;

      public:
        using Ptr = std::unique_ptr<DataContainer>;

        IData *get(std::string_view key) const
        {
            if (auto data = _data.find(std::string{key}); data != _data.end())
            {
                return data->second.get();
            }
            return nullptr;
        }

        void add(std::string_view key, IData::Ptr data) { _data[std::string{key}] = std::move(data); }

        bool remove(std::string_view key) { return _data.erase(std::string{key}) > 0; }

        bool exists(std::string_view key) const { return _data.contains(std::string{key}); }
    };
} // namespace sd