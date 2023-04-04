#pragma once

#include <cstddef>
#include <exception>
#include <forward_list>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Common/Utils.hpp"
#include "DI/CircularDependencyGuard.hpp"
#include "DI/CtorReflection.hpp"
#include "DI/Exceptions.hpp"
#include "DI/IServiceHolder.hpp"
#include "DI/ServiceOwner.hpp"
#include "DI/ServiceScope.hpp"
#include "DI/ServicesContainer.hpp"
#include "Engine/IContext.hpp"
#include "Services/ContextAccessor.hpp"
#include "Services/IContextAccessor.hpp"

namespace sd
{

    class ServiceProvider;

    template <class T> T getService(ServiceProvider &provider);

    class ServiceCollection
    {
      private:
        friend class ServiceProvider;
        // Internal helper classes

        struct IServiceCreator
        {
            using Ptr = std::unique_ptr<IServiceCreator>;

            virtual const ServiceScope &getServiceScope() const = 0;

            virtual TypeId getServiceTypeId() const = 0;

            virtual TypeId getServiceInterfaceTypeId() const = 0;

            virtual IServiceHolder::Ptr create(ServiceProvider &serviceProvider) const = 0;

            virtual ~IServiceCreator() {}
        };

        template <class I, class T, class FactoryFcn> class ServiceFactory final : public IServiceCreator
        {
          private:
            FactoryFcn _factoryFunction;
            ServiceScope _scope;

          public:
            ServiceFactory(const ServiceScope &scope, FactoryFcn factoryFunction)
                : _factoryFunction{std::move(factoryFunction)}, _scope{scope}
            {
                static_assert(std::is_base_of_v<I, T>, "Type T must inherit from I");
                // static_assert(std::is_same_v<std::unique_ptr<T>, decltype(std::declval<FactoryFcn>())>,
                //               "Factory function must return unique pointner");
            }

            const ServiceScope &getServiceScope() const { return _scope; }

            TypeId getServiceTypeId() const { return typeid(T); }
            TypeId getServiceInterfaceTypeId() const { return typeid(I); }

            IServiceHolder::Ptr create(ServiceProvider &serviceProvider) const
            {
                return std::make_unique<ServiceOwner<I, T>>(_factoryFunction(serviceProvider));
            }
        };

        template <class I, class T> class ServiceConstructor final : public IServiceCreator
        {
          private:
            using ConstructorTraits = ConstrutorTraits<T>;
            using Indices = std::make_index_sequence<ConstructorTraits::ArgsSize>;
            ServiceScope _scope;

          public:
            using Ptr = std::unique_ptr<ServiceConstructor>;
            ServiceConstructor(const ServiceScope &scope) : _scope{scope}
            {
                static_assert(std::is_base_of_v<I, T>, "Type T must inherit from I");
            }

            const ServiceScope &getServiceScope() const { return _scope; };

            TypeId getServiceTypeId() const { return typeid(T); }
            TypeId getServiceInterfaceTypeId() const { return typeid(I); }

            IServiceHolder::Ptr create(ServiceProvider &serviceProvider) const
            {
                return create(serviceProvider, Indices{});
            };

          private:
            template <size_t... Index>
            typename ServiceOwner<I, T>::Ptr create(ServiceProvider &serviceProvider,
                                                    std::index_sequence<Index...>) const
            {
                auto servicePtr = std::make_unique<T>(
                    getService<typename ConstructorTraits::template Arg<Index>::Type>(serviceProvider)...);
                return std::make_unique<ServiceOwner<I, T>>(std::move(servicePtr));
            }
        };

        class ServiceCreators
        {
          private:
            std::vector<IServiceCreator::Ptr> _serviceCreators;

          public:
            ServiceCreators() { _serviceCreators.reserve(1); }

            ServiceCreators(ServiceCreators &&) = default;
            ServiceCreators(const ServiceCreators &) = delete;
            ServiceCreators &operator=(const ServiceCreators &) = delete;
            ServiceCreators &operator=(ServiceCreators &&) = default;

            ServiceScope getServicesScope() const { return getMainCreator().getServiceScope(); }

            TypeId getInterfaceTypeId() const { return getMainCreator().getServiceInterfaceTypeId(); }

            void add(IServiceCreator::Ptr creator)
            {
                if (!creator)
                {
                    throw ServiceCreatorInvalidException{};
                }
                _serviceCreators.emplace_back(std::move(creator));
            }

