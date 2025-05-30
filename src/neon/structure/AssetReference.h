//
// Created by gaeqs on 29/05/25.
//

#ifndef ASSETREFERENCE_H
#define ASSETREFERENCE_H

#include <memory>
#include <neon/structure/Versioned.h>

namespace neon
{

    /**
     * Reference to an asset that may change over time.
     */
    template<typename T>
    class AssetReference : public Versioned
    {
        std::shared_ptr<T> _asset;

      public:
        AssetReference(std::shared_ptr<T> asset) :
            _asset(asset)
        {
        }

        AssetReference() = default;

        std::shared_ptr<T> get() const
        {
            return _asset;
        }

        T* operator->()
        {
            return _asset.operator->();
        }

        void set(std::shared_ptr<T> asset)
        {
            _asset = std::move(asset);
            incrementVersion();
        }

        AssetReference& operator=(std::shared_ptr<T> asset)
        {
            _asset = std::move(asset);
            return *this;
        }

        AssetReference& operator=(std::shared_ptr<T>&& asset)
        {
            _asset = std::move(asset);
            return *this;
        }
    };

} // namespace neon

#endif // ASSETREFERENCE_H
