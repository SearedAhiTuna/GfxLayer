
#pragma once

#include <dict.h>
//#include <iostream>

template <typename K, typename V, typename ... Cargs>
class RefCntDict
{
private:
    struct Entry
    {
        Entry(const V& v_ = V(), const unsigned int& cnt_ = 0) :
            v(v_),
            cnt(cnt_)
        {
        }

        Entry(const Entry& e) :
            v(e.v),
            cnt(e.cnt)
        {
        }

        V v;
        unsigned int cnt;
    };

public:
    class Ref : public V
    {
    public:
        template <typename ... Args>
        Ref(Args... args) :
            V(args...)
        {
        }

        Ref(const Ref& r):
            V(r)
        {
            _dict = r._dict;
            k = r.k;

            if (_dict)
                _dict->inc(k);
        }

        Ref& operator=(const V& v) = delete;

        Ref& operator=(const Ref& r)
        {
            V::operator=(r);

            if (_dict != r._dict)
            {
                if (_dict)
                    _dict->dec(k);

                _dict = r._dict;
                k = r.k;

                if (_dict)
                    _dict->inc(k);
            }

            return *this;
        }

        ~Ref()
        {
            if (_dict)
                _dict->dec(k);
        }

    private:
        friend class RefCntDict;

    private:
        RefCntDict* _dict{};
        K k;
    };

public:
    Ref get(const K& k, Cargs ... cargs)
    {
        // Create an instance at this key if there is not one
        if (base.count(k) == 0)
        {
            base.emplace(k, create(k, cargs...), 0);
        }

        // Update the reference count
        ++base[k].cnt;

        // Create a reference
        Ref r(base[k].v);
        r._dict = this;
        r.k = k;

        // Return a reference
        return r;
    }

protected:
    virtual V create(const K& k, Cargs ... cargs) = 0;
    virtual void destroy(const K& k, V& v) {};

private:
    void inc(const K& k)
    {
        ++base[k].cnt;
        //std::cout << "Incremented ref " << k << ": " << base[k].cnt << "\n";
    }

    void dec(const K& k)
    {
        if (--base[k].cnt == 0)
        {
            destroy(k, base[k].v);
            base.erase(k);
            //std::cout << "Destroyed " << k << "\n";
        }
        else
        {
            //std::cout << "Decremented ref " << k << ": " << base[k].cnt << "\n";
        }
        
    }

private:
    dict<K, Entry> base;
};