            template <class T> size_t remove()
            {
                TypeId type = typeid(T);
                return std::erase_if(_serviceCreators,
                                     [=](auto &creator) { return creator.getServiceTypeId() == type; });
            }

            template <class T> bool contains()
            {
                TypeId type = typeid(T);
                for (auto &creator : _serviceCreators)
                {
                    if (creator->getServiceTypeId() == type)
                    {
                        return true;
                    }
                }
                return false;
            }

            void seal() { _serviceCreators.shrink_to_fit(); }

            auto begin() const { return _serviceCreators.crbegin(); }
            auto end() const { return _serviceCreators.crend(); }

            size_t size() const { return _serviceCreators.size(); }

            IServiceCreator &getMainCreator() const { return *_serviceCreators.back(); }
        };

        std::unordered_map<TypeId, ServiceCreators> _serviceCreatorsMap;

      public:
        using Ptr = std::unique_ptr<ServiceCollection>;

        ServiceCollection() = default;
        ServiceCollection(const ServiceCollection &) = delete;
        ServiceCollection(ServiceCollection &&) = default;

        ServiceCollection &operator=(const ServiceCollection &) = delete;
        ServiceCollection &operator=(ServiceCollection &&) = default;

        template <class I, class T = I> void addSingleton() { add<I, T>(ServiceScope::singeleton()); }
        template <class I, class T = I> void addScoped() { add<I, T>(ServiceScope::scoped()); }
        template <class I, class T = I> void addTransient() { add<I, T>(ServiceScope::transient()); }
        template <class I, class T = I> void add(ServiceScope scope)
        {
            add(std::make_unique<ServiceConstructor<I, T>>(scope));
        }

        template <class I, class T = I, class FactoryFcn> void addSingleton(FactoryFcn factory)
        {
            add<I, T>(ServiceScope::singeleton(), std::move(factory));
        }
        template <class I, class T = I, class FactoryFcn> void addScoped(FactoryFcn factory)
        {
            add<I, T>(ServiceScope::scoped(), std::move(factory));
        }
        template <class I, class T = I, class FactoryFcn> void addTransient(FactoryFcn factory)
        {
            add<I, T>(ServiceScope::transient(), std::move(factory));
        }
        template <class I, class T = I, class FactoryFcn> void add(ServiceScope scope, FactoryFcn factory)
        {
            add(std::make_unique<ServiceFactory<I, T, FactoryFcn>>(scope, std::move(factory)));
        }

        void addContextAccessor() { addScoped<IContextAccessor, ContextAccessor>(); }

        template <class I> bool contains() const { return _serviceCreatorsMap.contains(typeid(I)); }

        template <class I, class T> bool contains() const
        {
            if (auto it = _serviceCreatorsMap.find(typeid(I)); it != _serviceCreatorsMap.end())
            {
                return it->second.contains<I, T>();
            }
            return false;
        }

        template <class I> bool removeAll() { return _serviceCreatorsMap.erase(typeid(I)) > 0; }

        template <class I, class T> bool remove()
        {
            auto it = _serviceCreatorsMap.find(typeid(I));
            if (it != _serviceCreatorsMap.end())
            {
                auto &creators = it->second;
                auto removed = creators.remove<T>() > 0;
                if (creators.size() == 0)
                {
                    _serviceCreatorsMap.erase(it);
                }
                return removed;
            }

            return false;
        }

        void merge(ServiceCollection &&serviceCollection)
        {
            _serviceCreatorsMap.merge(std::move(serviceCollection._serviceCreatorsMap));
        }

      private:
        void add(IServiceCreator::Ptr creator)
        {
            if (!creator)
            {
                throw ServiceCreatorInvalidException{};
            }
            auto serviceTypeId = creator->getServiceTypeId();
            auto serviceInterfaceTypeId = creator->getServiceInterfaceTypeId();
            auto serviceScope = creator->getServiceScope();

            if (isAlreadyResistered(*creator))
            {
                throw new ServiceTypeAlreadyRegisteredException{serviceTypeId};
            }

            auto &creators = _serviceCreatorsMap[serviceInterfaceTypeId];
            if (creators.size() && creators.getServicesScope() != serviceScope)
            {
                throw ServiceScopeMissmatchException{serviceTypeId, serviceInterfaceTypeId};
            }
            creators.add(std::move(creator));
        }

        const ServiceCreators *getCreators(TypeId typeId) const
        {
            auto pair = _serviceCreatorsMap.find(typeId);
            return pair != _serviceCreatorsMap.end() ? &pair->second : nullptr;
        }

