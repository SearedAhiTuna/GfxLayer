
#pragma once

#include <list>

namespace std
{
    template <typename K, typename V>
    class dict
    {
    private:
        struct Entry
        {
            K k;
            V v;
        };

    public:
        template <typename ... Args>
        void emplace(const K& k, Args ... args)
        {
            base.push_back({ k, V(args...) });
        }

        V& operator[](const K& k)
        {
            for (Entry& ent : base)
            {
                if (ent.k == k)
                    return ent.v;
            }

            emplace(k);

            return base.back().v;
        }

        const V& operator[](const K& k) const
        {
            for (const Entry& ent : base)
            {
                if (ent.k == k)
                    return ent.v;
            }

            return base.back().v;
        }

        size_t count(const K& k)
        {
            for (const Entry& ent : base)
            {
                if (ent.k == k)
                    return 1;
            }

            return 0;
        }

        void erase(const K& k)
        {
            for (auto it = base.begin(); it != base.end(); ++it)
            {
                if (it->k == k)
                {
                    base.erase(it);
                    return;
                }
            }
        }

    private:
        std::list<Entry> base;
    };
}
