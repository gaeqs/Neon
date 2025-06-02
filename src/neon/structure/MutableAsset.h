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
    class MutableAsset : public Versioned
    {
        std::shared_ptr<T> _asset;

      public:
        MutableAsset(std::shared_ptr<T> asset) :
            _asset(asset)
        {
        }

        MutableAsset() = default;

        const std::shared_ptr<T>& get() const
        {
            return _asset;
        }

        T* operator->()
        {
            return _asset.operator->();
        }

        T& operator*() const
        {
            return *_asset;
        }

        template<typename U>
        U* as() const
        {
            return dynamic_cast<U*>(_asset.get());
        }

        template<typename U>
            requires std::derived_from<U, T>
        void set(std::shared_ptr<U> newAsset)
        {
            _asset = std::move(newAsset);
            incrementVersion();
        }

        template<typename U, typename... Args>
            requires std::derived_from<U, T>
        void emplace(Args&&... args)
        {
            _asset = std::make_shared<U>(std::forward<Args>(args)...);
            incrementVersion();
        }

        template<typename U>
            requires std::derived_from<U, T>
        MutableAsset& operator=(std::shared_ptr<U> asset)
        {
            _asset = std::move(asset);
            incrementVersion();
            return *this;
        }

        template<typename U>
            requires std::derived_from<U, T>
        MutableAsset& operator=(std::shared_ptr<U>&& asset)
        {
            _asset = std::move(asset);
            incrementVersion();
            return *this;
        }

        void reset()
        {
            _asset.reset();
            incrementVersion();
        }

        bool operator==(const MutableAsset& other) const
        {
            return _asset == other._asset;
        }

        bool operator!=(const MutableAsset& other) const
        {
            return !(*this == other);
        }

        bool operator==(const std::shared_ptr<T>& other) const
        {
            return _asset == other;
        }

        bool operator!=(const std::shared_ptr<T>& other) const
        {
            return !(*this == other);
        }
    };

} // namespace neon

#endif // ASSETREFERENCE_H
