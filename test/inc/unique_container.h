
#pragma once

template <typename container, typename T>
class unique_container : public container
{
public:
    void emplace_back(const T& t)
    {
        for (const T& old_t : *this)
        {
            if (old_t == t)
                return;
        }
        container::emplace_back(t);
    }
};