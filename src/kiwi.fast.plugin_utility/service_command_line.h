#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

#include <boost/program_options/variables_map.hpp>

#include <string>
#include <optional>
#include <deque>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE
class service_command_line;
KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_command_line, u8"service_command_line")

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

/*!
 * 只提供选项值是简单类型或容器类型的情况，再复杂的就使用配置文件方式提供
 */
class service_command_line
{
protected:
    virtual ~service_command_line(){}

public:
    virtual void parse(int argc, wchar_t* argv[], wchar_t* envp[]) = 0;
    virtual void parse(int argc, wchar_t* argv[]) = 0;
    virtual void output_options_description() = 0;

    template<typename CharType>
    std::size_t variable_count(CharType const* name)
    {
        return variable_count(std::basic_string<CharType>(name));
    }
    template<typename CharType>
    std::size_t variable_count(std::basic_string<CharType> const& name)
    {
        return variable_count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
    }
    virtual std::size_t variable_count(const char8_t* name) = 0;

    /*!
     * 如果设置了有默认值的选项，那么即使命令行中没有指定选项，也会返回false
     */
    virtual bool variable_empty() = 0;

    template<typename T, typename CharType> 
    std::optional<std::reference_wrapper<T>> variable(CharType const* name)
    {
        return variable<T, CharType>(std::basic_string<CharType>(name));
    }
    template<typename T, typename CharType>
    std::optional<std::reference_wrapper<T>> variable(std::basic_string<CharType> const& name)
    {
        auto tmp = variable_as_any(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
        if (tmp)
        {
			std::reference_wrapper<boost::program_options::variable_value> variable_value
				= std::any_cast<std::reference_wrapper<boost::program_options::variable_value>>(*tmp);
            try
            {
                return std::ref(variable_value.get().as<T>());
            }
            catch (...)
            {
                return {};
            }
        }
        else
        {
            return {};
        }
    }
    virtual std::optional<std::any> variable_as_any(const char8_t* name) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_option(CharType const* name, CharType const* description)
    {
        add_option<CharType>(std::basic_string<CharType>(name)
                             , std::basic_string<CharType>(description));
    }
    template<typename CharType>
    void add_option(std::basic_string<CharType> const& name, std::basic_string<CharType> const& description)
    {
        add_option(
                    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
                    , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str());
    }
    virtual void add_option(const char8_t* name, const char8_t* description) = 0;

    /*!
     * 注意，这种选项在命令行中不需要指定值，有选项名就是true，否则就是false(如果有默认值那么按默认值)
     * 注意，default_value_设置为false跟没有设置一样
     */
    template<typename CharType>
    void add_bool_switch_option(CharType const* name
        , CharType const* description
        , std::optional<bool> default_value_ = {})
    {
        add_bool_switch_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_);
    }
    template<typename CharType>
    void add_bool_switch_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<bool> default_value_ = {})
    {
        add_bool_switch_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_);
    }
    virtual void add_bool_switch_option(const char8_t* name
        , const char8_t* description
        , std::optional<bool> default_value_ = {}) = 0;



    /*!
     *
     */
    template<typename CharType>
    void add_wstring_option(CharType const* name
        , CharType const* description
        , std::optional<std::wstring> default_value_ = {}
        , std::optional<std::wstring> implicit_value_ = {})
    {
        add_wstring_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_wstring_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::wstring> default_value_ = {}
        , std::optional<std::wstring> implicit_value_ = {})
    {
        add_wstring_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_wstring_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::wstring> default_value_ = {}
        , std::optional<std::wstring> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_wstring_option(CharType const* name
        , CharType const* description
        , std::optional<std::vector<std::wstring>> default_value_ = {}
        , std::optional<std::vector<std::wstring>> implicit_value_ = {})
    {
        add_vector_wstring_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_wstring_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<std::wstring>> default_value_ = {}
        , std::optional<std::vector<std::wstring>> implicit_value_ = {})
    {
        add_vector_wstring_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_wstring_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<std::wstring>> default_value_ = {}
        , std::optional<std::vector<std::wstring>> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_short_option(CharType const* name
        , CharType const* description
        , std::optional<short> default_value_
        , std::optional<short> implicit_value_)
    {
        add_short_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_short_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<short> default_value_ = {}
        , std::optional<short> implicit_value_ = {})
    {
        add_short_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_short_option(const char8_t* name
        , const char8_t* description
        , std::optional<short> default_value_ = {}
        , std::optional<short> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_short_option(CharType const* name
        , CharType const* description
        , std::optional<std::vector<short>> default_value_ = {}
        , std::optional<std::vector<short>> implicit_value_ = {})
    {
        add_vector_short_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_short_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<short>> default_value_ = {}
        , std::optional<std::vector<short>> implicit_value_ = {})
    {
        add_vector_short_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_short_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<short>> default_value_ = {}
        , std::optional<std::vector<short>> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_unsigned_short_option(CharType const* name
        , CharType const* description
        , std::optional<unsigned short> default_value_ = {}
        , std::optional<unsigned short> implicit_value_ = {})
    {
        add_unsigned_short_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_unsigned_short_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<unsigned short> default_value_ = {}
        , std::optional<unsigned short> implicit_value_ = {})
    {
        add_unsigned_short_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_unsigned_short_option(const char8_t* name
        , const char8_t* description
        , std::optional<unsigned short> default_value_ = {}
        , std::optional<unsigned short> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_unsigned_short_option(CharType const* name
        , CharType const* description
        , std::optional<std::vector<unsigned short>> default_value_ = {}
        , std::optional<std::vector<unsigned short>> implicit_value_ = {})
    {
        add_vector_unsigned_short_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_unsigned_short_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<unsigned short>> default_value_ = {}
        , std::optional<std::vector<unsigned short>> implicit_value_ = {})
    {
        add_vector_unsigned_short_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name)
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description)
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_unsigned_short_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<unsigned short>> default_value_ = {}
        , std::optional<std::vector<unsigned short>> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_int_option(CharType const* name
        , CharType const* description
        , std::optional<int> default_value_ = {}
        , std::optional<int> implicit_value_ = {})
    {
        add_int_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_int_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<int> default_value_ = {}
        , std::optional<int> implicit_value_ = {})
    {
        add_int_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_int_option(const char8_t* name
        , const char8_t* description
        , std::optional<int> default_value_ = {}
        , std::optional<int> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_int_option(CharType const* name
        , CharType const* description
        , std::optional<std::vector<int>> default_value_ = {}
        , std::optional<std::vector<int>> implicit_value_ = {})
    {
        add_vector_int_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_int_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<int>> default_value_ = {}
        , std::optional<std::vector<int>> implicit_value_ = {})
    {
        add_vector_int_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_int_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<int>> default_value_ = {}
        , std::optional<std::vector<int>> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_unsigned_int_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<unsigned int> default_value_ = {}
        , std::optional<unsigned int> implicit_value_ = {})
    {
        add_unsigned_int_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_unsigned_int_option(const char8_t* name
        , const char8_t* description
        , std::optional<unsigned int> default_value_ = {}
    , std::optional<unsigned int> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_unsigned_int_option(CharType const* name
        , CharType const* description
        , std::optional<std::vector<unsigned int>> default_value_ = {}
        , std::optional<std::vector<unsigned int>> implicit_value_ = {})
    {
        add_vector_unsigned_int_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_unsigned_int_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<unsigned int>> default_value_ = {}
        , std::optional<std::vector<unsigned int>> implicit_value_ = {})
    {
        add_vector_unsigned_int_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_unsigned_int_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<unsigned int>> default_value_ = {}
        , std::optional<std::vector<unsigned int>> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_long_long_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<long long> default_value_ = {}
        , std::optional<long long> implicit_value_ = {})
    {
        add_long_long_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_long_long_option(const char8_t* name
        , const char8_t* description
        , std::optional<long long> default_value_ = {}
    , std::optional<long long> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_long_long_option(CharType const* name
        , CharType const* description
        , std::optional<std::vector<long long>> default_value_ = {}
        , std::optional<std::vector<long long>> implicit_value_ = {})
    {
        add_vector_long_long_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_long_long_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<long long>> default_value_ = {}
        , std::optional<std::vector<long long>> implicit_value_ = {})
    {
        add_vector_long_long_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_long_long_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<long long>> default_value_ = {}
        , std::optional<std::vector<long long>> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_unsigned_long_long_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<unsigned long long> default_value_ = {}
        , std::optional<unsigned long long> implicit_value_ = {})
    {
        add_unsigned_long_long_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_unsigned_long_long_option(const char8_t* name
        , const char8_t* description
        , std::optional<unsigned long long> default_value_ = {}
        , std::optional<unsigned long long> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_unsigned_long_long_option(CharType const* name
        , CharType const* description
        , std::optional<std::deque<unsigned long long>> default_value_ = {}
        , std::optional<std::deque<unsigned long long>> implicit_value_ = {})
    {
        add_vector_unsigned_long_long_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_unsigned_long_long_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<unsigned long long>> default_value_ = {}
        , std::optional<std::vector<unsigned long long>> implicit_value_ = {})
    {
        add_vector_unsigned_long_long_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_unsigned_long_long_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<unsigned long long>> default_value_ = {}
        , std::optional<std::vector<unsigned long long>> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_float_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<float> default_value_ = {}
        , std::optional<float> implicit_value_ = {})
    {
        add_float_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_float_option(const char8_t* name
        , const char8_t* description
        , std::optional<float> default_value_ = {}
        , std::optional<float> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_float_option(CharType const* name
        , CharType const* description
        , std::optional<std::deque<float>> default_value_ = {}
        , std::optional<std::deque<float>> implicit_value_ = {})
    {
        add_vector_float_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_float_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<float>> default_value_ = {}
        , std::optional<std::vector<float>> implicit_value_ = {})
    {
        add_vector_float_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_float_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<float>> default_value_ = {}
        , std::optional<std::vector<float>> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_double_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<double> default_value_ = {}
        , std::optional<double> implicit_value_ = {})
    {
        add_double_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_double_option(const char8_t* name
        , const char8_t* description
        , std::optional<double> default_value_ = {}
    , std::optional<double> implicit_value_ = {}) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_vector_double_option(CharType const* name
        , CharType const* description
        , std::optional<std::vector<double>> default_value_ = {}
        , std::optional<std::vector<double>> implicit_value_ = {})
    {
        add_vector_double_option(std::basic_string<CharType>(name)
            , std::basic_string<CharType>(description)
            , default_value_
            , implicit_value_);
    }
    template<typename CharType>
    void add_vector_double_option(std::basic_string<CharType> const& name
        , std::basic_string<CharType> const& description
        , std::optional<std::vector<double>> default_value_ = {}
        , std::optional<std::vector<double>> implicit_value_ = {})
    {
        add_vector_double_option(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description).c_str()
            , default_value_
            , implicit_value_);
    }
    virtual void add_vector_double_option(const char8_t* name
        , const char8_t* description
        , std::optional<std::vector<double>> default_value_ = {}
        , std::optional<std::vector<double>> implicit_value_ = {}) = 0;

    virtual std::optional<std::any> variable_as_any_kiwi_fast_cpu_mask() = 0;

    std::optional<std::reference_wrapper<unsigned int>> variable_kiwi_fast_cpu_mask()
    {
        auto tmp = variable_as_any_kiwi_fast_cpu_mask();
        if (tmp)
        {
            return std::ref(std::any_cast<boost::program_options::variable_value>(*tmp).template as<unsigned int>());
        }
        else
        {
            return {};
        }
    }
};


KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
