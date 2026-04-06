#pragma once


namespace alang
{
    template <size_t N>
    struct ConstexprString
    {
        char data[N + 1]{};
        size_t size = 0;

        constexpr void append(const char* str)
        {
            for (size_t i = 0; str[i] != '\0'; ++i)
            {
                if (size < N)
                    data[size++] = str[i];
            }
            data[size] = '\0';
        }

        constexpr void append(char c)
        {
            if (size < N)
                data[size++] = c;
            data[size] = '\0';
        }

        constexpr void append(std::string_view str)
        {
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (size < N)
                    data[size++] = str[i];
            }
            data[size] = '\0';
        }

        constexpr std::string_view view() const
        {
            return { data, size };
        }
    };

}