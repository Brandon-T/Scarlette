#ifndef EVENTMANAGER_HPP_INCLUDED
#define EVENTMANAGER_HPP_INCLUDED

#include <functional>
#include <vector>
#include <cstdint>
#include <tuple>
#include <iostream>
#include <algorithm>

template<typename T>
class EventManager;

template<typename R, typename... Args>
class EventManager<R(Args...)>
{
    private:
        std::vector<std::tuple<std::uint64_t, std::uint64_t, std::function<R(Args...)>>> Events;

    public:
        std::uint64_t Subscribe(std::uint64_t Notification, std::function<R(Args...)>&& Func)
        {
            static std::uint64_t ID = 0;
            Events.emplace_back(std::make_tuple(!Notification ? 0 : ++ID, Notification, std::forward<std::function<R(Args...)>>(Func)));
            return ID;
        }

        void UnSubscribe(std::uint64_t EventID)
        {
            auto it = std::find_if(Events.begin(), Events.end(), [&](typename decltype(Events)::value_type &it) {
                return std::get<0>(it) == EventID;
            });

            if (it != Events.end())
            {
                Events.erase(it);
            }
        }

        bool IsSubscribed(std::uint64_t Notification)
        {
            auto it = std::find_if(Events.begin(), Events.end(), [&](typename decltype(Events)::value_type &it) {
                return std::get<1>(it) == Notification;
            });
            return it != Events.end();
        }

        R Notify(std::uint64_t Notification, Args... args)
        {
            auto it = std::find_if(Events.begin(), Events.end(), [&](typename decltype(Events)::value_type &it) {
                return std::get<1>(it) == Notification;
            });

            if (it != Events.end())
            {
                return std::get<2>(*it)(args...);
            }
            return R();
        }

        R NotifyAll(std::uint64_t Notification, Args... args)
        {
            if (IsSubscribed(Notification))
            {
                return Notify(Notification, args...);
            }

            auto it = std::find_if(Events.begin(), Events.end(), [&](typename decltype(Events)::value_type &it) {
                return !std::get<1>(it);
            });

            if (it != Events.end())
            {
                return std::get<2>(*it)(args...);
            }
            return R();
        }
};

#endif // EVENTMANAGER_HPP_INCLUDED
