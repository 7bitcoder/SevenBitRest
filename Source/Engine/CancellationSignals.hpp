#pragma once

#include <boost/asio/cancellation_signal.hpp>
#include <list>
#include <mutex>

namespace sd
{
    // A simple helper for cancellation_slot
    struct CancellationSignals
    {
        std::list<boost::asio::cancellation_signal> sigs;
        std::mutex mtx;

        void emit(boost::asio::cancellation_type ct = boost::asio::cancellation_type::all)
        {
            std::lock_guard<std::mutex> _(mtx);

            for (auto &sig : sigs)
                sig.emit(ct);
        }

        boost::asio::cancellation_slot slot()
        {
            std::lock_guard<std::mutex> _(mtx);

            auto itr = std::find_if(sigs.begin(), sigs.end(),
                                    [](boost::asio::cancellation_signal &sig) { return !sig.slot().has_handler(); });

            if (itr != sigs.end())
                return itr->slot();
            else
                return sigs.emplace_back().slot();
        }
    };

} // namespace sd