        const IServiceCreator *getMainCreator(TypeId typeId) const
        {
            if (auto creators = getCreators(typeId))
            {
                return &creators->getMainCreator();
            }
            return nullptr;
        }

        bool isAlreadyResistered(IServiceCreator &newCreator) const
        {
            auto it = _serviceCreatorsMap.find(newCreator.getServiceInterfaceTypeId());

            if (it == _serviceCreatorsMap.end())
            {
                return false;
            }

            auto &[_, creators] = *it;
            for (auto &creator : creators)
            {
                if (creator && newCreator.getServiceTypeId() == creator->getServiceTypeId())
                {
                    return true;
                }
            }
            return false;
        }

        auto begin() const { return _serviceCreatorsMap.cbegin(); }

        auto end() const { return _serviceCreatorsMap.cend(); }
    };

    class ServiceProvider
    {
      public:
        using ExternalServices = std::vector<IServiceHolder::Ptr>;
        using Ptr = std::unique_ptr<ServiceProvider>;

      private:
        const ServiceCollection &_collection;

        ServiceContainer &_singletons;
        ServiceContainer _scoped;

        CircularDependencyGuard _guard;

      public:
        ServiceProvider(const ServiceProvider &) = delete;
        ServiceProvider(ServiceProvider &&) = default;

        ServiceProvider &operator=(const ServiceProvider &) = delete;
        ServiceProvider &operator=(ServiceProvider &&) = delete;

        ServiceProvider(const ServiceCollection &collection, ServiceContainer &singletons)
            : _collection{collection}, _singletons{singletons}
        {
        }

        template <class I> I &getRequiredService()
        {
            if (auto ptr = getService<I>())
            {
                return *ptr;
            }
            throw ServiceNotRegisteredException{typeid(I)};
        }

        template <class I> I *getService() { return (I *)getService(typeid(I)); }

        void *getService(TypeId typeId)
        {
            if (auto list = _singletons.getList(typeId))
            {
                return list->at(0);
            }
            if (auto list = _scoped.getList(typeId))
            {
                return list->at(0);
            }
            if (typeId == typeid(ServiceProvider))
            {
                return this;
            }
            return createAndRegister(typeId);
        }

        template <class I> std::vector<I *> getServices()
        {
            auto services = getServices(typeid(I));
            std::vector<I *> result;
            result.reserve(services.size());
            for (auto ptr : services)
            {
                result.push_back((I *)ptr);
            }
            return result;
        }

        std::vector<void *> getServices(TypeId typeId)
        {
            if (auto list = _singletons.getList(typeId); list && list->isSealed())
            {
                return list->getAll();
            }
            if (auto list = _scoped.getList(typeId); list && list->isSealed())
            {
                return list->getAll();
            }
            if (typeId == typeid(ServiceProvider))
            {
                return {this};
            }
            return createAndRegisterAll(typeId);
        }

        template <class I> std::unique_ptr<I> createService() { return createUnique<I>(); }

        template <class I> std::vector<std::unique_ptr<I>> createServices() { return createAllUnique<I>(); }

        void prebuildSingeletons()
        {
            for (auto &objectToBuild : _collection)
            {
                auto &[_, creators] = objectToBuild;
                if (!creators.getServicesScope().isSingeleton())
                {
                    continue;
                }
                getServices(creators.getInterfaceTypeId());
            }
        }

        ServiceProvider::Ptr createScoped() { return std::make_unique<ServiceProvider>(_collection, _singletons); }

      private:
        void *createAndRegister(TypeId typeId)
        {
            if (auto creator = _collection.getMainCreator(typeId))
            {
                return createAndRegister(*creator);
            }
            return nullptr;
        }

        std::vector<void *> createAndRegisterAll(TypeId typeId)
        {
            if (auto creators = _collection.getCreators(typeId))
            {
                return createAndRegisterAll(*creators);
            }
            return {};
        }

        template <class I> std::unique_ptr<I> createUnique()
        {
            if (auto creator = _collection.getMainCreator(typeid(I)))
            {
                return createUnique<I>(*creator);
            }
            return nullptr;
        }

        template <class I> std::vector<std::unique_ptr<I>> createAllUnique()
        {
            if (auto creators = _collection.getCreators(typeid(I)))
            {
                return createAllUnique<I>(*creators);
            }
            return {};
        }

