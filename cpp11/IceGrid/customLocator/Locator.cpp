// **********************************************************************
//
// Copyright (c) 2003-2017 ZeroC, Inc. All rights reserved.
//
// **********************************************************************

#include <Ice/Ice.h>

using namespace std;

namespace
{

//
// This LocatorRegistryI forwards everything to the provided (IceGrid) registry
//

class LocatorRegistryI : public Ice::LocatorRegistry
{
public:

    LocatorRegistryI(const shared_ptr<Ice::LocatorRegistryPrx>& registry) :
        _registry(registry)
    {
    }

    virtual void
    setAdapterDirectProxyAsync(string id,
                               shared_ptr<Ice::ObjectPrx> proxy,
                               function<void()> response,
                               function<void(exception_ptr)> exception,
                               const Ice::Current&) override
    {
        // AMI call using the AMD callbacks
        _registry->setAdapterDirectProxyAsync(id, proxy, response, exception);
    }

    virtual void
    setReplicatedAdapterDirectProxyAsync(string id,
                                         string group,
                                         shared_ptr<Ice::ObjectPrx> proxy,
                                         function<void()> response,
                                         function<void(exception_ptr)> exception,
                                         const Ice::Current&) override
    {
        _registry->setReplicatedAdapterDirectProxyAsync(id, group, proxy, response, exception);
    }

    virtual void
    setServerProcessProxyAsync(string id,
                               shared_ptr<Ice::ProcessPrx> proxy,
                               function<void()> response,
                               function<void(exception_ptr)> exception,
                               const Ice::Current&) override
    {
        _registry->setServerProcessProxyAsync(id, proxy, response, exception);
    }

private:

    const shared_ptr<Ice::LocatorRegistryPrx> _registry;
};

//
// Likewise, LocatorI forwards everything to the provided (IceGrid) locator
// when the context contains SECRET == LetMeIn
//
class LocatorI : public Ice::Locator
{
public:

    LocatorI(const shared_ptr<Ice::LocatorPrx>& locator,
             const shared_ptr<Ice::LocatorRegistryPrx>& registry) :
        _locator(locator),
        _registry(registry)
    {
    }

    virtual void
    findObjectByIdAsync(Ice::Identity id,
                        function<void(const shared_ptr<Ice::ObjectPrx>&)> response,
                        function<void(exception_ptr)> exception,
                        const Ice::Current& current) const override
    {
        auto p = current.ctx.find("SECRET");
        if(p != current.ctx.end() && p->second == "LetMeIn")
        {
            _locator->findObjectByIdAsync(id, response, exception);
        }
        else
        {
            response(nullptr);
        }
    }

    virtual void
    findAdapterByIdAsync(string id,
                         function<void(const shared_ptr<Ice::ObjectPrx>&)> response,
                         function<void(exception_ptr)> exception,
                         const Ice::Current& current) const override
    {
        auto p = current.ctx.find("SECRET");
        if(p != current.ctx.end() && p->second == "LetMeIn")
        {
            _locator->findAdapterByIdAsync(id, response, exception);
        }
        else
        {
            response(nullptr);
        }
    }

    virtual shared_ptr<Ice::LocatorRegistryPrx>
    getRegistry(const Ice::Current&) const override
    {
        return _registry;
    }

private:

    const shared_ptr<Ice::LocatorPrx> _locator;
    const shared_ptr<Ice::LocatorRegistryPrx> _registry;
};

}

class LocatorServer : public Ice::Application
{
public:

    virtual int run(int, char*[]) override;
};

int
main(int argc, char* argv[])
{
    LocatorServer app;
    return app.main(argc, argv, "config.locator");
}

int
LocatorServer::run(int argc, char*[])
{
    if(argc > 1)
    {
        cerr << appName() << ": too many arguments" << endl;
        return EXIT_FAILURE;
    }

    //
    // Get the IceGrid Locator and LocatorRegistry
    //
    auto defaultLocator = communicator()->getDefaultLocator();
    auto defaultRegistry = defaultLocator->getRegistry();

    //
    // Create the custom Locator and LocatorRegistry
    //
    auto adapter = communicator()->createObjectAdapter("Locator");
    auto registry = make_shared<LocatorRegistryI>(defaultRegistry);
    auto registryPrx =
        Ice::uncheckedCast<Ice::LocatorRegistryPrx>(adapter->add(registry, Ice::stringToIdentity("registry")));
    auto locator = make_shared<LocatorI>(defaultLocator, registryPrx);
    adapter->add(locator, Ice::stringToIdentity("locator"));
    adapter->activate();
    communicator()->waitForShutdown();
    return EXIT_SUCCESS;
}