        void *createAndRegister(const ServiceCollection::IServiceCreator &creator)
        {
            auto &scope = creator.getServiceScope();
            if (scope.isTransient())
            {
                throw TransientForbidException{creator.getServiceTypeId()};
            }
            auto holder = createHolder(creator);
            auto &container = scope.isSingeleton() ? _singletons : _scoped;
            return container.addAndGetList(std::move(holder))->at(0);
        }

        std::vector<void *> createAndRegisterAll(const sd::ServiceCollection::ServiceCreators &creators)
        {
            if (creators.getServicesScope().isTransient())
            {
                throw TransientForbidException{creators.getInterfaceTypeId()};
            }
            auto &container = creators.getServicesScope().isSingeleton() ? _singletons : _scoped;
            ServiceList *serviceList = container.getList(creators.getInterfaceTypeId());

            if (!serviceList)
            {
                serviceList = container.addAndGetList(createHolder(creators.getMainCreator()));
            }
            serviceList->reserve(creators.size());
            for (auto it = ++creators.begin(); it != creators.end(); ++it) // skip main service
            {
                serviceList->add(createHolder(**it));
            }
            serviceList->seal();
            return serviceList->getAll();
        }

        template <class I> std::unique_ptr<I> createUnique(const ServiceCollection::IServiceCreator &creator)
        {
            auto &scope = creator.getServiceScope();
            if (!scope.isTransient())
            {
                throw NotTransientException{creator.getServiceTypeId()};
            }
            auto holder = createHolder(creator);
            auto &casted = static_cast<ServiceOwner<I, I> &>(*holder);
            return casted.moveOutService();
        }

        template <class I>
        std::vector<std::unique_ptr<I>> createAllUnique(const sd::ServiceCollection::ServiceCreators &creators)
        {
            std::vector<std::unique_ptr<I>> result;
            if (!creators.getServicesScope().isTransient())
            {
                throw NotTransientException{typeid(I)};
            }
            result.reserve(creators.size());
            for (auto &creator : creators)
            {
                result.push_back(createUnique<I>(*creator));
            }
            return result;
        }

        IServiceHolder::Ptr createHolder(const ServiceCollection::IServiceCreator &creator)
        {
            auto scopedGuard = _guard.spawnGuard(creator.getServiceTypeId());

            auto holder = creator.create(*this);

            if (holder && holder->isValid())
            {
                return holder;
            }
            throw ConstructionException{creator.getServiceTypeId()};
        }
    };

    namespace serviceExtractors
    {
        using SP = ServiceProvider;
        template <class T> struct R
        {
        };

        template <class T> auto get(R<T *>, SP &sp) { return &sp.getRequiredService<T>(); }
        template <class T> auto get(R<T *const>, SP &sp) { return &sp.getRequiredService<T>(); }
        template <class T> auto get(R<const T *>, SP &sp) { return &sp.getRequiredService<T>(); }
        template <class T> auto get(R<const T *const>, SP &sp) { return &sp.getRequiredService<T>(); }

        template <class T> auto &get(R<T &>, SP &sp) { return sp.getRequiredService<T>(); }
        template <class T> auto &get(R<const T &>, SP &sp) { return sp.getRequiredService<T>(); }

        template <class T> auto get(R<std::unique_ptr<T>>, SP &sp) { return sp.createService<T>(); }
        template <class T> auto get(R<const std::unique_ptr<T>>, SP &sp) { return sp.createService<T>(); }

        template <class T> auto get(R<std::vector<T *>>, SP &sp) { return sp.getServices<T>(); }
        template <class T> auto get(R<const std::vector<T *>>, SP &sp) { return sp.getServices<T>(); }

        template <class T> auto get(R<std::vector<std::unique_ptr<T>>>, SP &sp) { return sp.createServices<T>(); }
        template <class T> auto get(R<const std::vector<std::unique_ptr<T>>>, SP &sp) { return sp.createServices<T>(); }

        template <class T> auto get(R<std::vector<T>>, SP &)
        {
            static_assert(utils::notSupportedType<T>,
                          "Vector should contain pointners or unique pointners for transient services");
        }

        template <class T> T get(R<T>, SP &)
        {
            static_assert(utils::notSupportedType<T>, "Type is not supported as function augument parameter");
        }
    } // namespace serviceExtractors

    template <class T> T getService(ServiceProvider &provider)
    {
        return serviceExtractors::get(serviceExtractors::R<T>{}, provider);
    }
} // namespace sd
